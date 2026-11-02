#include<iostream>
#include<mutex>
#include<thread>
#include<condition_variable>

using namespace std;

condition_variable cv;
mutex m1;

const int LIMIT = 100000;
int cnt = 0;

// Two threads take turns: one prints the odd numbers, the other the even numbers.
// cnt is only read while holding m1 (reading it outside the lock is a data race).
// The predicate also checks LIMIT, so a waiting thread wakes up and stops at the end
// instead of printing LIMIT+1.

void incOdd(){   // runs when cnt is even, prints 1, 3, 5, ...
    while(true){
        unique_lock<mutex> lock(m1);
        cv.wait(lock, []{ return !(cnt&1) || cnt>=LIMIT; });
        if(cnt>=LIMIT) break;
        cnt++;
        cout<<cnt<<"\n";
        cv.notify_all();
    }
}

void incEven(){   // runs when cnt is odd, prints 2, 4, 6, ...
    while(true){
        unique_lock<mutex> lock(m1);
        cv.wait(lock, []{ return (cnt&1) || cnt>=LIMIT; });
        if(cnt>=LIMIT) break;
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
