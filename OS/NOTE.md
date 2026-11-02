# Threads and Synchronization in C++

## Thread

A thread is the smallest unit of execution that can be independently scheduled by the operating system within a process.

| Method | What it does |
| --- | --- |
| `join()` | Wait for the thread to finish |
| `detach()` | Run independently in the background (it can no longer be joined) |

A thread can be joined or detached only once; check `joinable()` first.

## Mutex

A mutex is a synchronization mechanism that ensures only one thread at a time can access a protected critical section of code.

## Locks

### lock_guard

A lock guard (`lock_guard`) in C++ is an STL class that automatically locks a mutex when an object is created and unlocks it when the object goes out of scope.

### unique_lock

`unique_lock` in C++ is similar to `lock_guard`, but it gives you more control over locking and unlocking a mutex manually.

Think of it this way:

- `lock_guard` → Automatically locks and unlocks. You cannot manually unlock it.
- `unique_lock` → Automatically manages locking, but you can manually lock and unlock whenever needed.

### scoped_lock

`scoped_lock` is a C++ RAII locking mechanism used to lock one or more mutexes automatically and unlock them when they go out of scope. When it locks several mutexes at once, it does so without deadlock.

## atomic

`atomic` (`std::atomic`) makes a simple operation on one variable, like `++`, happen as a single indivisible step, so no mutex is needed. Use a mutex instead when several variables must change together.

## shared_mutex

`shared_mutex` lets many readers hold the lock at the same time (`shared_lock`), but only one writer (`unique_lock`). It is used for the readers–writers problem.

## Mutex vs Semaphore

| | Mutex | Semaphore |
| --- | --- | --- |
| State | locked / unlocked | a counter (0 … max) |
| Ownership | yes: only the thread that locked it may unlock it | none: any thread can `release()` |
| Purpose | mutual exclusion around a critical section | counting resources, or signaling between threads |
| Unlock from another thread | undefined behavior | normal usage |
| RAII in std | `lock_guard`, `unique_lock`, `scoped_lock` | none built in |
| C++ type | `std::mutex` | `std::counting_semaphore`, `std::binary_semaphore` (C++20) |
| Priority inheritance | OS mutexes often support it | no |

---

## condition_variable

### What is condition_variable in C++?

A condition variable allows one thread to wait until another thread notifies it that something has happened.

### What are spurious wakeups in C++?

A spurious wakeup happens when a thread waiting on a condition variable wakes up even though nobody called `notify_one()` or `notify_all()`.

That is why `wait()` is always called with a predicate (or inside a `while` loop): the condition is checked again after every wake-up, and the thread goes back to sleep if it is still false.

### Example: producer–consumer with a bank balance

This is the classic producer–consumer example with `std::condition_variable`. One thread waits until a condition is true, and another thread makes it true and wakes it up.

**The pieces**

- `std::mutex m` protects the shared balance.
- `std::condition_variable cv` lets a thread sleep until another thread signals it, without burning CPU in a loop.
- `balance = 0` is the shared state both threads touch.

**addMoney (the producer)**

```cpp
void addMoney(int money) {
    std::lock_guard<mutex> lg(m);   // lock
    balance += money;
    cout << ...;
    cv.notify_one();                // wake one thread waiting on cv
}                                   // lg unlocks here
```

It locks, updates the balance, then calls `notify_one()` to wake up one thread blocked on `cv`. A `lock_guard` is fine here because this function never needs to unlock early.

**withdrawMoney (the consumer)**

```cpp
std::unique_lock<mutex> ul(m);
cv.wait(ul, [] { return balance != 0; });
```

This is the important line. `cv.wait(lock, predicate)` is equivalent to:

```cpp
while (!predicate()) {
    cv.wait(ul);   // atomically: unlock m + sleep; on wake: re-lock m
}
```

If the predicate is false, the call releases the mutex and puts the thread to sleep in a single atomic step. That lets `addMoney` acquire the lock. When the thread is notified, it re-locks the mutex and checks the predicate again. It only continues once the predicate is true and it holds the lock.

**Why `unique_lock` instead of `lock_guard`?** `wait()` has to unlock and re-lock the mutex internally, and `lock_guard` can’t do that. `unique_lock` can.

**What happens in main**

1. `t1` (`withdrawMoney(500)`) starts, locks `m`, and sees `balance == 0`. The predicate is false, so it unlocks and sleeps.
2. Main sleeps for 2 seconds. This only forces `t1` to go first for the demo.
3. `t2` (`addMoney(500)`) locks `m`, which it can do because `t1` released it. It sets `balance` to 500 and calls `notify_one()`.
4. `t1` wakes, waits for `t2`’s `lock_guard` to release, re-acquires the lock, and finds the predicate is now true. It deducts 500.

---

## Thread Pool

A Thread Pool is a collection of reusable worker threads that execute multiple tasks without creating a new thread for every task.

RAII is a C++ technique where resources are acquired when an object is created and automatically released when that object is destroyed.


Smart pointers are objects that automatically manage dynamically allocated memory.
2. Three types of smart pointers
C++ has three major smart pointers:
Smart pointer	Meaning
unique_ptr	Only one owner
shared_ptr	Multiple owners
weak_ptr	Non-owning reference to shared memory

    unique_ptr<int> p = make_unique<int>(10);

3. unique_ptr
One resource = One owner.
    unique_ptr<int> p = make_unique<int>(10);

4. shared_ptr
    One resource = Multiple owners.

    shared_ptr<int> p1 = make_shared<int>(100);
    shared_ptr<int> p2 = p1;
    cout << p1.use_count();

Object Pool in C++
An Object Pool is a creational design pattern where we create and store a fixed number of objects in advance and reuse them whenever needed instead of creating and destroying objects repeatedly.