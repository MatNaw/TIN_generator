//
// Created by mivine on 26.05.19.
//

#include "Server.h"

#define DISCONNECT_MESSAGE "Disconnected"
#define TRANSMISSION_END_MESSAGE "quit"

#define PRECISION 100

using boost::asio::ip::udp;

Server::Server(boost::asio::io_service &io_service, int exposedPort)
        : socket(udp::socket(io_service, udp::endpoint(udp::v4(), exposedPort))) {
}

void Server::processUserInput() {
    while (userCommand != "exit") {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        std::cin >> userCommand;

        if(userCommand == "speed") {
            calculateTransmissionSpeed();
            showTransmissionSpeed();
        }
    }
}

void Server::sendResponse() {
    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));

        size_t bytesSent = socket.send_to(boost::asio::buffer(responseBuf), remoteEndpoint);
//        std::cout << "Wyslalem " << std::endl;

        {
            boost::unique_lock<boost::mutex> lock(mut);

            if (stopTransmission) {
                break;
            }
        }

        if(bytesSent > 0) {
            updateResponseStats();
        }
    }

    socket.send_to(boost::asio::buffer(DISCONNECT_MESSAGE), remoteEndpoint);
}

void Server::receiveMessage() {

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));

        size_t len = socket.receive_from(boost::asio::buffer(receiveBuf), remoteEndpoint, 0, error);
//        std::cout << std::string(reinterpret_cast<const char *>(receiveBuf.data()), len) << std::endl;

        if (std::string(reinterpret_cast<const char *>(receiveBuf.data()), len).find(TRANSMISSION_END_MESSAGE) != std::string::npos ) {
            boost::unique_lock<boost::mutex> lock(mut);
            stopTransmission = true;
            break;
        }

        updateReceiveStats();
    }
}

void Server::start() {
    try {

        for (;;) {
            size_t len = socket.receive_from(boost::asio::buffer(receiveBuf),
                                             remoteEndpoint, 0, error);

            if (std::string(reinterpret_cast<const char *>(receiveBuf.data()), len) != "")
//                std::cout << std::string(reinterpret_cast<const char *>(receiveBuf.data()), len) << std::endl;

            if (error && error != boost::asio::error::message_size)
                throw boost::system::system_error(error);

            if (len > 0) {
                boost::thread userInputThread(boost::bind(&Server::processUserInput, this));
                boost::thread sendThread(boost::bind(&Server::sendResponse, this));
                boost::thread recvThread(boost::bind(&Server::receiveMessage, this));

                userInputThread.join();
                break;
            }
        }
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

}

void Server::showTransmissionSpeed() {
    std::cout << std::endl;
    std::cout << "Aktualna prędkość odbierania pakietów: " << currentReceiveSpeed << " KB na sekundę" << std::endl;
    std::cout << "Aktualna prędkość wysyłania pakietów: " << currentResponseSpeed << " KB na sekundę" << std::endl << std::endl;

    std::cout << "Sumaryczna prędkość odbierania pakietów: " << totalReceiveSpeed  << " KB na sekundę" <<  std::endl;
    std::cout << "Sumaryczna prędkość wysyłania pakietów: " << totalResponseSpeed << " KB na sekundę" <<  std::endl << std::endl;

    std::cout << "Odebranych pakietów: " << totalReceivedMessages  << std::endl;
    std::cout << "Wysłanych pakietów: " << totalSentResponses <<  std::endl;
}

void Server::updateResponseStats() {
    ++totalSentResponses;

    if(totalSentResponses == 1) {
        startOfResponse = boost::posix_time::microsec_clock::local_time();
        temporaryEndOfResponse = startOfResponse;
    }

    endOfResponse = boost::posix_time::microsec_clock::local_time();

    if(totalSentResponses >= 99 && totalSentResponses % PRECISION == 99) {
        temporaryStartOfResponse = temporaryEndOfResponse;
        temporaryEndOfResponse = endOfResponse;
    }
}

void Server::updateReceiveStats() {
    ++totalReceivedMessages;

    if(totalReceivedMessages == 1) {
        startOfReceive = boost::posix_time::microsec_clock::local_time();
        temporaryEndOfReceive = startOfReceive;
    }

    endOfReceive = boost::posix_time::microsec_clock::local_time();

    if(totalReceivedMessages >= 99 && totalReceivedMessages % PRECISION == 99) {
        temporaryStartOfReceive = temporaryEndOfReceive;
        temporaryEndOfReceive = endOfReceive;
    }
}

void Server::calculateTransmissionSpeed() {
    boost::posix_time::time_duration totalResponseDurationPosix = endOfResponse - startOfResponse;
    boost::posix_time::time_duration totalReceiveDurationPosix = endOfReceive - startOfReceive;
    boost::posix_time::time_duration currentResponseDurationPosix = temporaryEndOfResponse - temporaryStartOfResponse;
    boost::posix_time::time_duration currentReceiveDurationPosix = temporaryEndOfReceive - temporaryStartOfReceive;

    long long totalResponseDuration = totalResponseDurationPosix.total_milliseconds();
    long long totalReceiveDuration = totalReceiveDurationPosix.total_milliseconds();
    long long currentResponseDuration = currentResponseDurationPosix.total_milliseconds();
    long long currentReceiveDuration = currentReceiveDurationPosix.total_milliseconds();

    totalReceiveSpeed = (totalReceivedMessages * RECEIVE_BUFFER_SIZE * 977) / totalReceiveDuration;
    totalResponseSpeed = (totalSentResponses * RESPONSE_SIZE * 977) / totalResponseDuration;

    currentReceiveSpeed = (PRECISION * RECEIVE_BUFFER_SIZE * 977) / currentReceiveDuration;
    currentResponseSpeed = (PRECISION * RESPONSE_SIZE * 977) / currentResponseDuration;

    if(totalReceivedMessages < PRECISION) {
        currentReceiveSpeed = totalReceiveSpeed;
    }

    if(totalSentResponses < PRECISION) {
        currentResponseSpeed = totalResponseSpeed;
    }
}