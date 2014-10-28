// see: https://sites.google.com/site/boostjp/tips/network/tcp
#include <iostream>
#include <boost/asio.hpp>

int main() {
    namespace asio = boost::asio;
    using asio::ip::tcp;
    
    asio::io_service io_service;
    tcp::socket socket(io_service);
    
    // name resolution
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("www.boost.org", "http");
    tcp::endpoint endpoint(*resolver.resolve(query));
    
    // connect
    asio::streambuf request;
    asio::streambuf response;
    
    socket.async_connect(endpoint, [&](auto & error){
        if (error) {
            std::cout << error.message() << std::endl;
            return;
        }
        
        // request
        std::ostream request_stream(&request);
        request_stream
            << "GET / HTTP 1.0\r\n"
            << "Host: www.boost.org\r\n"
            << "\r\n";
        
        asio::async_write(socket, request, [&](auto &, auto){
            // response
            asio::async_read(socket, response, asio::transfer_all(), [&](auto & error, auto){
                if (error && error != asio::error::eof) {
                    std::cout << "receive failed: " << error.message() << std::endl;
                }
                else {
                    std::istream response_stream(&response);
                    std::string l;
                    while (std::getline(response_stream,l)) std::cout << l << std::endl;
                }
            });
        });
    });
    
    io_service.run();
    
    return 0;
}
