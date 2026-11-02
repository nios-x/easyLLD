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

// Cheese has the same cost() method as Pizza, so decorators can wrap each other
const doubleCheesePizza = new Cheese(new Cheese(pizza));
console.log(doubleCheesePizza.cost()); // 140
```

## Adapter Pattern

The Adapter Pattern allows incompatible interfaces to work together by converting one interface into another that the client expects.

```ts
// Existing system
class USB_A {
  connect() {
    console.log("USB-A connected");
  }
}

// What the client expects
interface USB_C {
  connectUSBC(): void;
}

// Adapter: looks like USB_C to the client, uses USB_A inside
class USB_C_Adapter implements USB_C {
  constructor(private usbA: USB_A) {}

  connectUSBC() {
    this.usbA.connect();
  }
}

// Usage
const oldUSB = new USB_A();
const adapter: USB_C = new USB_C_Adapter(oldUSB);

adapter.connectUSBC(); // USB-A connected
```

## Facade Pattern

The Facade Design Pattern is a structural design pattern that provides a single, simplified interface to a complex subsystem. Instead of forcing clients to coordinate many moving parts, a facade hides the internal complexity and exposes a clean, easy-to-use entry point.

**In short:** it provides a simple interface for a complex system.

**Example:** a HomeTheater class with one watchMovie() method that turns on the TV, the sound system and the lights in the right order.

## Proxy Pattern

Provides a substitute or placeholder for another object to control access to it.

**Example:** a proxy that loads a large image only when it is first displayed (lazy loading), checks permissions, or caches results before calling the real object.

## Reference

[How to answer an LLD interview problem](https://blog.algomaster.io/p/how-to-answer-a-lld-interview-problem)
