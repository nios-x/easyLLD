#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int total = 0;

mutex m1;

void addtillN(int n){
    std::lock_guard<mutex> lock(m1); // locks m1 here, unlocks automatically when the function ends
    // m1.lock();     // not needed: lock_guard does this
    for(int i=1; i<=n; i++){
        total++;
    }
    // m1.unlock();   // not needed: unlocked even if an exception is thrown (RAII)
}

int main(){
    thread t1 = thread(addtillN, 5000);
    thread t2 = thread(addtillN, 5000);
    t1.join();
    t2.join();
    cout<<total<<"\n";   // always 10000
    return 0;
}
