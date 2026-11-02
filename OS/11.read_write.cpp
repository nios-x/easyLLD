#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <chrono>

using namespace std;

// Readers-writers problem:
// - many readers can read at the same time  -> shared_lock (shared ownership)
// - a writer needs the data all to itself   -> unique_lock (exclusive ownership)
// - while a writer holds the lock, no reader or other writer can enter

shared_mutex rw;
int database = 0;

void reader(int id) {
    shared_lock<shared_mutex> lock(rw);   // other readers can hold it too
    // Readers print at the same time, so their lines may get mixed together.
    cout << "Reader " << id
         << " reads: " << database << endl;

    this_thread::sleep_for(
        chrono::milliseconds(500)
    );
}

void writer(int id) {
    unique_lock<shared_mutex> lock(rw);   // waits until no reader or writer holds it
    database++;
    cout << "Writer " << id
         << " updates: " << database << endl;
    this_thread::sleep_for(
        chrono::milliseconds(500)
    );
}

int main() {
    vector<thread> threads;
    threads.emplace_back(reader, 1);
    threads.emplace_back(reader, 2);
    threads.emplace_back(writer, 1);
    threads.emplace_back(reader, 3);
    threads.emplace_back(writer, 2);

    for (auto &t : threads) {
        t.join();
    }
    // Final value of database is always 2. The order of the lines depends on
    // which thread gets the lock first, so it can change between runs.

    return 0;
}
