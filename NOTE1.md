# OOPs
OOPs stands for Object-Oriented Programming System. It is a way of writing code by organizing it around objects rather than just functions.

    #include <iostream>
    using namespace std;

    class Car {
    public:
        string color;

        void drive() {
            cout << "Car is driving";
        }
    };

    int main() {
        Car c1;            // Object
        c1.color = "Red";
        c1.drive();
    }


## 1. Encapsulation
Wrapping data and methods together in one class and controlling access to the data.

## 2. Abstraction
Hiding implementation details and exposing only what the user needs.

## 3. Inheritance
One class can acquire accessible properties and behaviors from another class.

## 4. Polymorphism
One interface, different behaviors. The same function call can produce different results depending on the object.



# SOLID
### Single Responsibility Principle
A class should have only one purpose.

    class Invoice {
    public:
        void calculateTotal() {
            // Calculate invoice total
        }
    };

    class InvoicePrinter {
    public:
        void print() {
            // Print invoice
        }
    };

### Open/Closed Principle (OCP)
Software entities should be open for extension but closed for modification.

    class Shape {
    public:
        virtual double area() = 0;
    };

    class Circle : public Shape {
    public:
        double area() override {
            return 3.14 * 5 * 5;
        }
    };

### Liskov Substitution Principle (LSP)
A child class should be replaceable for its parent class without breaking program correctness.

    class Bird {
    public:
        virtual void eat() {}
    };

    class Sparrow : public Bird {
    public:
        void eat() override {}
    };


### Interface Segregation Principle (ISP)
Clients should not be forced to depend on interfaces they do not use.

    class Printer {
    public:
        virtual void print() = 0;
    };

    class Scanner {
    public:
        virtual void scan() = 0;
    };

### Dependency Inversion Principle (DIP)
High-level modules should not depend directly on low-level modules; both should depend on abstractions.

    class MessageSender {
    public:
        virtual void send() = 0;
    };

    class EmailSender : public MessageSender {
    public:
        void send() override {}
    };

    class Notification {
        MessageSender* sender;

    public:
        Notification(MessageSender* s) : sender(s) {}

        void notify() {
            sender->send();
        }
    };




DRY stands for Don't Repeat Yourself.

Definition: The DRY principle states that every piece of knowledge or logic in a software system should have a single, authoritative representation.


KISS stands for Keep It Simple, Stupid.

Definition: The KISS principle states that software design and implementation should be kept as simple as possible, avoiding unnecessary complexity.


YAGNI stands for “You Aren't Gonna Need It.”

Definition: The YAGNI principle states that developers should not implement a feature until it is actually needed.