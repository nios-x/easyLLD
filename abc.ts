class SingleTon{
    private static instance:SingleTon|null = null;
    private constructor(){}
    public static getInstance(){
        if(SingleTon.instance===null){
            SingleTon.instance = new SingleTon();
        }
        return SingleTon.instance;
    }
    public  getName(){
        console.log("Instance");
    }
}

// let s = SingleTon.getInstance();
// s.getName()

class BMW {
    drive() {
        console.log("Driving BMW");
    }
}

class Tesla {
    drive() {
        console.log("Driving Tesla");
    }
}

class CarFactory {
    static createCar(type:string):any {
        if (type === "BMW") {
            return new BMW();
        }

        if (type === "Tesla") {
            return new Tesla();
        }

        return null;
    }
}

// // Use the factory
// const car1 = CarFactory.createCar("BMW");
// car1.drive();

// const car2 = CarFactory.createCar("Tesla");
// car2.drive();


// class PC {
//   constructor(
//     public ram: number = 8,
//     public storage: number = 256,
//     public graphics: string = "Integrated",
//     public wifi: boolean = false
//   ) {}
// }

// class PCBuilder {
//   private pc = new PC();

//   setRAM(ram: number): this {
//     this.pc.ram = ram;
//     return this;
//   }

//   setStorage(storage: number): this {
//     this.pc.storage = storage;
//     return this;
//   }

//   setGraphics(graphics: string): this {
//     this.pc.graphics = graphics;
//     return this;
//   }

//   setWifi(wifi: boolean): this {
//     this.pc.wifi = wifi;
//     return this;
//   }

//   build(): PC {
//     return this.pc;
//   }
// }

// const pc = new PCBuilder()
//   .setRAM(16)
//   .setStorage(512)
//   .setGraphics("RTX 4060")
//   .setWifi(true)
//   .build();

// console.log(pc);







interface PaymentStrategy {
  pay(amount: number): void;
}

class UpiPayment implements PaymentStrategy {
  pay(amount: number): void {
    console.log(`Paid ₹${amount} using UPI`);
  }
}

class CardPayment implements PaymentStrategy {
  pay(amount: number): void {
    console.log(`Paid ₹${amount} using Card`);
  }
}

class Payment {
  constructor(private strategy: PaymentStrategy) {}

  pay(amount: number): void {
    this.strategy.pay(amount);
  }
}

// Choose UPI
const payment1 = new Payment(new UpiPayment());
payment1.pay(500);

// Choose Card
const payment2 = new Payment(new CardPayment());
payment2.pay(1000);














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
    this.subscribers = this.subscribers.filter(
      sub => sub !== observer
    );
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
