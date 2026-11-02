#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int total = 0;

mutex m1;

void addtillN(int n){
    std::lock_guard<mutex> lock(m1); //auto lock whole thread
    // m1.lock();
    for(int i=1; i<=n; i++){
        total++;
    }
    // m1.unlock();
}

int main(){
    thread t1 = thread(addtillN, 5000);
    thread t2 = thread(addtillN, 5000);
    t1.join();
    t2.join();
    cout<<total<<"\n";
    return 0;
}