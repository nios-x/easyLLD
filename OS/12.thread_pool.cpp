#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
using namespace std;

class ThreadPool {
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex m;
    condition_variable cv;
    bool stop = false;
    public:
    ThreadPool(int n) {
        for(int i = 0; i < n; i++) {
            workers.emplace_back([this] {
                while(true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(m);
                        cv.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });
                        if(stop && tasks.empty())
                            return;
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void submit(function<void()> task) {
        {
            lock_guard<mutex> lock(m);
            tasks.push(move(task));
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        {
            lock_guard<mutex> lock(m);
            stop = true;
        }
        cv.notify_all();
        for(auto &worker : workers) {
            worker.join();
        }
    }
};


int main() {
    ThreadPool pool(3);
    for(int i = 0; i < 10; i++) {
        pool.submit([i] {
            cout << "Task " << i
                 << " executed by thread "
                 << this_thread::get_id() << endl;
        });
    }

    return 0;
}