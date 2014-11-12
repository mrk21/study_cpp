// see: C++ pocket reference, p329
#include <iostream>
#include <chrono>

int main() {
    using namespace std::chrono;
    
    auto now = system_clock::now();
    auto epoch = duration_cast<seconds>(now.time_since_epoch());
    
    std::cout << epoch.count() << std::endl;
    
    return 0;
}
