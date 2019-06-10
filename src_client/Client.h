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
#define MESSAGE_SIZE 100

class Client {

public:
    Client(boost::asio::io_service& io_service, const std::string& host, const std::string& port, unsigned long long numberOfPackages, unsigned long long packagesInterval);

    void transmitToServer();

private:
    void receiveMessages();
    void sendMessages();

    void processUserInput();

    void updateSendingStats();
    void updateReceiveStats();

    void calculateTransmissionSpeed();
    void showTransmissionSpeed();

    std::string userCommand;

    unsigned long long intervalBetweenPackages;
    unsigned long long numberOfPackagesToSend;

    bool transmissionEnd = false;

    long long totalSentMessages = 0;
    long long totalReceivedMessages = 0;

    long long totalSendingSpeed = 0;
    long long totalReceiveSpeed = 0;

    long long currentSendingSpeed = 0;
    long long currentReceiveSpeed = 0;

    boost::posix_time::ptime startOfSending;
    boost::posix_time::ptime startOfReceive;
    boost::posix_time::ptime endOfSending;
    boost::posix_time::ptime endOfReceive;

    boost::posix_time::ptime temporaryStartOfReceive;
    boost::posix_time::ptime temporaryStartOfResponse;
    boost::posix_time::ptime temporaryEndOfReceive;
    boost::posix_time::ptime temporaryEndOfResponse;

    boost::array<char, RECEIVE_BUFFER_SIZE> recvBuf;
    boost::array<char, MESSAGE_SIZE> sendBuf;

    udp::resolver resolver;
    udp::resolver::query query;
    udp::resolver::iterator iterator;
    udp::endpoint server_endpoint;
    udp::socket socket;
};

#endif //SRC_CLIENT_TIN_GENERATOR_CLIENT_H
