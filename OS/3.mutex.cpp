#include<iostream>
#include<chrono>
#include<thread>
#include<mutex>
using namespace std;

int a = 0;   // shared between both threads

mutex m;
void run(){
    m.lock();      // only one thread at a time can get past this line
    for (int i=0; i<500; i++){
        a++;       // critical section: a++ is read + add + write, not one step
    }
    m.unlock();    // must always be called, or the other thread waits forever
}
int main(){
    std::thread t1 = std::thread(run);
    std::thread t2 = std::thread(run);

    t1.join();
    t2.join();
    cout<<a<<"\n";   // always 1000; without the mutex it can be less (race condition)
    return 0;
}
