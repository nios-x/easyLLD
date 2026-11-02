#include<iostream>
#include<mutex>
#include<thread>
#include<condition_variable>
#include<queue>
#include<chrono>

using namespace std;

mutex m1;
condition_variable cv;
const int CAPACITY = 5;    // max items the buffer can hold
const int ITEMS = 100;     // total items produced (and consumed)
queue<int> q;

void prod(){
    for(int i=0; i<ITEMS; i++){
        unique_lock<mutex> lock(m1);
        cv.wait(lock, []{ return q.size()<CAPACITY; });   // wait while the buffer is full
        q.push(i);
        lock.unlock();      // unlock first, so we don't sleep while holding the lock
        cv.notify_all();    // wake the consumer
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}


void cons(){
    for(int i=0; i<ITEMS; i++){   // consume exactly ITEMS items, so the program can exit
        unique_lock<mutex> lock(m1);
        cv.wait(lock, []{ return !q.empty(); });   // wait while the buffer is empty
        int elem  = q.front();
        q.pop();
        cout<<elem<<"\n";
        lock.unlock();
        cv.notify_all();    // wake the producer if it was waiting for space
    }
}


int main(){
    thread t1 = thread(prod);
    thread t2 = thread(cons);
    t1.join();
    t2.join();
    return 0;
}
