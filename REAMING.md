# LLD Practice Plan

## Week 1

Warm up with LeetCode’s design-a-class problems:

- LRU Cache (146)
- Browser History (1472)
- Underground System (1396)
- Parking System (1603)

Then solve:

- Tic-Tac-Toe
- Snake & Ladder
- Parking Lot
- Vending Machine (State pattern)

## Week 2

Solve:

- Splitwise
- Library Management
- Elevator
- Pub-Sub queue
- A leveled Logger (Chain of Responsibility)
- Coffee Machine

From this week on, time every problem: 90 minutes, in-memory storage, and a driver main that actually runs.

## Week 3 (Nov 9 – 15): Method and easy problems

First read “How to answer an LLD interview problem.” It becomes your template for every problem after this.

Warm up with LeetCode’s design-a-class problems:

- LRU Cache (146)
- Browser History (1472)
- Underground System (1396)
- Parking System (1603)

Then solve:

- Tic-Tac-Toe
- Snake & Ladder
- Parking Lot
- Vending Machine (State pattern)

## Week 4 (Nov 16 – 22): Medium problems, machine-coding style

Solve:

- Splitwise
- Library Management
- Elevator
- Pub-Sub queue
- A leveled Logger (Chain of Responsibility)
- Coffee Machine

From this week on, time every problem: 90 minutes, in-memory storage, and a driver main that actually runs.

## Week 5 (Nov 23 – 29): Hard problems and concurrency

Solve:

- BookMyShow (seat locking)
- A token-bucket Rate Limiter
- A KV store with TTL
- Ride-sharing
- Food Delivery
- Chess

Do the concurrency parts in Go:

- Mutex vs RWMutex
- sync.Once for a singleton
- Channels

Make BookMyShow safe against double booking and make the rate limiter thread-safe.

## Week 6 (Nov 30 – Dec 6): Mock interviews and revision

Use AlgoMaster’s LLD practice. It has 45 problems in 9 sections with AI-graded timed mock interviews, and some of the questions are free.

- algomaster
- Do one live mock with a friend playing the interviewer and explain your design out loud.
- Redo two earlier problems from scratch and compare how long they take now.

## How to work each problem

- Requirements (5 minutes): write 5–7 bullets, plus what is out of scope.
- Entities and class diagram (15 minutes).
- Code (60–75 minutes): models, then interfaces, then services, then the driver.
- Compare with a reference solution only after you finish. Use the awesome-LLD repo or kousiknath/LowLevelDesign.
- Write down 3 things they did better in a `mistakes.md` file.
- Extension test: add one new requirement, such as a new vehicle type or split type. If you had to edit more than 2 classes, your design was too rigid, so refactor it.