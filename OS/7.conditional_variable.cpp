#include<iostream>
#include<thread>
#include<mutex>
#include <condition_variable>
using namespace std;

condition_variable cv;
/*

3. Why does wait() require unique_lock?
This is the important concept.

When a thread calls cv.wait(lock):
1. It releases the mutex automatically.
2. The thread goes to sleep.
3. Another thread modifies the shared variable.
4. Another thread calls cv.notify_one().
5. The sleeping thread wakes up.
6. It automatically locks the mutex again.
7. It checks the condition.

Steps 1 and 6 mean wait() must unlock and re-lock the mutex by itself.
lock_guard cannot unlock early, unique_lock can. That is why wait() needs unique_lock.

The condition (predicate) is checked again after every wake-up, so
spurious wakeups are handled, and it is fine if notify happens before wait.

*/
int amt = 0;
int n=100;
mutex m;
void incre(){
    lock_guard<mutex> lock(m);   // lock_guard is enough: no waiting here
    for(int i=0; i<n; i++) amt++;
    cout<<"INCre ->"<<endl;
    cv.notify_one();             // wake up the thread waiting in decre()
}
void decre(){
    unique_lock<mutex> lock(m);
    cv.wait(lock, [&]{return amt==n; });   // sleep until incre() has added n
    for(int i=0; i<n; i++) amt--;
    cout<<"DECre ->"<<endl;
}
int main(){
    std::thread t1 = thread(decre);   // started first, but waits for incre
    std::thread t2 = thread(incre);
    t1.join();
    t2.join();
    // Output is always "INCre ->" then "DECre ->", and amt ends at 0.

    return 0;
}
