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
#define RESPONSE_SIZE 100

using boost::asio::ip::udp;

class Server {

public:
    Server(boost::asio::io_service& io_service, int exposedPort, unsigned long long responseInterval);

    void processUserInput();
    void start();

private:
    boost::mutex mut;

    bool stopTransmission = false;
    bool disconnected = true;

    std::string userCommand;

    udp::endpoint remoteEndpoint;
    udp::socket socket;

    boost::posix_time::ptime startOfResponse;
    boost::posix_time::ptime startOfReceive;
    boost::posix_time::ptime endOfResponse;
    boost::posix_time::ptime endOfReceive;

    boost::posix_time::ptime temporaryStartOfReceive;
    boost::posix_time::ptime temporaryStartOfResponse;
    boost::posix_time::ptime temporaryEndOfReceive;
    boost::posix_time::ptime temporaryEndOfResponse;

    unsigned long long intervalBetweenResponses;

    long long totalSentResponses = 0;
    long long totalReceivedMessages = 0;

    long long totalResponseSpeed = 0;
    long long totalReceiveSpeed = 0;

    long long currentResponseSpeed = 0;
    long long currentReceiveSpeed = 0;

    boost::system::error_code error;
    boost::array<char, RECEIVE_BUFFER_SIZE> receiveBuf;
    boost::array<char, RESPONSE_SIZE> responseBuf;

    void sendResponse();
    void receiveMessage();

    void updateResponseStats();
    void updateReceiveStats();

    void calculateTransmissionSpeed();
    void showTransmissionSpeed();

    void resetStats();
};


#endif //SRC_CLIENT_TIN_GENERATOR_SERVER_H
