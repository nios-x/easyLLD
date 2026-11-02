#include <iostream>
using namespace std;

// Overloading: same function name, different parameters, in the same class.
//              The compiler picks the version at compile time (compile-time polymorphism).
class Car {
public:
    void drive() {
        cout << "Car is driving\n";
    }
    void drive(int speed) {
        cout << "Car is driving at " << speed << " km/h\n";
    }
};

// Same name in an unrelated class: this is neither overloading nor overriding.
class Car2 {
public:
    void drive() {
        cout << "Car2 is driving\n";
    }
};

// Overriding: a child class redefines a virtual function of its parent.
//             The version is picked at run time from the real object (run-time polymorphism).
class Car3 {
public:
    virtual void drive() {
        cout << "Car3 is driving\n";
    }
    virtual ~Car3() = default;   // so delete through a Car3* also destroys the Car4 part
};

class Car4 : public Car3 {
public:
    void drive() override {
        cout << "Car4 is driving\n";
    }
};

int main() {
    Car car;
    car.drive();       // overloading -> drive()
    car.drive(80);     // overloading -> drive(int)

    Car2 car2;
    car2.drive();

    Car3* c = new Car4();
    c->drive();        // overriding -> "Car4 is driving"
                       // without virtual it prints "Car3 is driving" (method hiding)
    delete c;
    return 0;
}

// https://blog.algomaster.io/p/how-to-answer-a-lld-interview-problem
