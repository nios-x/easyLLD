#include<iostream>
using namespace std;

class Car {
    public:
    void drive() {
        cout << "Car is driving";
    }
};

class Car2 {
    public:
    void drive() {
        cout << "Car2 is driving";
    }
};



class Car3 {
    public:
    void drive() {
        cout << "Car3 is driving";
    }
};

class Car4: public Car3{
    public:
    void drive() {
        cout << "Car4 is driving";
    }
};

int main(){
    Car3 *c  = new Car4();
    c->drive();
    return 0;
}



https://blog.algomaster.io/p/how-to-answer-a-lld-interview-problem