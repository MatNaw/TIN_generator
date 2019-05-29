//
// Created by mivine on 26.05.19.
//

#include "Client.h"

#define MESSAGES_TO_SEND 10000

using boost::asio::ip::udp;
using boost::thread;

void Client::sendMessages() {

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));

        if (sentMessageCount != MESSAGES_TO_SEND) {
            socket.send_to(boost::asio::buffer(sendBuf), server_endpoint);
        }
        else {
            std::cout << "quit" ;
            socket.send_to(boost::asio::buffer("quit"), server_endpoint);
            break;
        }

        std::cout << "Wyslalem wiadomosc numer:" << sentMessageCount << std::endl;

        ++sentMessageCount;
    }
}

void Client::receiveMessages() {

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));
        boost::system::error_code error;

        std::cout << "Odbieram " << receivedMessageCount <<  std::endl;

        size_t len = socket.receive_from(boost::asio::buffer(recvBuf), server_endpoint, 0, error);
//        std::cout << std::string(reinterpret_cast<const char *>(recvBuf.data()), len) << std::endl;

        if (std::string(reinterpret_cast<const char *>(recvBuf.data())) == "Disconnected") break;

        std::cout << receivedMessageCount << std::endl;

        ++receivedMessageCount;
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
