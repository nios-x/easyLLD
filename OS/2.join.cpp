#include<iostream>
#include<chrono>
#include<thread>
using namespace std;

void run(){
    int a = 0;
    cout << "Hello 1" << endl;
    while(a<10){
        a++;
    }
    std::this_thread::sleep_for(chrono::seconds(2));
    cout<<"Hello 2"<<endl;
}

void run2(){
    int a = 0;
    cout << "Hello 1" << endl;
    while(a<10){
        a++;
    }
    std::this_thread::sleep_for(chrono::seconds(1));
    cout<<"Hello 2"<<endl;
}

int main(){
    std::thread t1 = std::thread(run);
    std::thread t2 = std::thread(run2);
    t2.detach();
    if(t2.joinable()){
        t2.detach();
    }
    
    t1.join();
    if(t1.joinable()){
        t1.join();
    }
    return 0;
}