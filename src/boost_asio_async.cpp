// see: https://sites.google.com/site/boostjp/tips/network/tcp
#include <iostream>
#include <boost/asio.hpp>

int main() {
    namespace asio = boost::asio;
    using asio::ip::tcp;
    
    asio::io_service io_service;
    tcp::socket socket(io_service);
    tcp::resolver resolver(io_service);
    asio::streambuf request;
    asio::streambuf response;
    
    // name resolution
    resolver.async_resolve({"www.boost.org", "http"}, [&](auto & error, auto it){
        if (error) {
            std::cout << "resolve failed: " << error.message() << std::endl;
            return;
        }
        
        // connect
        socket.async_connect(*it, [&](auto & error){
            if (error) {
                std::cout << "connect failed: " << error.message() << std::endl;
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
                        return;
                    }
                    std::istream response_stream(&response);
                    std::string l;
                    while (std::getline(response_stream,l)) std::cout << l << std::endl;
                });
            });
        });
    });
    
    io_service.run();
    
    return 0;
}
