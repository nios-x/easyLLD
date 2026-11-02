#include<iostream>
#include<thread>
using namespace std;

// 1. A thread is the smallest unit of execution within a process.
class Main{
    public:
    static int sumtillN(int x){
        int total = 0;
        cout<<total;
        return total;
    }
};


int main(){
    thread t1 = thread(Main::sumtillN, 45);
    auto adder = [](int a, int b){
        int total = 0; 
        for(int i=a; i<=b; i++){
            total+=i;
        }
        cout<<total;
        return total;
    };

    thread t2 = thread(adder, 30, 40);
    t1.join();
    t2.join();


    return 0;
}
