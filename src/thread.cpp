#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

// see: C++ Pocket Reference, p446
namespace thread_sleep {
    std::mutex print_mutex;
    
    void print(std::string const & func, int value) {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << func << ": " << value << std::endl;
    }
    
    void foo() {
        std::chrono::seconds d(1);
        for (int i=0; i<3; ++i) {
            print("foo()", i);
            std::this_thread::sleep_for(d);
        }
    }
    
    void bar() {
        std::chrono::system_clock::time_point t =
            std::chrono::system_clock::now();
        for (int i=0; i<3; ++i) {
            print("bar()", i);
            std::this_thread::sleep_until(t + std::chrono::seconds(i+1));
        }
    }
}

int main() {
    { using namespace thread_sleep;
        std::thread t1(foo);
        std::thread t2(bar);
        t1.join();
        t2.join();
    }
    return 0;
}
