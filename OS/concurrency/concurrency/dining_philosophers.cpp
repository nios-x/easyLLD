// dining_philosophers.cpp — dining philosophers in C++20
//
// N = 5 philosophers around a table, one fork between each pair.
// Philosopher i needs fork i (left) and fork (i+1) % N (right) to eat.
//
// Deadlock needs all four Coffman conditions at once:
//   mutual exclusion · hold-and-wait · no preemption · circular wait
// The naive "left fork, then right fork" has all four. Each fix breaks one:
//
//   OrderedForks  always lock the lower-numbered fork first     → no circular wait
//   ScopedLock    std::scoped_lock: lock both or back off       → no hold-and-wait
//   Waiter        semaphore lets at most N-1 try at once        → no circular wait
//   Monitor       eat only when neither neighbour is eating     → no hold-and-wait
//                 (Tanenbaum / Silberschatz state-machine solution)
//
//   ./dining_philosophers             run the four solutions with live safety checks
//   ./dining_philosophers --deadlock  run the naive version and watch it deadlock
//
// Build:  g++ -std=c++20 -O2 -Wall -Wextra -pthread dining_philosophers.cpp -o dining_philosophers

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <random>
#include <semaphore>
#include <thread>
#include <vector>

using namespace std::chrono_literals;
using Clock = std::chrono::steady_clock;

constexpr int N = 5;
constexpr int left_fork(int i) { return i; }
constexpr int right_fork(int i) { return (i + 1) % N; }
constexpr int left_neighbour(int i) { return (i + N - 1) % N; }
constexpr int right_neighbour(int i) { return (i + 1) % N; }

// The forks, plus live checks on who is eating.
class Table {
public:
    std::array<std::mutex, N> forks;

    void start_eating(int i) {
        eating_[i].store(true);
        // Two neighbours share a fork, so they must never eat at the same time.
        // (seq_cst store-then-load on both sides: if both eat, at least one sees it.)
        if (eating_[left_neighbour(i)].load() || eating_[right_neighbour(i)].load()) ++violations_;
        const int now = ++eaters_;
        int m = max_eaters_.load();
        while (now > m && !max_eaters_.compare_exchange_weak(m, now)) {}
    }
    void stop_eating(int i) {
        --eaters_;
        eating_[i].store(false);
    }

    long violations() const { return violations_.load(); }
    int max_eaters() const { return max_eaters_.load(); }  // can never exceed N/2 = 2

private:
    std::array<std::atomic<bool>, N> eating_{};
    std::atomic<int> eaters_{0}, max_eaters_{0};
    std::atomic<long> violations_{0};
};

// ═════════════════════════════════════════════════════════════════════════════
// Solutions. Each has  dine(i, eat)  which acquires what philosopher i needs,
// calls eat(), and releases.
// ═════════════════════════════════════════════════════════════════════════════

// 1. Resource ordering: number the forks, always take the lower number first.
//    Philosophers 0..3 take left then right; philosopher 4 takes fork 0 (right)
//    before fork 4 (left). A cycle in the wait-for graph would need someone to
//    hold a higher fork while waiting for a lower one — impossible here.
class OrderedForks {
public:
    explicit OrderedForks(Table& t) : t_(t) {}
    template <class Eat>
    void dine(int i, Eat&& eat) {
        const int first = std::min(left_fork(i), right_fork(i));
        const int second = std::max(left_fork(i), right_fork(i));
        std::lock_guard a(t_.forks[first]);
        std::lock_guard b(t_.forks[second]);
        eat();
    }

private:
    Table& t_;
};

// 2. std::scoped_lock (C++17) locks several mutexes with a deadlock-avoidance
//    algorithm: it never *blocks* while holding one of them — if a try_lock
//    fails it releases what it has and retries. Simplest correct answer in C++.
class ScopedLock {
public:
    explicit ScopedLock(Table& t) : t_(t) {}
    template <class Eat>
    void dine(int i, Eat&& eat) {
        std::scoped_lock both(t_.forks[left_fork(i)], t_.forks[right_fork(i)]);
        eat();
    }

private:
    Table& t_;
};

// 3. Waiter / limited seating: a counting semaphore with N-1 permits. With at
//    most 4 philosophers competing for 5 forks, by pigeonhole at least one of
//    them can get both, so the circular wait can't close.
//    Note: the lock order itself is still cyclic (0→1→2→3→4→0). ThreadSanitizer
//    (and lockdep-style checkers in general) flag this as a potential deadlock
//    because they can't see the semaphore invariant. OrderedForks has no such cycle.
class Waiter {
public:
    explicit Waiter(Table& t) : t_(t) {}
    template <class Eat>
    void dine(int i, Eat&& eat) {
        seats_.acquire();
        {
            std::lock_guard l(t_.forks[left_fork(i)]);  // naive order is fine now
            std::lock_guard r(t_.forks[right_fork(i)]);
            eat();
        }
        seats_.release();
    }

private:
    Table& t_;
    std::counting_semaphore<N> seats_{N - 1};
};

// 4. Monitor (Tanenbaum / Silberschatz): no fork mutexes at all. One mutex guards
//    everyone's state; a hungry philosopher sleeps on its own CV until neither
//    neighbour is eating, and grabs "both forks" atomically by switching to Eating.
//    Deadlock-free, but a philosopher CAN starve if its two neighbours keep
//    alternating meals (fix: add aging/priority, or use Chandy–Misra).
class Monitor {
public:
    explicit Monitor(Table&) {}
    template <class Eat>
    void dine(int i, Eat&& eat) {
        {
            std::unique_lock lk(m_);
            state_[i] = State::Hungry;
            self_[i].wait(lk, [&] {
                return state_[left_neighbour(i)] != State::Eating &&
                       state_[right_neighbour(i)] != State::Eating;
            });
            state_[i] = State::Eating;
        }
        eat();
        {
            std::lock_guard lk(m_);
            state_[i] = State::Thinking;
        }
        // Our neighbours are the only ones whose condition may have just become true.
        self_[left_neighbour(i)].notify_one();
        self_[right_neighbour(i)].notify_one();
    }

private:
    enum class State { Thinking, Hungry, Eating };
    std::mutex m_;
    std::array<State, N> state_{};  // all Thinking
    std::array<std::condition_variable, N> self_;
};

// ═════════════════════════════════════════════════════════════════════════════
// Harness
// ═════════════════════════════════════════════════════════════════════════════

long g_violations = 0;

template <class Strategy>
void run(const char* name, std::chrono::milliseconds duration) {
    Table table;
    Strategy strategy(table);
    std::array<long, N> meals{};
    std::array<double, N> max_wait_ms{};  // longest hungry → eating gap per philosopher

    {
        std::vector<std::jthread> philosophers;
        for (int i = 0; i < N; ++i)
            philosophers.emplace_back([&, i](std::stop_token st) {
                std::mt19937 rng(1234 + i);
                std::uniform_int_distribution<int> think_us(0, 2000), eat_us(500, 1500);
                while (!st.stop_requested()) {
                    std::this_thread::sleep_for(std::chrono::microseconds(think_us(rng)));  // think
                    const auto hungry_at = Clock::now();
                    strategy.dine(i, [&] {
                        const double waited =
                            std::chrono::duration<double, std::milli>(Clock::now() - hungry_at).count();
                        max_wait_ms[i] = std::max(max_wait_ms[i], waited);
                        table.start_eating(i);
                        std::this_thread::sleep_for(std::chrono::microseconds(eat_us(rng)));  // eat
                        table.stop_eating(i);
                    });
                    ++meals[i];  // only thread i writes meals[i]; read after join
                }
            });
        std::this_thread::sleep_for(duration);
    }  // jthread destructors: request_stop() then join()

    long total = 0;
    std::printf("  %-14s [", name);
    for (long m : meals) {
        std::printf(" %4ld", m);
        total += m;
    }
    const auto [lo, hi] = std::minmax_element(meals.begin(), meals.end());
    std::printf(" ]  %5ld   %4ld/%-4ld  %6.1f ms   %d          %ld\n", total, *lo, *hi,
                *std::max_element(max_wait_ms.begin(), max_wait_ms.end()), table.max_eaters(),
                table.violations());
    g_violations += table.violations();
}

// Naive: everyone takes the left fork, then the right. The sleep in between makes
// the bad interleaving certain instead of "eventually": all five hold their left
// fork, all five wait for their right, which is someone else's left. Circular wait.
[[noreturn]] void deadlock_demo() {
    std::puts("== Naive solution: take left fork, then right fork ==\n");
    Table table;
    std::atomic<long> meals{0};
    std::mutex io;
    std::vector<std::thread> philosophers;  // plain threads: they never finish, we never join
    for (int i = 0; i < N; ++i)
        philosophers.emplace_back([&, i] {
            for (;;) {
                std::lock_guard l(table.forks[left_fork(i)]);
                {
                    std::lock_guard g(io);
                    std::printf("  P%d holds fork %d, reaching for fork %d\n", i, left_fork(i), right_fork(i));
                }
                std::this_thread::sleep_for(100ms);  // widen the race window
                std::lock_guard r(table.forks[right_fork(i)]);
                ++meals;
            }
        });

    // Watchdog: if no meal completes for a full second, nobody is making progress.
    long last = -1;
    for (;;) {
        std::this_thread::sleep_for(1s);
        const long now = meals.load();
        if (now == last) break;
        last = now;
    }
    {
        std::lock_guard g(io);
        std::printf("\n  Watchdog: no meal finished in 1 s — DEADLOCK.\n");
        std::printf("  Wait-for cycle: ");
        for (int i = 0; i < N; ++i) std::printf("P%d -> ", i);
        std::printf("P0   (each waits on the fork its right neighbour holds)\n");
        std::printf("  All four Coffman conditions hold: mutual exclusion, hold-and-wait,\n"
                    "  no preemption, circular wait.\n");
        std::printf("  The threads are stuck forever and can't be joined, so exiting with _Exit.\n");
        std::fflush(stdout);
    }
    std::_Exit(0);
}

int main(int argc, char** argv) {
    const char* only = argc > 1 ? argv[1] : nullptr;  // optionally run a single solution
    if (only && std::strcmp(only, "--deadlock") == 0) deadlock_demo();
    auto want = [&](const char* key) { return !only || std::strcmp(only, key) == 0; };
    if (only && !want("ordered") && !want("scoped") && !want("waiter") && !want("monitor")) {
        std::fprintf(stderr, "usage: %s [ordered|scoped|waiter|monitor|--deadlock]\n", argv[0]);
        return 2;
    }

    constexpr auto kDuration = 1000ms;
    std::printf("== %d philosophers, %lld ms per solution (think 0-2 ms, eat 0.5-1.5 ms) ==\n\n", N,
                static_cast<long long>(kDuration.count()));
    std::printf("  %-14s  %-26s  %5s   %-9s  %-9s  %-9s  %s\n", "solution", "meals per philosopher",
                "total", "min/max", "max wait", "max eating", "violations");
    if (want("ordered")) run<OrderedForks>("ordered forks", kDuration);
    if (want("scoped")) run<ScopedLock>("scoped_lock", kDuration);
    if (want("waiter")) run<Waiter>("waiter (N-1)", kDuration);
    if (want("monitor")) run<Monitor>("monitor", kDuration);

    std::puts("\n  max wait   = longest any philosopher stayed hungry (bounded → no starvation in this run)");
    std::puts("  max eating = most philosophers eating at once (at most 2 of 5 is possible)");
    std::puts("  violations = two neighbours eating at the same time (must be 0)");
    std::puts("  Try --deadlock to see the naive solution lock up.");
    std::printf("\n%s\n", g_violations == 0 ? "NO NEIGHBOURS EVER ATE TOGETHER" : "VIOLATIONS DETECTED");
    return g_violations == 0 ? 0 : 1;
}
