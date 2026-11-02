#include<iostream>
#include<chrono>
#include<thread>
#include<mutex>
using namespace std;

int a = 0;

mutex m;
void run(){
    m.lock();
    for (int i=0; i<500; i++){
        a++;
    }
    m.unlock();
}
int main(){
    std::thread t1 = std::thread(run);
    std::thread t2 = std::thread(run);
 
    t1.join();
    t2.join();
    cout<<"\n"<<a;
    return 0;
}