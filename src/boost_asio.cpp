#include <iostream>
#include <boost/asio.hpp>

int main() {
    using namespace boost::asio;
    using ip::tcp;
    
    tcp::iostream s("example.com","http"); s
        << "GET /index.html HTTP/1.0\r\n"
        << "Host: example.com\r\n"
        << "\r\n"
        << std::flush;
    
    std::string l;
    while (std::getline(s,l)) std::cout << l << std::endl;
    
    return 0;
}
