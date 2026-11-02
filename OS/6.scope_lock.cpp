#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int total = 0;

mutex m1;

void addtillN(int n){
    std::scoped_lock lock(m1); // C++17: locks m1 here, unlocks when the function ends
    // It can also lock several mutexes at once without deadlock:
    //   std::scoped_lock lock(m1, m2);

    for(int i=1; i<=n; i++){
        total++;
    }

}

int main(){
    thread t1 = thread(addtillN, 5000);
    thread t2 = thread(addtillN, 5000);
    t1.join();
    t2.join();
    cout<<total<<"\n";   // always 10000
    return 0;
}
