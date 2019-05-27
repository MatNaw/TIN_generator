//
// Created by mivine on 26.05.19.
//

#include "Server.h"

using boost::asio::ip::udp;

void Server::sendResponse() {
    std::string messageSent;

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        std::cout << "Wyszedlem" << std::endl;
        messageSent = "next packet";

        socket.send_to(boost::asio::buffer(messageSent), remoteEndpoint);
        std::cout << "Wyslalem " << std::endl;

        {
            boost::unique_lock<boost::mutex> lock(mut);
            std::cout << stopTransmission << std::endl;
            if (stopTransmission) break;
        }
    }
    messageSent = "Disconnected";

    socket.send_to(boost::asio::buffer(messageSent), remoteEndpoint);
}

void Server::receiveMessage() {

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));

        std::cout << "Odbieram" << std::endl;
        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), remoteEndpoint, 0, error);
        std::cout << std::string(reinterpret_cast<const char *>(recv_buf.data()), len) << std::endl;

        if (std::string(reinterpret_cast<const char *>(recv_buf.data()), len) == "quit") {
            boost::unique_lock<boost::mutex> lock(mut);
            stopTransmission = true;
            std::cout << stopTransmission << std::endl;
            break;
        }

        std::cout << "Powiadomienie" << std::endl;
    }
}

Server::Server(boost::asio::io_service& io_service, int exposedPort)
    : socket(udp::socket(io_service, udp::endpoint(udp::v4(), exposedPort))) {
}

void Server::start() {
    try {

        for (;;) {
            size_t len = socket.receive_from(boost::asio::buffer(recv_buf),
                                             remoteEndpoint, 0, error);

            if (std::string(reinterpret_cast<const char *>(recv_buf.data()), len) != "")
                std::cout << std::string(reinterpret_cast<const char *>(recv_buf.data()), len) << std::endl;

            if (error && error != boost::asio::error::message_size)
                throw boost::system::system_error(error);

            if (len > 0) {
                boost::thread sendThread(boost::bind(&Server::sendResponse, this));
                boost::thread recvThread(boost::bind(&Server::receiveMessage, this));

                sendThread.join();
                recvThread.join();
            }
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

}
