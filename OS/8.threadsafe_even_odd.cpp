#include<iostream>
#include<mutex>
#include<thread>
#include<condition_variable>

using namespace std;

condition_variable cv;
mutex m1;

using namespace std;

int cnt = 0;

void incOdd(){
    while(cnt<100000){
        unique_lock<mutex> lock(m1);
        cv.wait(lock, []{ return !(cnt&1); });
        cnt++;
        cout<<cnt<<"\n";
        cv.notify_all();
    }
}

void incEven(){
    while(cnt<100000){
        unique_lock<mutex> lock(m1);
        cv.wait(lock, []{ return (cnt&1); });
        cnt++;
        cout<<cnt<<"\n";
        cv.notify_all();
    }
}

int main(){
    
    thread t1 = thread(incOdd);
    thread t2 = thread(incEven);
    t1.join();
    t2.join();
    return 0;
}