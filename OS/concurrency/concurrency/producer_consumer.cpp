// producer_consumer.cpp — bounded-buffer producer/consumer in C++20
//
//   BoundedQueue<T>           mutex + two condition variables, close() for shutdown
//                             (the "monitor" version — what you'd write in an LLD round)
//   SemaphoreBoundedQueue<T>  empty/full counting semaphores + mutex, poison-pill shutdown
//                             (the classic OS-textbook version)
//
// Both use a fixed-size ring buffer.
//
// Build:  g++ -std=c++20 -O2 -Wall -Wextra -pthread producer_consumer.cpp -o producer_consumer
// Races:  add -fsanitize=thread -g   (or: make tsan)

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <optional>
#include <semaphore>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std::chrono_literals;
using Clock = std::chrono::steady_clock;

// ═════════════════════════════════════════════════════════════════════════════
// 1. Monitor-style bounded queue (mutex + condition variables)
// ═════════════════════════════════════════════════════════════════════════════
//
// Invariants (guarded by m_):
//   0 <= count_ <= cap_
//   live items occupy slots head_, head_+1, ..., head_+count_-1  (mod cap_)
//
// Why two condition variables?
//   Producers wait for "not full", consumers wait for "not empty". With one CV,
//   a push could wake another *producer* (which goes straight back to sleep)
//   while the consumer that could make progress stays asleep. You'd be forced
//   into notify_all everywhere to stay correct.
//
// Why wait with a predicate?
//   cv.wait(lk, pred) is  while (!pred()) cv.wait(lk);  The loop handles
//   spurious wakeups and "stolen" wakeups (another thread took the slot between
//   the notify and us re-acquiring the mutex).
template <typename T>
class BoundedQueue {
public:
    explicit BoundedQueue(std::size_t capacity) : buf_(capacity), cap_(capacity) {
        if (capacity == 0) throw std::invalid_argument("capacity must be > 0");
    }
    BoundedQueue(const BoundedQueue&) = delete;
    BoundedQueue& operator=(const BoundedQueue&) = delete;

    // Blocks while full. Returns false (item not enqueued) if the queue is closed.
    bool push(T item) {
        {
            std::unique_lock lk(m_);
            not_full_.wait(lk, [&] { return count_ < cap_ || closed_; });
            if (closed_) return false;
            enqueue(std::move(item));
        }
        // Notify after unlocking so the woken consumer doesn't immediately block on m_.
        // notify_one is enough: one new item can unblock at most one consumer.
        not_empty_.notify_one();
        return true;
    }

    // Blocks while empty. Returns nullopt only when closed AND drained,
    // so nothing that was successfully pushed is ever lost.
    std::optional<T> pop() {
        std::optional<T> out;
        {
            std::unique_lock lk(m_);
            not_empty_.wait(lk, [&] { return count_ > 0 || closed_; });
            if (count_ == 0) return std::nullopt;  // closed and drained
            out = dequeue();
        }
        not_full_.notify_one();
        return out;
    }

    // Non-blocking variants: fail immediately instead of waiting.
    bool try_push(T item) {
        {
            std::lock_guard lk(m_);
            if (closed_ || count_ == cap_) return false;
            enqueue(std::move(item));
        }
        not_empty_.notify_one();
        return true;
    }

    std::optional<T> try_pop() {
        std::optional<T> out;
        {
            std::lock_guard lk(m_);
            if (count_ == 0) return std::nullopt;
            out = dequeue();
        }
        not_full_.notify_one();
        return out;
    }

    // Wake everyone. Blocked/future producers fail; consumers drain what's left,
    // then get nullopt. This is the clean alternative to poison pills.
    void close() {
        {
            std::lock_guard lk(m_);
            closed_ = true;
        }
        not_full_.notify_all();
        not_empty_.notify_all();
    }

    std::size_t size() const {
        std::lock_guard lk(m_);
        return count_;
    }
    std::size_t capacity() const { return cap_; }

private:
    void enqueue(T&& item) {  // caller holds m_
        buf_[tail_].emplace(std::move(item));
        tail_ = (tail_ + 1) % cap_;
        ++count_;
    }

    T dequeue() {  // caller holds m_
        T item = std::move(*buf_[head_]);
        buf_[head_].reset();  // destroy the moved-from object now, not on overwrite
        head_ = (head_ + 1) % cap_;
        --count_;
        return item;
    }

    mutable std::mutex m_;
    std::condition_variable not_full_, not_empty_;
    std::vector<std::optional<T>> buf_;  // optional<T>: T needn't be default-constructible
    const std::size_t cap_;
    std::size_t head_ = 0, tail_ = 0, count_ = 0;
    bool closed_ = false;
};

// ═════════════════════════════════════════════════════════════════════════════
// 2. Semaphore version (the OS-textbook solution)
// ═════════════════════════════════════════════════════════════════════════════
//
//   empty_slots_  counts free slots    (starts at capacity)
//   full_slots_   counts filled slots  (starts at 0)
//   m_            protects head_/tail_ when there are several producers/consumers
//
//   producer:  P(empty)  lock  put   unlock  V(full)
//   consumer:  P(full)   lock  take  unlock  V(empty)
//
// ORDER MATTERS: acquire the semaphore *before* the mutex. If a producer locked
// m_ first and then blocked on a full buffer, it would sleep holding m_, no
// consumer could get in to free a slot, and everything deadlocks.
//
// With exactly ONE producer and ONE consumer the mutex can go: the producer only
// touches tail_, the consumer only head_, and the semaphores already order the
// slot writes/reads. That's the idea behind lock-free SPSC ring buffers.
//
// No close(): a semaphore can't "wake everyone and say quit", so shutdown uses
// poison pills (one sentinel item per consumer).
template <typename T>
class SemaphoreBoundedQueue {
public:
    explicit SemaphoreBoundedQueue(std::ptrdiff_t capacity)
        : cap_(validate(capacity)),
          buf_(static_cast<std::size_t>(cap_)),
          empty_slots_(cap_),
          full_slots_(0) {}
    SemaphoreBoundedQueue(const SemaphoreBoundedQueue&) = delete;
    SemaphoreBoundedQueue& operator=(const SemaphoreBoundedQueue&) = delete;

    void push(T item) {
        empty_slots_.acquire();  // P(empty): wait for a free slot
        {
            std::lock_guard lk(m_);
            buf_[tail_].emplace(std::move(item));
            tail_ = (tail_ + 1) % buf_.size();
        }
        full_slots_.release();   // V(full): one more item available
    }

    T pop() {
        full_slots_.acquire();   // P(full): wait for an item
        std::optional<T> item;
        {
            std::lock_guard lk(m_);
            item.emplace(std::move(*buf_[head_]));
            buf_[head_].reset();
            head_ = (head_ + 1) % buf_.size();
        }
        empty_slots_.release();  // V(empty): one more free slot
        return std::move(*item);
    }

private:
    static std::ptrdiff_t validate(std::ptrdiff_t c) {
        if (c <= 0) throw std::invalid_argument("capacity must be > 0");
        return c;
    }

    const std::ptrdiff_t cap_;
    std::vector<std::optional<T>> buf_;
    std::counting_semaphore<> empty_slots_;
    std::counting_semaphore<> full_slots_;
    std::mutex m_;
    std::size_t head_ = 0, tail_ = 0;
};

// ═════════════════════════════════════════════════════════════════════════════
// Test harness
// ═════════════════════════════════════════════════════════════════════════════

// Thread-safe timestamped printf.
class Logger {
public:
    void operator()(const char* fmt, ...) {
        std::lock_guard lk(m_);  // timestamp under the lock so output lines are in time order
        const double ms = std::chrono::duration<double, std::milli>(Clock::now() - t0_).count();
        std::printf("  [%6.1f ms] ", ms);
        va_list ap;
        va_start(ap, fmt);
        std::vprintf(fmt, ap);
        va_end(ap);
        std::putchar('\n');
    }

private:
    Clock::time_point t0_ = Clock::now();
    std::mutex m_;
};

int g_failures = 0;

void check(bool cond, const char* what) {
    std::printf("  %s  %s\n", cond ? "ok  " : "FAIL", what);
    if (!cond) ++g_failures;
}

// Single-threaded checks of the API contract.
void unit_checks() {
    std::puts("== Unit checks: BoundedQueue API contract ==");
    BoundedQueue<int> q(2);
    check(!q.try_pop().has_value(), "try_pop on empty queue fails");
    check(q.try_push(1) && q.try_push(2), "try_push succeeds up to capacity");
    check(!q.try_push(3), "try_push on full queue fails");
    check(q.try_pop() == 1, "items come out in FIFO order (1)");
    check(q.push(3), "push succeeds once a slot frees up");
    q.close();
    check(!q.push(4), "push after close() returns false");
    check(q.pop() == 2 && q.pop() == 3, "pop after close() still drains remaining items");
    check(!q.pop().has_value(), "pop on closed + drained queue returns nullopt (no block)");

    // A consumer blocked in pop() must be released by close().
    BoundedQueue<int> q2(1);
    std::atomic<bool> released{false};
    std::jthread waiter([&] {
        (void)q2.pop();
        released = true;
    });
    std::this_thread::sleep_for(20ms);
    q2.close();
    waiter.join();
    check(released, "close() wakes a consumer blocked in pop()");
    std::puts("");
}

// Fast producer, slow consumer, tiny buffer: watch the producer get throttled.
void trace_demo() {
    std::puts("== Trace: 1 fast producer, 1 slow consumer (20 ms/item), capacity 3 ==");
    std::puts("   The producer races 3-4 items ahead, then blocks and is paced by the consumer.");
    BoundedQueue<int> q(3);
    Logger log;
    {
        std::jthread consumer([&] {
            while (auto v = q.pop()) {
                log("                           consumer popped %d", *v);
                std::this_thread::sleep_for(20ms);
            }
            log("                           consumer: queue closed and drained, exiting");
        });
        std::jthread producer([&] {
            for (int i = 0; i < 8; ++i) {
                q.push(i);
                log("producer pushed %d", i);
            }
            q.close();
            log("producer closed the queue");
        });
    }
    std::puts("");
}

// Items encode (producer, seq) as producer * per_producer + seq. Checks:
//   - every item consumed exactly once (no loss, no duplicates)
//   - per-producer FIFO: each consumer sees a given producer's items in increasing order
class Verifier {
public:
    Verifier(int producers, int per_producer)
        : producers_(producers), per_(per_producer),
          seen_(static_cast<std::size_t>(producers) * static_cast<std::size_t>(per_producer)) {}

    // `last` is the calling consumer's private "highest seq seen per producer".
    void record(int item, std::vector<int>& last) {
        const int p = item / per_, seq = item % per_;
        if (seq <= last[p]) order_errors_.fetch_add(1, std::memory_order_relaxed);
        last[p] = seq;
        if (seen_[item].fetch_add(1, std::memory_order_relaxed) != 0)
            duplicates_.fetch_add(1, std::memory_order_relaxed);
    }

    std::vector<int> fresh_last() const { return std::vector<int>(producers_, -1); }

    void report(const char* label, double ms) const {
        const long missing = std::count_if(seen_.begin(), seen_.end(),
                                           [](const std::atomic<int>& c) { return c.load() == 0; });
        const bool ok = missing == 0 && duplicates_ == 0 && order_errors_ == 0;
        std::printf("  %-40s %8zu items %8.1f ms   missing=%ld dup=%ld order_err=%ld  %s\n", label,
                    seen_.size(), ms, missing, duplicates_.load(), order_errors_.load(),
                    ok ? "PASS" : "FAIL");
        if (!ok) ++g_failures;
    }

private:
    int producers_, per_;
    std::vector<std::atomic<int>> seen_;
    std::atomic<long> duplicates_{0}, order_errors_{0};
};

double ms_since(Clock::time_point t0) {
    return std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
}

void stress_cv_queue(int P, int C, int per, std::size_t cap) {
    BoundedQueue<int> q(cap);
    Verifier v(P, per);
    const auto t0 = Clock::now();
    {
        std::vector<std::jthread> consumers;
        for (int c = 0; c < C; ++c)
            consumers.emplace_back([&] {
                auto last = v.fresh_last();
                while (auto item = q.pop()) v.record(*item, last);
            });
        {
            std::vector<std::jthread> producers;
            for (int p = 0; p < P; ++p)
                producers.emplace_back([&, p] {
                    for (int s = 0; s < per; ++s) q.push(p * per + s);
                });
        }  // all producers joined here
        q.close();  // consumers drain the rest, then pop() returns nullopt
    }  // all consumers joined here
    char label[64];
    std::snprintf(label, sizeof label, "cv queue        %dP / %dC  cap=%zu", P, C, cap);
    v.report(label, ms_since(t0));
}

void stress_semaphore_queue(int P, int C, int per, std::ptrdiff_t cap) {
    constexpr int kPoison = -1;
    SemaphoreBoundedQueue<int> q(cap);
    Verifier v(P, per);
    const auto t0 = Clock::now();
    {
        std::vector<std::jthread> consumers;
        for (int c = 0; c < C; ++c)
            consumers.emplace_back([&] {
                auto last = v.fresh_last();
                for (int item; (item = q.pop()) != kPoison;) v.record(item, last);
            });
        {
            std::vector<std::jthread> producers;
            for (int p = 0; p < P; ++p)
                producers.emplace_back([&, p] {
                    for (int s = 0; s < per; ++s) q.push(p * per + s);
                });
        }
        // All real items are already queued and the queue is FIFO, so every
        // real item is popped before any pill. Each consumer eats exactly one pill.
        for (int c = 0; c < C; ++c) q.push(kPoison);
    }
    char label[64];
    std::snprintf(label, sizeof label, "semaphore queue %dP / %dC  cap=%td", P, C, cap);
    v.report(label, ms_since(t0));
}

int main() {
    unit_checks();
    trace_demo();

    std::puts("== Stress: exactly-once delivery + per-producer FIFO ==");
    // Timings are for curiosity only. On Linux/libstdc++ the semaphore version is
    // usually faster: its acquire() spins briefly in user space before sleeping,
    // whereas condition_variable::wait always goes to sleep on a futex.
    stress_cv_queue(4, 4, 50'000, 16);
    stress_cv_queue(8, 2, 5'000, 1);  // capacity 1: every item is a hand-off
    stress_cv_queue(1, 8, 100'000, 64);
    stress_semaphore_queue(4, 4, 50'000, 16);
    stress_semaphore_queue(8, 2, 5'000, 1);
    stress_semaphore_queue(1, 8, 100'000, 64);

    std::printf("\n%s\n", g_failures == 0 ? "ALL CHECKS PASSED" : "SOME CHECKS FAILED");
    return g_failures == 0 ? 0 : 1;
}
