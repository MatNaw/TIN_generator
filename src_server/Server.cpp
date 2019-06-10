//
// Created by mivine on 26.05.19.
//

#include "Server.h"

#define DISCONNECT_MESSAGE "Disconnected"
#define TRANSMISSION_END_MESSAGE "End"
#define TRANSMISSION_BEGIN_MESSAGE "Begin"

#define SHUTDOWN_SERVER_COMMAND "exit"

#define NANOSECONDS_PER_SECOND 1000000000LL

#define PRECISION 100

using boost::asio::ip::udp;

Server::Server(boost::asio::io_service &io_service, unsigned exposedPort, unsigned long long responseInterval)
        : socket(udp::socket(io_service, udp::endpoint(udp::v4(), exposedPort))), intervalBetweenResponses(responseInterval) {
}

void Server::processUserInput() {
    while (userCommand != SHUTDOWN_SERVER_COMMAND) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
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
        boost::this_thread::sleep(boost::posix_time::microseconds(intervalBetweenResponses));

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
        size_t len = socket.receive_from(boost::asio::buffer(receiveBuf), remoteEndpoint, 0, error);

        std::string receivedMessage = std::string(receiveBuf.data(), len);

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

            std::string receivedMessage = std::string(receiveBuf.data(), len);

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

    long long totalResponseDuration = totalResponseDurationPosix.total_nanoseconds();
    long long totalReceiveDuration = totalReceiveDurationPosix.total_nanoseconds();
    long long currentResponseDuration = currentResponseDurationPosix.total_nanoseconds();
    long long currentReceiveDuration = currentReceiveDurationPosix.total_nanoseconds();

    totalReceiveSpeed = (totalReceivedMessages * RECEIVE_BUFFER_SIZE * NANOSECONDS_PER_SECOND) / totalReceiveDuration;
    totalResponseSpeed = (totalSentResponses * RESPONSE_SIZE * NANOSECONDS_PER_SECOND) / totalResponseDuration;

    currentReceiveSpeed = (PRECISION * RECEIVE_BUFFER_SIZE * NANOSECONDS_PER_SECOND) / currentReceiveDuration;
    currentResponseSpeed = (PRECISION * RESPONSE_SIZE * NANOSECONDS_PER_SECOND) / currentResponseDuration;

    if (totalReceivedMessages < PRECISION) {
        currentReceiveSpeed = totalReceiveSpeed;
    }

    if (totalSentResponses < PRECISION) {
        currentResponseSpeed = totalResponseSpeed;
    }
}