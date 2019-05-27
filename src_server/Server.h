//
// Created by mivine on 26.05.19.
//

#ifndef SRC_CLIENT_TIN_GENERATOR_SERVER_H
#define SRC_CLIENT_TIN_GENERATOR_SERVER_H

#define BOOST_SYSTEM_NO_DEPRECATED

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>

#define RECEIVE_BUFFER_SIZE 100

using boost::asio::ip::udp;

class Server {

public:
    Server(boost::asio::io_service& io_service, int exposedPort);

    void start();

private:
    boost::mutex mut;
    bool stopTransmission = false;



    udp::endpoint remoteEndpoint;
    udp::socket socket;

    boost::system::error_code error;
    boost::array<char, RECEIVE_BUFFER_SIZE> recv_buf;

    void sendResponse();
    void receiveMessage();
};


#endif //SRC_CLIENT_TIN_GENERATOR_SERVER_H
