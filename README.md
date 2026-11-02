# LLD: Placement Prep

Notes and code for low-level design (LLD) interviews: OOP, SOLID, UML, design patterns, LeetCode design problems, and multithreading in C++.

```
LLD/
├── NOTES/        Theory: OOP, SOLID, UML, design patterns, study plan
├── EXAMPLES/     Design patterns in TypeScript, LeetCode design problems in C++
└── OS/           Multithreading in C++, one concept per file
    └── concurrency/concurrency/   Full producer–consumer, readers–writers, dining philosophers
```

## NOTES

| File | Topic |
| --- | --- |
| [NOTE1.md](NOTES/NOTE1.md) | OOP pillars, SOLID, DRY / KISS / YAGNI |
| [NOTE2.md](NOTES/NOTE2.md) | UML class relationships: association, aggregation, composition, inheritance, multiplicity |
| [NOTE3.md](NOTES/NOTE3.md) | Creational and behavioral patterns: Singleton, Factory, Builder, Strategy, Observer, State, Command, Chain of Responsibility, Template Method |
| [NOTE4.md](NOTES/NOTE4.md) | Structural patterns: Decorator, Adapter, Facade, Proxy |
| [REMAINING.md](NOTES/REMAINING.md) | Study plan, weeks 3–5 |
| [extend.MD](NOTES/extend.MD) | Study plan, weeks 6–8 (Node.js) |

## EXAMPLES

| File | What it shows |
| --- | --- |
| [abc.ts](EXAMPLES/abc.ts) | Singleton, Factory, Builder, Strategy, Observer |
| [overloading vs overriding.cpp](EXAMPLES/overloading%20vs%20overriding.cpp) | Overloading (compile time) vs overriding (run time) |
| [LRUCACHE.md](EXAMPLES/LRUCACHE.md) | LeetCode 146: LRU Cache |
| [UNGRAILWAY.cpp](EXAMPLES/UNGRAILWAY.cpp) | LeetCode 1396: Design Underground System |

## OS: multithreading in C++

Start with [OS/NOTE.md](OS/NOTE.md). It explains every concept used in the files below.

| File | Concept |
| --- | --- |
| [1.threads.cpp](OS/1.threads.cpp) | Creating threads from a function, a static member and a lambda |
| [2.join.cpp](OS/2.join.cpp) | `join()` vs `detach()` |
| [3.mutex.cpp](OS/3.mutex.cpp) | `mutex` and race conditions |
| [4.lock_guard.cpp](OS/4.lock_guard.cpp) | `lock_guard` (RAII locking) |
| [5.unique_lock.cpp](OS/5.unique_lock.cpp) | `unique_lock` and manual unlock |
| [6.scope_lock.cpp](OS/6.scope_lock.cpp) | `scoped_lock` (C++17) |
| [7.conditional_variable.cpp](OS/7.conditional_variable.cpp) | `condition_variable`: wait and notify |
| [8.threadsafe_even_odd.cpp](OS/8.threadsafe_even_odd.cpp) | Two threads printing odd and even numbers in turn |
| [9.atomic.cpp](OS/9.atomic.cpp) | `std::atomic` counter |
| [10.prod-cons-problem.cpp](OS/10.prod-cons-problem.cpp) | Producer–consumer (bounded buffer) |
| [11.read_write.cpp](OS/11.read_write.cpp) | Readers–writers with `shared_mutex` |
| [12.thread_pool.cpp](OS/12.thread_pool.cpp) | Thread pool with a task queue |
| [13.smart_pointers.cpp](OS/13.smart_pointers.cpp) | Smart pointers (in progress) |
| [14.object_pool.cpp](OS/14.object_pool.cpp) | Object pool pattern |
| [99.examples.cpp](OS/99.examples.cpp) | Thread-safe counter with a mutex |

[OS/concurrency/concurrency/](OS/concurrency/concurrency/README.md) has longer versions of the three classic problems. Each program checks its own correctness while it runs.

## Build and run

### C++

You need g++ with C++20 support. On Windows, WSL (Ubuntu) is the easiest option.

```bash
g++ -std=c++20 -Wall -pthread OS/3.mutex.cpp -o mutex
./mutex
```

The classic problems have a Makefile:

```bash
cd OS/concurrency/concurrency
make run
```

`LRUCACHE.md` and `UNGRAILWAY.cpp` are written for the LeetCode editor, so they have no `#include` lines or `main`.

### TypeScript

```bash
npx -p typescript tsc EXAMPLES/abc.ts --outDir build
node build/abc.js
```

## References

- [How to answer an LLD interview problem](https://blog.algomaster.io/p/how-to-answer-a-lld-interview-problem) (AlgoMaster)
- [kousiknath/LowLevelDesign](https://github.com/kousiknath/LowLevelDesign)
