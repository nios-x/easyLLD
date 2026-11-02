#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;

// Thread-safe counter using a mutex (see 9.atomic.cpp for the std::atomic version).
// mutable lets the const get() still lock the mutex.
class Counter {
    mutable mutex m;
    long long value = 0;
public:
    void increment() { lock_guard<mutex> lg(m); ++value; }
    void decrement() { lock_guard<mutex> lg(m); --value; }
    long long get() const { lock_guard<mutex> lg(m); return value; }
};

int main() {
    Counter c;
    vector<thread> threads;

    for (int t = 0; t < 4; t++)
        threads.emplace_back([&c] {
            for (int i = 0; i < 100000; i++) c.increment();
        });

    for (auto& th : threads) th.join();
    cout << c.get() << endl;   // always 400000
}
