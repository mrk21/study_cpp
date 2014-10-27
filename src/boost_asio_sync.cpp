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
    socket.connect(endpoint);
    
    // request
    asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream
        << "GET / HTTP/1.0\r\n"
        << "Host: www.boost.org\r\n"
        << "\r\n";
    
    asio::write(socket, request);
    
    // response
    asio::streambuf response;
    std::istream response_stream(&response);
    boost::system::error_code error;
    
    asio::read(socket, response, asio::transfer_all(), error);
    
    if (error && error != asio::error::eof) {
        std::cout << "receive failed: " << error.message() << std::endl;
    }
    else {
        std::string l;
        while (std::getline(response_stream,l)) std::cout << l << std::endl;
    }
    
    return 0;
}
