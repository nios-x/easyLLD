#include <iostream>
#include <vector>
using namespace std;

class Object {
public:
    void use() {
        cout << "Object is being used\n";
    }
};

class ObjectPool {
    vector<Object> pool;
    vector<bool> available;

public:
    ObjectPool(int size) {
        pool.resize(size);
        available.resize(size, true);
    }

    Object* acquire() {
        for (int i = 0; i < pool.size(); i++) {
            if (available[i]) {
                available[i] = false;
                return &pool[i];
            }
        }

        return nullptr; // Pool is full
    }

    void release(Object* obj) {
        for (int i = 0; i < pool.size(); i++) {
            if (&pool[i] == obj) {
                available[i] = true;
                return;
            }
        }
    }
};

int main() {
    ObjectPool p(2);

    Object* a = p.acquire();
    Object* b = p.acquire();

    if (a) a->use();
    if (b) b->use();

    p.release(a);

    Object* c = p.acquire(); // Reuses a

    if (c) c->use();
}