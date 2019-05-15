//
// Created by dominik on 5/15/19.
//

#ifndef SRC_CLIENT_TIN_GENERATOR_UDPCLIENT_H
#define SRC_CLIENT_TIN_GENERATOR_UDPCLIENT_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

//using boost::asio::ip::udp;

//std::string make_daytime_string()
//{
//    using namespace std; // For time_t, time and ctime;
//    time_t now = time(0);
//    return ctime(&now);
//}

//
//
//class UDPClient {
//
//public:
//    UDPClient(boost::asio::io_service& io_service);
//
//private:
//
//    void start_receive();
//
//    void handle_receive(const boost::system::error_code& error,
//                        std::size_t /*bytes_transferred*/);
//
//    void handle_send(boost::shared_ptr<std::string> /*message*/,
//                     const boost::system::error_code& /*error*/,
//                     std::size_t /*bytes_transferred*/);
//
//    udp::socket socket_;
//    udp::endpoint remote_endpoint_;
//    boost::array<char, 1> recv_buffer_;
//
//
//};


#endif //SRC_CLIENT_TIN_GENERATOR_UDPCLIENT_H
