#include<iostream>
#include<thread>
using namespace std;

// 1. A thread is the smallest unit of execution within a process.
// 2. A thread can run a static member function, a normal function or a lambda.
class Main{
    public:
    static int sumtillN(int x){
        int total = 0;
        for(int i=1; i<=x; i++){
            total+=i;
        }
        cout<<"sumtillN: "<<total<<"\n";   // 1+2+...+45 = 1035
        return total;
    }
};


int main(){
    thread t1 = thread(Main::sumtillN, 45);   // arguments are passed after the function
    auto adder = [](int a, int b){
        int total = 0;
        for(int i=a; i<=b; i++){
            total+=i;
        }
        cout<<"adder: "<<total<<"\n";       // 30+31+...+40 = 385
        return total;
    };

    thread t2 = thread(adder, 30, 40);
    // Both threads now run at the same time, so the two lines can print in any order.
    // The return values are ignored; a thread cannot return a value directly.
    t1.join();   // main waits here until t1 finishes
    t2.join();


    return 0;
}
