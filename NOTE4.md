# Structural Design Patterns

## Decorator Pattern

The Decorator Pattern lets you add new features to an object without changing its original class.

```ts
class Pizza {
  cost(): number {
    return 100;
  }
}

class Cheese {
  constructor(private pizza: Pizza) {}

  cost(): number {
    return this.pizza.cost() + 20;
  }
}

const pizza = new Pizza();
const cheesePizza = new Cheese(pizza);

console.log(pizza.cost());       // 100
console.log(cheesePizza.cost()); // 120
```

## Adapter Pattern

Adapter Pattern: It allows incompatible interfaces to work together by converting one interface into another that the client expects.

```ts
// Existing system
class USB_A {
  connect() {
    console.log("USB-A connected");
  }
}

// Adapter
class USB_C_Adapter {
  constructor(private usbA: USB_A) {}

  connect() {
    this.usbA.connect();
  }
}

// Usage
const oldUSB = new USB_A();
const adapter = new USB_C_Adapter(oldUSB);

adapter.connect(); // USB-A connected
```

## Facade

The Facade Design Pattern is a structural design pattern that provides a single, simplified interface to a complex subsystem. Instead of forcing clients to coordinate many moving parts, a facade hides the internal complexity and exposes a clean, easy-to-use entry point.

It provides a simple interface for a complex system.

## Proxy Pattern

Provides a substitute or placeholder for another object to control access to it.

https://blog.algomaster.io/p/how-to-answer-a-lld-interview-problem
