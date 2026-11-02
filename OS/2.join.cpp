#include<iostream>
#include<chrono>
#include<thread>
using namespace std;

// join()   -> the calling thread waits until the thread finishes.
// detach() -> the thread runs independently in the background; nobody waits for it.
// A thread can be joined or detached only once. After that joinable() returns false,
// so always check joinable() before calling join()/detach() a second time.
// If a std::thread is destroyed while still joinable, the program calls std::terminate().

void run(){
    int a = 0;
    cout << "run: Hello 1" << endl;
    while(a<10){
        a++;
    }
    std::this_thread::sleep_for(chrono::seconds(2));
    cout<<"run: Hello 2"<<endl;
}

void run2(){
    int a = 0;
    cout << "run2: Hello 1" << endl;
    while(a<10){
        a++;
    }
    std::this_thread::sleep_for(chrono::seconds(1));
    cout<<"run2: Hello 2"<<endl;
}

int main(){
    std::thread t1 = std::thread(run);
    std::thread t2 = std::thread(run2);
    t2.detach();
    if(t2.joinable()){   // false: t2 is already detached, so this is skipped
        t2.detach();
    }

    t1.join();           // main blocks here for ~2 seconds
    if(t1.joinable()){   // false: t1 is already joined, so this is skipped
        t1.join();
    }
    // t2 sleeps only 1 second, so it finishes before t1 and its output is seen.
    // If a detached thread is still running when main returns, it is killed.
    return 0;
}
