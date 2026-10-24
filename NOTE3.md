# Design Patterns

## Creational

### Singleton

Singleton is a creational design pattern that lets you ensure that a class has only one instance, while providing a global access point to this instance.

### Factory

Factory Method is a creational design pattern that provides an interface for creating objects in a superclass, but allows subclasses to alter the type of objects that will be created.

### Builder

Builder is a creational design pattern that lets you construct complex objects step by step. The pattern allows you to produce different types and representations of an object using the same construction code.

## Behavioural

### Strategy

Definition: The Strategy Pattern is a behavioral design pattern that lets you define multiple algorithms, put each one in a separate class, and switch between them at runtime.

```ts
const navigator = new Navigator(new CarStrategy());

navigator.navigate();
// Building car route

navigator.setStrategy(new BikeStrategy());
navigator.navigate();
// Building bike route

navigator.setStrategy(new WalkingStrategy());
navigator.navigate();
// Building walking route
```

### Observer

The Observer Pattern is a behavioral design pattern where one object notifies multiple objects automatically whenever its state changes.

```ts
interface Observer {
  update(video: string): void;
}

class Subscriber implements Observer {
  constructor(private name: string) {}

  update(video: string): void {
    console.log(`${this.name} notified: ${video}`);
  }
}

class YouTubeChannel {
  private subscribers: Observer[] = [];

  subscribe(observer: Observer): void {
    this.subscribers.push(observer);
  }

  unsubscribe(observer: Observer): void {
    this.subscribers = this.subscribers.filter((sub) => sub !== observer);
  }

  uploadVideo(video: string): void {
    console.log(`New video: ${video}`);

    for (const subscriber of this.subscribers) {
      subscriber.update(video);
    }
  }
}

const channel = new YouTubeChannel();

const s1 = new Subscriber("Soumya");
const s2 = new Subscriber("Rahul");

channel.subscribe(s1);
channel.subscribe(s2);

channel.uploadVideo("Learn TypeScript");
```

### State

The State Pattern is a behavioral design pattern that allows an object to change its behavior when its internal state changes.

### State vs Strategy vs Observer

| Pattern | Main purpose | Example |
| --- | --- | --- |
| State | Behavior changes with internal state | ATM authentication |
| Strategy | Choose an interchangeable algorithm | UPI vs Stripe |
| Observer | Notify multiple objects of changes | YouTube subscribers |

### Command

The Command Pattern is a behavioral design pattern that turns a request or action into a separate object.

```ts
const tv = new TV();
const remote = new RemoteControl();

remote.setCommand(new PowerOnCommand(tv));
remote.pressButton();
// TV turned on

remote.setCommand(new VolumeUpCommand(tv));
remote.pressButton();
// TV volume increased
```

### Chain of Responsibility

The Chain of Responsibility is a behavioral design pattern that lets you pass a request along a chain of handler objects until one of them decides to process it.

```ts
class Handler {
  next?: Handler;

  setNext(handler: Handler) {
    this.next = handler;
    return handler;
  }

  approve(amount: number): void {
    if (this.next) {
      this.next.approve(amount);
    } else {
      console.log("Nobody can approve this!");
    }
  }
}

class Dad extends Handler {
  approve(amount: number): void {
    if (amount <= 100) {
      console.log("Dad approved!");
    } else {
      super.approve(amount);
    }
  }
}

class Mom extends Handler {
  approve(amount: number): void {
    if (amount <= 500) {
      console.log("Mom approved!");
    } else {
      super.approve(amount);
    }
  }
}

class Grandpa extends Handler {
  approve(amount: number): void {
    console.log("Grandpa approved!");
  }
}
```

```ts
class RequestHandlerApp {
  static main(): void {
    // Create handlers
    const auth: RequestHandler = new AuthHandler();
    const authorization: RequestHandler = new AuthorizationHandler();
    const rateLimit: RequestHandler = new RateLimitHandler();
    const validation: RequestHandler = new ValidationHandler();
    const businessLogic: RequestHandler = new BusinessLogicHandler();

    // Build the chain
    auth.setNext(authorization);
    authorization.setNext(rateLimit);
    rateLimit.setNext(validation);
    validation.setNext(businessLogic);

    // Send a request through the chain
    const request = new Request("john", "ADMIN", 10, '{ "data": "valid" }');
    auth.handle(request);

    console.log("\n--- Trying an invalid request ---");
    const badRequest = new Request(null, "USER", 150, "");
    auth.handle(badRequest);
  }
}
```

### Template Method

The Template Method Pattern defines the steps of an algorithm in a parent class, while child classes customize specific steps.