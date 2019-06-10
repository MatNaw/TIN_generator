//
// Created by mivine on 26.05.19.
//

#include "Server.h"

#define DISCONNECT_MESSAGE "Disconnected"
#define TRANSMISSION_END_MESSAGE "End"
#define TRANSMISSION_BEGIN_MESSAGE "Begin"

#define SHUTDOWN_SERVER_COMMAND "exit"

#define PRECISION 100

using boost::asio::ip::udp;

Server::Server(boost::asio::io_service &io_service, int exposedPort)
        : socket(udp::socket(io_service, udp::endpoint(udp::v4(), exposedPort))) {
}

void Server::processUserInput() {
    while (userCommand != SHUTDOWN_SERVER_COMMAND) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        std::cin >> userCommand;

        if (userCommand == "speed") {
            if (!disconnected) {
                calculateTransmissionSpeed();
            }
            showTransmissionSpeed();
        }
    }
}

void Server::sendResponse() {
    size_t bytesSent;

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));

        if (!stopTransmission) {
            {
                boost::unique_lock<boost::mutex> lock(mut);
                bytesSent = socket.send_to(boost::asio::buffer(responseBuf), remoteEndpoint);
            }

            if (bytesSent > 0) {
                updateResponseStats();
            }
        } else if (!disconnected) {
            socket.send_to(boost::asio::buffer(DISCONNECT_MESSAGE), remoteEndpoint);
            disconnected = true;
            resetStats();
        }
    }
}

void Server::receiveMessage() {

    std::cout << "Receiving messages started" << std::endl;

    while (true) {
//        boost::this_thread::sleep(boost::posix_time::milliseconds(5));

        size_t len = socket.receive_from(boost::asio::buffer(receiveBuf), remoteEndpoint, 0, error);

        std::string receivedMessage = std::string(reinterpret_cast<const char *>(receiveBuf.data()), len);

        if (receivedMessage.find(TRANSMISSION_BEGIN_MESSAGE) != std::string::npos) {
            std::cout << "Receiving messages started" << std::endl;
            stopTransmission = false;
            disconnected = false;

        } else if (receivedMessage.find(TRANSMISSION_END_MESSAGE) != std::string::npos) {
            boost::unique_lock<boost::mutex> lock(mut);
            stopTransmission = true;
            std::cout << "Receiving messages stopped" << std::endl << std::endl;
            resetStats();
        }

        if (len > 0 && !stopTransmission) {
            updateReceiveStats();
        }
    }

}

void Server::start() {
    try {

        std::cout << "Server has started" << std::endl;
        boost::thread userInputThread(boost::bind(&Server::processUserInput, this));

        while (userCommand != SHUTDOWN_SERVER_COMMAND) {

            size_t len = socket.receive_from(boost::asio::buffer(receiveBuf),
                                             remoteEndpoint, 0, error);

            std::string receivedMessage = std::string(reinterpret_cast<const char *>(receiveBuf.data()), len);

            if (error && error != boost::asio::error::message_size)
                throw boost::system::system_error(error);

            if (receivedMessage.find(TRANSMISSION_BEGIN_MESSAGE) != std::string::npos) {
                disconnected = false;

                boost::thread sendThread(boost::bind(&Server::sendResponse, this));
                boost::thread recvThread(boost::bind(&Server::receiveMessage, this));

                userInputThread.join();
            }
        }

        std::cout << "Server is shutting down" << std::endl;
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

}

void Server::showTransmissionSpeed() {
    std::cout << std::endl;
    std::cout << "Current receive speed: " << currentReceiveSpeed << " Bytes per second" << std::endl;
    std::cout << "Current response speed: " << currentResponseSpeed << " Bytes per second \n" << std::endl;

    std::cout << "Total receive speed: " << totalReceiveSpeed << " Bytes per second" << std::endl;
    std::cout << "Total response speed: " << totalResponseSpeed << " Bytes per second \n" << std::endl;

    std::cout << "Received packages: " << totalReceivedMessages << std::endl;
    std::cout << "Sent packages: " << totalSentResponses << std::endl;
}

void Server::updateResponseStats() {
    ++totalSentResponses;

    if (totalSentResponses == 1) {
        startOfResponse = boost::posix_time::microsec_clock::local_time();
        temporaryEndOfResponse = startOfResponse;
    }

    endOfResponse = boost::posix_time::microsec_clock::local_time();

    if (totalSentResponses >= 99 && totalSentResponses % PRECISION == 99) {
        temporaryStartOfResponse = temporaryEndOfResponse;
        temporaryEndOfResponse = endOfResponse;
    }
}

void Server::resetStats() {
    totalSentResponses = 0;
    totalReceivedMessages = 0;

    totalResponseSpeed = 0;
    totalReceiveSpeed = 0;

    currentResponseSpeed = 0;
    currentReceiveSpeed = 0;
}

void Server::updateReceiveStats() {
    ++totalReceivedMessages;

    if (totalReceivedMessages == 1) {
        startOfReceive = boost::posix_time::microsec_clock::local_time();
        temporaryEndOfReceive = startOfReceive;
    }

    endOfReceive = boost::posix_time::microsec_clock::local_time();

    if (totalReceivedMessages >= 99 && totalReceivedMessages % PRECISION == 99) {
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

    totalReceiveSpeed = (totalReceivedMessages * RECEIVE_BUFFER_SIZE * 1000) / totalReceiveDuration;
    totalResponseSpeed = (totalSentResponses * RESPONSE_SIZE * 1000) / totalResponseDuration;

    currentReceiveSpeed = (PRECISION * RECEIVE_BUFFER_SIZE * 1000) / currentReceiveDuration;
    currentResponseSpeed = (PRECISION * RESPONSE_SIZE * 1000) / currentResponseDuration;

    if (totalReceivedMessages < PRECISION) {
        currentReceiveSpeed = totalReceiveSpeed;
    }

    if (totalSentResponses < PRECISION) {
        currentResponseSpeed = totalResponseSpeed;
    }
}