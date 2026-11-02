# LLD Plan: Weeks 3–5

## Week 3 (Nov 9 – 15): Method and easy problems

First read [How to answer an LLD interview problem](https://blog.algomaster.io/p/how-to-answer-a-lld-interview-problem). It becomes your template for every problem after this.

Warm up with LeetCode’s design-a-class problems:

- [ ] LRU Cache (146)
- [ ] Browser History (1472)
- [ ] Underground System (1396)
- [ ] Parking System (1603)

Then solve:

- [ ] Tic-Tac-Toe
- [ ] Snake & Ladder
- [ ] Parking Lot
- [ ] Vending Machine (State pattern)

## Week 4 (Nov 16 – 22): Medium problems, machine-coding style

Solve:

- [ ] Splitwise
- [ ] Library Management
- [ ] Elevator
- [ ] Pub-Sub queue
- [ ] A leveled Logger (Chain of Responsibility)
- [ ] Coffee Machine

> From this week on, time every problem: 90 minutes, in-memory storage, and a driver `main` that actually runs.

## Week 5 (Nov 23 – 29): Hard problems and concurrency

Solve:

- [ ] BookMyShow (seat locking)
- [ ] A token-bucket Rate Limiter
- [ ] A KV store with TTL
- [ ] Ride-sharing
- [ ] Food Delivery
- [ ] Chess

Do the concurrency parts in Go:

- [ ] Mutex vs RWMutex
- [ ] `sync.Once` for a singleton
- [ ] Channels

Make BookMyShow safe against double booking and make the rate limiter thread-safe.

**Reference repo:** [kousiknath/LowLevelDesign](https://github.com/kousiknath/LowLevelDesign)
