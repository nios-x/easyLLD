#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
using namespace std;

// std::atomic makes ++ and -- one indivisible step, so no mutex is needed.
// Same counter as 99.examples.cpp, but lock-free and faster.
// Use atomic for a single variable; use a mutex when several variables
// must change together.
class Counter {
    atomic<long long> value{0};
public:
    void increment() { ++value; }                  // same as value.fetch_add(1)
    void decrement() { --value; }                  // same as value.fetch_sub(1)
    long long get() const { return value.load(); }
};

int main() {
    Counter c;
    vector<thread> threads;

    for (int t = 0; t < 4; t++)
        threads.emplace_back([&c] {
            for (int i = 0; i < 100000; i++) c.increment();
        });

    for (auto& th : threads) th.join();
    cout << c.get() << endl;   // always 400000 (with a plain long long it would often be less)
}
