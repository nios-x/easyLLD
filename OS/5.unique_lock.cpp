#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int total = 0;

mutex m1;

void addtillN(int n){
    std::unique_lock<mutex> lock(m1); // locks m1 here, like lock_guard
    // To start unlocked and lock manually later:
    //   std::unique_lock<mutex> lock(m1, std::defer_lock);
    //   lock.lock();
    for(int i=1; i<=n; i++){
        total++;
    }
    lock.unlock();  // manual unlock is allowed (lock_guard cannot do this)
    // work that does not touch shared data can go here, without holding the lock
}   // if still locked, the destructor unlocks it here

int main(){
    thread t1 = thread(addtillN, 5000);
    thread t2 = thread(addtillN, 5000);
    t1.join();
    t2.join();
    cout<<total<<"\n";   // always 10000
    return 0;
}
