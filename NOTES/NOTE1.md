# OOPs

OOPs stands for Object-Oriented Programming System. It is a way of writing code by organizing it around objects rather than just functions.

```cpp
#include <iostream>
#include <string>
using namespace std;

class Car {          // Class = blueprint
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
```

## 1. Encapsulation

Wrapping data and methods together in one class and controlling access to the data (using private, protected and public).

## 2. Abstraction

Hiding implementation details and exposing only what the user needs (using abstract classes and interfaces).

## 3. Inheritance

One class can acquire accessible properties and behaviors from another class. It models an “is-a” relationship.

## 4. Polymorphism

One interface, different behaviors. The same function call can produce different results depending on the object.

- **Compile-time polymorphism:** function overloading and operator overloading.
- **Run-time polymorphism:** overriding virtual functions.

# SOLID

| Letter | Principle | In one line |
| --- | --- | --- |
| S | Single Responsibility | A class should have only one reason to change |
| O | Open/Closed | Open for extension, closed for modification |
| L | Liskov Substitution | A child class can replace its parent without breaking anything |
| I | Interface Segregation | Many small interfaces instead of one big one |
| D | Dependency Inversion | Depend on abstractions, not on concrete classes |

## Single Responsibility Principle (SRP)

A class should have only one purpose, meaning only one reason to change.

```cpp
class Invoice {
public:
    void calculateTotal() {
        // Calculate invoice total
    }
};

// Printing is a separate job, so it lives in a separate class.
// Changing the print format does not touch Invoice.
class InvoicePrinter {
public:
    void print(const Invoice& invoice) {
        // Print invoice
    }
};
```

## Open/Closed Principle (OCP)

Software entities should be open for extension but closed for modification.

```cpp
class Shape {
public:
    virtual double area() = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double r;
public:
    Circle(double r) : r(r) {}
    double area() override {
        return 3.14 * r * r;
    }
};

// Adding a new shape = adding a new class. Existing code is not modified.
class Rectangle : public Shape {
    double w, h;
public:
    Rectangle(double w, double h) : w(w), h(h) {}
    double area() override {
        return w * h;
    }
};
```

## Liskov Substitution Principle (LSP)

A child class should be replaceable for its parent class without breaking program correctness.

```cpp
// Put only behavior that every bird has in Bird.
// A fly() here would break LSP: a Penguin is a Bird but cannot fly.
class Bird {
public:
    virtual void eat() {}
};

class Sparrow : public Bird {
public:
    void eat() override {}
};
```

## Interface Segregation Principle (ISP)

Clients should not be forced to depend on interfaces they do not use.

```cpp
// Instead of one big Machine interface with print() and scan(), split it.
// A basic printer implements only Printer; an all-in-one machine implements both.
class Printer {
public:
    virtual void print() = 0;
};

class Scanner {
public:
    virtual void scan() = 0;
};
```

## Dependency Inversion Principle (DIP)

High-level modules should not depend directly on low-level modules; both should depend on abstractions.

```cpp
class MessageSender {
public:
    virtual void send() = 0;
};

class EmailSender : public MessageSender {
public:
    void send() override {}
};

// Notification depends on the MessageSender abstraction, not on EmailSender,
// so an SmsSender can be passed in without changing Notification.
class Notification {
    MessageSender* sender;

public:
    Notification(MessageSender* s) : sender(s) {}

    void notify() {
        sender->send();
    }
};
```

# Other Design Principles

## DRY

DRY stands for Don't Repeat Yourself.

**Definition:** The DRY principle states that every piece of knowledge or logic in a software system should have a single, authoritative representation.

## KISS

KISS stands for Keep It Simple, Stupid.

**Definition:** The KISS principle states that software design and implementation should be kept as simple as possible, avoiding unnecessary complexity.

## YAGNI

YAGNI stands for “You Aren't Gonna Need It.”

**Definition:** The YAGNI principle states that developers should not implement a feature until it is actually needed.
