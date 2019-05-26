//
// Created by mivine on 26.05.19.
//

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include "Client.h"

using boost::asio::ip::udp;
using boost::thread;

void Client::sendMessages() {

    std::string messageSent;

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        std::cout << "Wyszedlem" << std::endl;

        if (messageCount != 100)
            messageSent = "client";
        else
            messageSent = "quit";

        socket.send_to(boost::asio::buffer(messageSent), server_endpoint);
        std::cout << "Wyslalem wiadomosc numer:" << messageCount << std::endl;

        if (messageSent == "quit") break;

        ++messageCount;
    }
}


void Client::receiveMessages() {

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        boost::system::error_code error;

        std::cout << "Odbieram" << std::endl;

        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), server_endpoint, 0, error);
        std::cout << std::string(reinterpret_cast<const char *>(recv_buf.data()), len) << std::endl;

        if (std::string(reinterpret_cast<const char *>(recv_buf.data())) == "Disconnected") break;

        std::cout << "Powiadomienie" << std::endl;
    }
}

Client::Client(boost::asio::io_service& io_service, const std::string &host, const std::string &port)
        : resolver(udp::resolver(io_service))
        , query(udp::resolver::query(host, port))
        , iterator(resolver.resolve(query))
        , server_endpoint(iterator->endpoint())
        , socket(udp::socket(io_service))
        {
}

void Client::transmitToServer() {

    try {
        socket.open(udp::v4());

        boost::thread sendThread(boost::bind(&Client::receiveMessages, this));
        boost::thread recvThread(boost::bind(&Client::sendMessages, this));

        sendThread.join();
        recvThread.join();

        socket.close();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
