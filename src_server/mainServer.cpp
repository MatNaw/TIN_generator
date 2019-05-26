#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>


using boost::asio::ip::udp;
boost::mutex mut;
bool stopTransmission = false;

int sendingMessages(const udp::endpoint &receiver_endpoint, udp::socket *socket) {
    std::string messageSent;

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        std::cout << "Wyszedlem" << std::endl;
        messageSent = "next packet";

        socket->send_to(boost::asio::buffer(messageSent), receiver_endpoint);
        std::cout << "Wyslalem " << std::endl;

        {
            boost::unique_lock<boost::mutex> lock(mut);
            std::cout << stopTransmission << std::endl;
            if (stopTransmission) break;
        }
    }
    messageSent = "Disconnected";

    socket->send_to(boost::asio::buffer(messageSent), receiver_endpoint);
    return 0;
}

int receivingMessages(udp::endpoint &sender_endpoint, udp::socket *socket) {

    while (true) {
//        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        boost::array<char, 128> recv_buf;
        boost::system::error_code error;

        std::cout << "Odbieram" << std::endl;
        size_t len = socket->receive_from(boost::asio::buffer(recv_buf), sender_endpoint, 0, error);
        std::cout << std::string(reinterpret_cast<const char *>(recv_buf.data()), len) << std::endl;

        if (std::string(reinterpret_cast<const char *>(recv_buf.data()), len) == "quit") {
            boost::unique_lock<boost::mutex> lock(mut);
            stopTransmission = true;
            std::cout << stopTransmission << std::endl;
            break;
        }

        std::cout << "Powiadomienie" << std::endl;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    try {

        if (argc != 2) {
            std::cerr << "Required arguments: <portNumber>" << std::endl;
            return 1;
        }

        int serverPort = std::stoi(argv[1]);

        boost::asio::io_service io_service;

        udp::socket socket(io_service, udp::endpoint(udp::v4(), serverPort));

        for (;;) {
            boost::array<char, 100> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;
            size_t len = socket.receive_from(boost::asio::buffer(recv_buf),
                                             remote_endpoint, 0, error);

            if (std::string(reinterpret_cast<const char *>(recv_buf.data()), len) != "")
                std::cout << std::string(reinterpret_cast<const char *>(recv_buf.data()), len) << std::endl;

            if (error && error != boost::asio::error::message_size)
                throw boost::system::system_error(error);

            if (len > 0) {
                boost::thread sendThread(boost::bind(&sendingMessages, remote_endpoint, &socket));
                boost::thread recvThread(boost::bind(&receivingMessages, remote_endpoint, &socket));

                sendThread.join();
                recvThread.join();
            }
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
