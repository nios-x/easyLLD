A thread is the smallest unit of execution that can be independently scheduled by the operating system within a process.

join() — Wait for the thread
detach() — Run independently

A mutex is a synchronization mechanism that ensures only one thread at a time can access a protected critical section of code.

A lock guard (lock_guard) in C++ is an STL class that automatically locks a mutex when an object is created and unlocks it when the object goes out of scope.

unique_lock in C++ is similar to lock_guard, but it gives you more control over locking and unlocking a mutex manually.
 
Think of it this way:
- lock_guard → Automatically locks and unlocks. You cannot manually unlock it.
- unique_lock → Automatically manages locking, but you can manually lock and unlock whenever needed.

scoped_lock is a C++ RAII locking mechanism used to lock one or more mutexes automatically and unlock them when they go out of scope.






<!------------------------------------------------------------------------------------------------------------->
<!------------------------------------------------------------------------------------------------------------->
<!------------------------------------------------------------------------------------------------------------->
This is the classic producer–consumer example with std::condition_variable. One thread waits until a condition is true, and another thread makes it true and wakes it up.

The pieces
std::mutex m protects the shared balance.
std::condition_variable cv lets a thread sleep until another thread signals it, without burning CPU in a loop.
balance = 0 is the shared state both threads touch.
addMoney (the producer)
cpp
std::lock_guard<mutex> lg(m);   // lock
balance += money;
cout << ...;
cv.notify_one();                // wake one thread waiting on cv
}                               // lg unlocks here

It locks, updates the balance, then calls notify_one() to wake up one thread blocked on cv. A lock_guard is fine here because this function never needs to unlock early.

withdrawMoney (the consumer)

    std::unique_lock<mutex> ul(m);
    cv.wait(ul, [] { return balance != 0; });

This is the important line. cv.wait(lock, predicate) is equivalent to:


    while (!predicate()) {
        cv.wait(ul);   // atomically: unlock m + sleep; on wake: re-lock m
    }

If the predicate is false, the call releases the mutex and puts the thread to sleep in a single atomic step. That lets addMoney acquire the lock. When the thread is notified, it re-locks the mutex and checks the predicate again. It only continues once the predicate is true and it holds the lock.

Why unique_lock instead of lock_guard? wait() has to unlock and re-lock the mutex internally, and lock_guard can’t do that. unique_lock can.

What happens in main
    t1 (withdrawMoney(500)) starts, locks m, and sees balance == 0. The predicate is false, so it unlocks and sleeps.
    Main sleeps for 2 seconds. This only forces t1 to go first for the demo.
    t2 (addMoney(500)) locks m, which it can do because t1 released it. It sets balance to 500 and calls notify_one().
    t1 wakes, waits for t2’s lock_guard to release, re-acquires the lock, and finds the predicate is now true. It deducts 500.
<!------------------------------------------------------------------------------------------------------------->
<!------------------------------------------------------------------------------------------------------------->
<!------------------------------------------------------------------------------------------------------------->



What is condition_variable in C++?
A condition variable allows one thread to wait until another thread notifies it that something has happened.

What are Spurious Wakeups in C++?
A spurious wakeup happens when a thread waiting on a condition variable wakes up even though nobody called notify_one() or notify_all().