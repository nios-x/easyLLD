Got it. Weeks 1–5 stay exactly as they were, and these topics become new weeks after Week 5, all done in Node.js.

Week 6 (Nov 30 – Dec 6): Concurrency in Node

The idea: concurrency means two things happening at the same time. In Node, that’s usually two async requests overlapping, or two worker threads running in parallel.

Race condition. Two people book the same seat. Both requests check “is it free?”, both see yes, and both book it.
Fix it with a lock so only one request checks and books at a time. Use async-mutex.
Build: a BookMyShow seat booking that never double-books.
Thread pool. Instead of creating a new worker for every task, you keep a fixed number of workers (say 4), and tasks wait in line for a free one.
Read: worker_threads docs.
Build: a small pool of 4 workers that processes a queue of tasks.
Producer–consumer. One side adds items to a queue and the other side takes them out. If the queue is full, the producer waits. If it’s empty, the consumer waits.
Build: a queue with a max size, using Promises.
Deadlock. Task A holds lock 1 and waits for lock 2, while Task B holds lock 2 and waits for lock 1. Both wait forever.
Fix: always take locks in the same order, or give up after a timeout.
Build: create a deadlock on purpose with async-mutex, then fix it.

Week 7 (Dec 7 – 13): Error handling and database design

Error handling. Make your own error classes, like class SeatAlreadyBookedError extends Error.
Check inputs at the start of every service method, and throw clear errors instead of failing silently.
Add these to the Parking Lot and Splitwise code you already wrote.
Retry with backoff. If a call fails, wait 1s and try again, then 2s, then 4s.
Read: AWS: retries and backoff.
Circuit breaker. If a service keeps failing, stop calling it for a while so you don’t keep hammering it.
Read: Martin Fowler: Circuit Breaker.
Build: a small retry function and a circuit breaker class in TypeScript.
Database design. Write SQL tables for BookMyShow and Splitwise.
Example: UNIQUE(show_id, seat_id) means the database itself refuses a second booking of the same seat, even if your code has a bug.

Week 8 (Dec 14 – 20): Mock interviews and revision

This is the old Week 6, unchanged.

One tip: interviewers often say “thread” because they think in Java. In Node, answer with async race conditions and locks, or with worker threads. The ideas are the same.