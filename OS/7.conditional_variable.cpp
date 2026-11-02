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

*/
int amt = 0;
int n=100;
mutex m;
void incre(int x){
    lock_guard<mutex> lock(m);
    for(int i=0; i<n; i++) amt++;
    cout<<"INCre ->"<<endl;
    cv.notify_one();
}
void decre(int x){
    unique_lock<mutex> lock(m);
    cv.wait(lock, [&]{return amt==100; });
    for(int i=0; i<n; i++) amt--;
    cout<<"DECre ->"<<endl;
}
int main(){
    std::thread t1 = thread(decre, 10);
    std::thread t2 = thread(incre, 10);
    t1.join();
    t2.join();

    return 0;
}