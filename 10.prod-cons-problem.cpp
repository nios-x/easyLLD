#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

using namespace std;

// Producer-consumer (bounded buffer):
// - producer waits while the buffer is full
// - consumer waits while the buffer is empty
// - the lock is released before notify and sleep, so the other thread can run

queue<int> buffer;
const int CAPACITY = 5;

mutex m;
condition_variable cv;

void producer() {
    for (int i = 1; i <= 10; i++) {

        unique_lock<mutex> lock(m);

        cv.wait(lock, [] {
            return buffer.size() < CAPACITY;
        });

        buffer.push(i);

        cout << "Produced: " << i << endl;

        lock.unlock();
        cv.notify_all();

        this_thread::sleep_for(
            chrono::milliseconds(100)
        );
    }
}

void consumer() {
    for (int i = 1; i <= 10; i++) {

        unique_lock<mutex> lock(m);

        cv.wait(lock, [] {
            return !buffer.empty();
        });

        int item = buffer.front();
        buffer.pop();

        cout << "Consumed: " << item << endl;

        lock.unlock();
        cv.notify_all();

        this_thread::sleep_for(
            chrono::milliseconds(150)
        );
    }
}

int main() {

    thread p(producer);
    thread c(consumer);

    p.join();
    c.join();

    return 0;
}
