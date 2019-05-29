//
// Created by mivine on 26.05.19.
//

#ifndef SRC_CLIENT_TIN_GENERATOR_CLIENT_H
#define SRC_CLIENT_TIN_GENERATOR_CLIENT_H

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>

using boost::asio::ip::udp;
using boost::thread;


#define RECEIVE_BUFFER_SIZE 100
#define SEND_BUFFER_SIZE 100

class Client {

public:
    Client(boost::asio::io_service& io_service, const std::string& host, const std::string& port);

    void transmitToServer();

private:
    void receiveMessages();
    void sendMessages();

    int sentMessageCount = 0;
    int receivedMessageCount = 0;

    boost::array<char, RECEIVE_BUFFER_SIZE> recvBuf;
    boost::array<char, SEND_BUFFER_SIZE> sendBuf;

    udp::resolver resolver;
    udp::resolver::query query;
    udp::resolver::iterator iterator;
    udp::endpoint server_endpoint;
    udp::socket socket;
};


#endif //SRC_CLIENT_TIN_GENERATOR_CLIENT_H
