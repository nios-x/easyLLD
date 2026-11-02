// reader_writer.cpp — readers–writers problem in C++20
//
// Three hand-built reader–writer locks, plus std::shared_mutex for comparison:
//
//   ReaderPrefRWLock   "first readers–writers problem"   readers never wait for a
//                      *waiting* writer          → writers can starve
//   WriterPrefRWLock   "second readers–writers problem"  new readers queue behind a
//                      waiting writer            → readers can starve
//   FairRWLock         ticket (FIFO) order; consecutive readers go in together
//                                                → nobody starves
//   std::shared_mutex  what you'd use in real code; its policy is implementation-defined
//
// Each lock provides lock()/unlock()/lock_shared()/unlock_shared(), which is all
// std::unique_lock and std::shared_lock need here.
//
// The program runs every lock under a read-heavy and a write-heavy workload, checks
// mutual exclusion on every critical-section entry, and reports how long readers and
// writers waited — the starvation shows up in the "max wait" columns.
//
// Build:  g++ -std=c++20 -O2 -Wall -Wextra -pthread reader_writer.cpp -o reader_writer

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

using namespace std::chrono_literals;
using Clock = std::chrono::steady_clock;

// ═════════════════════════════════════════════════════════════════════════════
// 1. Reader-preferring lock
// ═════════════════════════════════════════════════════════════════════════════
// A reader gets in whenever no writer is *active*. If readers keep overlapping,
// readers_ never drops to 0 and a waiting writer waits forever.
class ReaderPrefRWLock {
public:
    void lock_shared() {
        std::unique_lock lk(m_);
        cv_.wait(lk, [&] { return !writer_; });
        ++readers_;
    }
    void unlock_shared() {
        bool last;
        {
            std::lock_guard lk(m_);
            last = (--readers_ == 0);
        }
        if (last) cv_.notify_all();  // only the last reader out can let a writer in
    }
    void lock() {
        std::unique_lock lk(m_);
        cv_.wait(lk, [&] { return !writer_ && readers_ == 0; });
        writer_ = true;
    }
    void unlock() {
        {
            std::lock_guard lk(m_);
            writer_ = false;
        }
        cv_.notify_all();  // wake all readers (and writers; one of them may win)
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    int readers_ = 0;
    bool writer_ = false;
};

// ═════════════════════════════════════════════════════════════════════════════
// 2. Writer-preferring lock
// ═════════════════════════════════════════════════════════════════════════════
// Once a writer is waiting, new readers are held back so the current readers can
// drain. If writers keep arriving, waiting_writers_ never hits 0 and readers starve.
// Separate CVs let us wake exactly the side we want.
class WriterPrefRWLock {
public:
    void lock_shared() {
        std::unique_lock lk(m_);
        readers_cv_.wait(lk, [&] { return !writer_ && waiting_writers_ == 0; });
        ++readers_;
    }
    void unlock_shared() {
        bool wake_writer;
        {
            std::lock_guard lk(m_);
            wake_writer = (--readers_ == 0 && waiting_writers_ > 0);
        }
        if (wake_writer) writers_cv_.notify_one();
    }
    void lock() {
        std::unique_lock lk(m_);
        ++waiting_writers_;
        writers_cv_.wait(lk, [&] { return !writer_ && readers_ == 0; });
        --waiting_writers_;
        writer_ = true;
    }
    void unlock() {
        bool writers_waiting;
        {
            std::lock_guard lk(m_);
            writer_ = false;
            writers_waiting = waiting_writers_ > 0;
        }
        if (writers_waiting)
            writers_cv_.notify_one();   // hand off to the next writer
        else
            readers_cv_.notify_all();   // no writers left: release every waiting reader
    }

private:
    std::mutex m_;
    std::condition_variable readers_cv_, writers_cv_;
    int readers_ = 0, waiting_writers_ = 0;
    bool writer_ = false;
};

// ═════════════════════════════════════════════════════════════════════════════
// 3. Fair (FIFO ticket) lock
// ═════════════════════════════════════════════════════════════════════════════
// Every arrival takes a ticket and waits until it's at the head of the line.
//   reader at head: enters once no writer is active, then immediately advances
//                   serving_ so a reader right behind it can join (readers batch)
//   writer at head: enters once no writer is active AND all readers have left
// A later arrival can never overtake an earlier one, so nobody starves.
//
// notify_all wakes every waiter to re-check its ticket (thundering herd). A
// production version would keep a queue of per-waiter CVs instead.
class FairRWLock {
public:
    void lock_shared() {
        std::unique_lock lk(m_);
        const std::uint64_t ticket = next_ticket_++;
        cv_.wait(lk, [&] { return ticket == serving_ && !writer_; });
        ++readers_;
        ++serving_;  // let the next in line (if it's a reader) in alongside us
        lk.unlock();
        cv_.notify_all();
    }
    void unlock_shared() {
        bool last;
        {
            std::lock_guard lk(m_);
            last = (--readers_ == 0);
        }
        if (last) cv_.notify_all();
    }
    void lock() {
        std::unique_lock lk(m_);
        const std::uint64_t ticket = next_ticket_++;
        cv_.wait(lk, [&] { return ticket == serving_ && !writer_ && readers_ == 0; });
        writer_ = true;
        ++serving_;  // next in line can now queue at the head (it still waits for writer_)
    }
    void unlock() {
        {
            std::lock_guard lk(m_);
            writer_ = false;
        }
        cv_.notify_all();
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    std::uint64_t next_ticket_ = 0, serving_ = 0;
    int readers_ = 0;
    bool writer_ = false;
};

// ═════════════════════════════════════════════════════════════════════════════
// Test harness
// ═════════════════════════════════════════════════════════════════════════════

struct Workload {
    const char* title;
    int readers, writers;
    std::chrono::microseconds read_hold, write_hold;      // time spent inside the lock
    std::chrono::microseconds reader_pause, writer_pause; // gap between attempts (0 = back-to-back)
    std::chrono::milliseconds duration;
};

// Live checks, updated on every critical-section entry/exit.
struct Occupancy {
    std::atomic<int> readers{0}, writers{0}, max_readers{0};
    std::atomic<long> violations{0};

    void enter_read() {
        const int r = ++readers;
        if (writers.load() != 0) ++violations;  // reader inside with a writer
        int m = max_readers.load();
        while (r > m && !max_readers.compare_exchange_weak(m, r)) {}
    }
    void exit_read() { --readers; }
    void enter_write() {
        if (++writers != 1 || readers.load() != 0) ++violations;  // writer not alone
    }
    void exit_write() { --writers; }
};

struct WaitStats {
    long ops = 0;
    double total_us = 0, max_us = 0;
    void add(double us) {
        ++ops;
        total_us += us;
        max_us = std::max(max_us, us);
    }
    void merge(const WaitStats& o) {
        ops += o.ops;
        total_us += o.total_us;
        max_us = std::max(max_us, o.max_us);
    }
    double avg_ms() const { return ops ? total_us / ops / 1000.0 : 0.0; }
    double max_ms() const { return max_us / 1000.0; }
};

double us_since(Clock::time_point t0) {
    return std::chrono::duration<double, std::micro>(Clock::now() - t0).count();
}

long g_violations = 0;

template <class RW>
void run(const char* name, const Workload& w) {
    RW lock;
    long a = 0, b = 0;  // writers keep a == b; a reader seeing a != b means a torn read
    Occupancy occ;
    std::atomic<bool> stop{false};
    std::atomic<long> torn_reads{0};
    std::mutex merge_m;
    WaitStats reads, writes;

    {
        std::vector<std::jthread> threads;
        for (int i = 0; i < w.readers; ++i)
            threads.emplace_back([&] {
                WaitStats mine;
                while (!stop.load(std::memory_order_relaxed)) {
                    const auto t0 = Clock::now();
                    std::shared_lock lk(lock);
                    mine.add(us_since(t0));
                    occ.enter_read();
                    const long seen_a = a;
                    std::this_thread::sleep_for(w.read_hold);  // simulate a slow read
                    const long seen_b = b;
                    if (seen_a != seen_b) ++torn_reads;
                    occ.exit_read();
                    lk.unlock();
                    if (w.reader_pause.count()) std::this_thread::sleep_for(w.reader_pause);
                }
                std::lock_guard g(merge_m);
                reads.merge(mine);
            });
        for (int i = 0; i < w.writers; ++i)
            threads.emplace_back([&] {
                WaitStats mine;
                while (!stop.load(std::memory_order_relaxed)) {
                    const auto t0 = Clock::now();
                    std::unique_lock lk(lock);
                    mine.add(us_since(t0));
                    occ.enter_write();
                    ++a;
                    std::this_thread::sleep_for(w.write_hold);  // a reader in here would see a != b
                    ++b;
                    occ.exit_write();
                    lk.unlock();
                    if (w.writer_pause.count()) std::this_thread::sleep_for(w.writer_pause);
                }
                std::lock_guard g(merge_m);
                writes.merge(mine);
            });
        std::this_thread::sleep_for(w.duration);
        stop = true;
    }  // join

    const long bad = occ.violations + torn_reads;
    g_violations += bad;
    std::printf("  %-18s %7ld %7ld %6d     %7.2f / %7.2f       %7.2f / %7.2f     %ld\n", name, reads.ops,
                writes.ops, occ.max_readers.load(), reads.avg_ms(), reads.max_ms(), writes.avg_ms(),
                writes.max_ms(), bad);
}

void run_all(const Workload& w) {
    std::printf("\n== %s ==\n", w.title);
    std::printf("   %d readers (hold %lld us, pause %lld us), %d writers (hold %lld us, pause %lld us), %lld ms\n",
                w.readers, static_cast<long long>(w.read_hold.count()),
                static_cast<long long>(w.reader_pause.count()), w.writers,
                static_cast<long long>(w.write_hold.count()), static_cast<long long>(w.writer_pause.count()),
                static_cast<long long>(w.duration.count()));
    std::printf("  %-18s %7s %7s %6s     %-19s       %-19s     %s\n", "lock", "reads", "writes", "max-R",
                "reader wait avg/max", "writer wait avg/max", "violations");
    std::printf("  %-18s %7s %7s %6s     %-19s       %-19s\n", "", "", "", "", "      (ms)", "      (ms)");
    run<ReaderPrefRWLock>("reader-preferring", w);
    run<WriterPrefRWLock>("writer-preferring", w);
    run<FairRWLock>("fair (ticket)", w);
    run<std::shared_mutex>("std::shared_mutex", w);
}

int main() {
    // Readers overlap constantly → a reader-preferring lock rarely sees readers_ == 0.
    run_all({"Read-heavy: readers back-to-back, one occasional writer", 6, 1, 200us, 50us, 0us, 1000us, 500ms});

    // Writers queue constantly → a writer-preferring lock always has waiting_writers_ > 0.
    run_all({"Write-heavy: writers back-to-back, occasional readers", 3, 3, 50us, 200us, 1000us, 0us, 500ms});

    std::puts("\n  max-R      = most readers inside at once (> 1 shows reads really run in parallel)");
    std::puts("  violations = reader+writer or writer+writer inside together, or a torn read (must be 0)");
    std::printf("\n%s\n", g_violations == 0 ? "MUTUAL EXCLUSION HELD IN EVERY RUN" : "VIOLATIONS DETECTED");
    return g_violations == 0 ? 0 : 1;
}
