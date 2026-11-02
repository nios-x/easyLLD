# Classic concurrency problems in C++20

Three self-contained programs. Each one checks its own correctness while it runs, so a
passing run is evidence, not just output.

| File | What's inside | What it checks |
|---|---|---|
| `producer_consumer.cpp` | `BoundedQueue<T>` (mutex + 2 CVs, `close()`), `SemaphoreBoundedQueue<T>` (textbook empty/full semaphores, poison pills) | every item consumed exactly once; per-producer FIFO; `close()` semantics |
| `reader_writer.cpp` | reader-preferring, writer-preferring and fair (ticket) RW locks, plus `std::shared_mutex` | no reader alongside a writer, no two writers, no torn reads; wait times per side |
| `dining_philosophers.cpp` | ordered forks, `std::scoped_lock`, waiter (N−1 semaphore), monitor; `--deadlock` naive demo | no two neighbours eat at once; ≤ 2 eaters; longest hungry wait |

## Build and run

Needs a C++20 compiler (g++ ≥ 11 or clang++ ≥ 14).

```bash
make            # build all three
make run        # run all three
make tsan       # build with ThreadSanitizer and run (reports any data race)

./dining_philosophers --deadlock   # watch the naive version deadlock
./dining_philosophers waiter       # run one solution: ordered | scoped | waiter | monitor
```

Without make: `g++ -std=c++20 -O2 -Wall -Wextra -pthread file.cpp -o file`

---

## 1. Producer–consumer (bounded buffer)

**Core idea:** a ring buffer guarded by one mutex. Producers wait on `not_full`, consumers wait on `not_empty`.

What to look for in the output: in the trace section the producer runs 3–4 items ahead, then gets paced to one push per pop. That's backpressure.

Talking points:
- **Why `while`, not `if`, around `wait`?** Spurious wakeups, and stolen wakeups: another thread can take the slot between the notify and you re-acquiring the mutex. `cv.wait(lk, pred)` is that loop.
- **Why two CVs?** With one, a push might wake another producer instead of a consumer. You'd need `notify_all` to stay correct.
- **`notify_one` vs `notify_all`:** one item can unblock at most one consumer, so `notify_one`. `close()` changes the condition for everyone, so it uses `notify_all`.
- **Semaphore version, order matters:** `P(empty)` *before* locking the mutex. The reverse order sleeps while holding the mutex, and everything deadlocks.
- **Shutdown:** `close()` (drain, then return `nullopt`) vs poison pills (one sentinel per consumer; works because the queue is FIFO).
- **SPSC:** with one producer and one consumer the mutex can go. That's the basis of lock-free ring buffers.

## 2. Readers–writers

| Lock | Rule | Who can starve |
|---|---|---|
| Reader-preferring | readers enter whenever no writer is *active* | writers |
| Writer-preferring | a *waiting* writer blocks new readers | readers |
| Fair (ticket) | strict arrival order; consecutive readers batch together | nobody |
| `std::shared_mutex` | implementation-defined | depends on platform |

What to look for in the output: the **max wait** columns.
- Read-heavy workload: the reader-preferring writer waits the whole run and gets about 1 write.
- Write-heavy workload: writer-preferring readers do the same.
- Fair lock: both sides' max waits stay at a few milliseconds.

On Linux/glibc, `std::shared_mutex` behaved like the reader-preferring lock here.

Talking points:
- Name them: "first" (reader-pref) and "second" (writer-pref) readers–writers problems.
- The ticket lock's fairness comes from never letting a later arrival overtake an earlier one.
- It uses `notify_all`, so it has a thundering-herd cost. A production lock keeps a queue of per-waiter CVs.
- In real code, use `std::shared_mutex` + `std::shared_lock` / `std::unique_lock`. A RW lock only pays off when reads are long or frequent. For tiny critical sections a plain mutex is often faster.

## 3. Dining philosophers

Deadlock needs all four **Coffman conditions**: mutual exclusion, hold-and-wait, no preemption, circular wait.

| Solution | Breaks | Notes |
|---|---|---|
| Ordered forks | circular wait | lowest-numbered fork first; slightly unfair (P0 and P4 both start with fork 0) |
| `std::scoped_lock` | hold-and-wait | try-lock-and-back-off; the idiomatic C++ answer |
| Waiter (N−1 seats) | circular wait | pigeonhole: 4 diners, 5 forks, so one gets both |
| Monitor (Tanenbaum) | hold-and-wait | eat only if neither neighbour is eating; deadlock-free but **can starve** |

The `--deadlock` run shows all five philosophers each holding their left fork, then a watchdog reports the wait-for cycle P0 → P1 → P2 → P3 → P4 → P0.

Talking points:
- **Deadlock vs livelock vs starvation:**
  - Deadlock: nobody runs.
  - Livelock: everyone runs, nobody progresses (e.g. back-off in lock-step).
  - Starvation: some run, one never does.
- **TSan and the waiter solution:** ThreadSanitizer flags it as a potential deadlock. Its lock order really is cyclic; only the semaphore keeps the cycle from closing, and lock-order checkers can't see that. Ordered forks has no cycle at all, which is why lock ordering is the usual rule in real codebases.
- **Starvation-free variant:** Chandy–Misra (dirty/clean forks with request tokens). Good to name if asked.
