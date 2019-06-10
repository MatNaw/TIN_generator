//
// Created by mivine on 26.05.19.
//

#include "Client.h"

#define TRANSMISSION_BEGIN_MESSAGE "Begin"
#define TRANSMISSION_END_MESSAGE "End"
#define SERVER_DISCONNECTED_MESSSAGE "Disconnected"

#define NANOSECONDS_PER_SECOND 1000000000LL

#define PRECISION 100

using boost::asio::ip::udp;
using boost::thread;

Client::Client(boost::asio::io_service &io_service, const std::string &host, const std::string &port,
               unsigned long long numberOfPackages, unsigned long long packagesInterval)
        : resolver(udp::resolver(io_service)), query(udp::resolver::query(host, port)),
          iterator(resolver.resolve(query)), server_endpoint(iterator->endpoint()), socket(udp::socket(io_service)),
          numberOfPackagesToSend(numberOfPackages), intervalBetweenPackages(packagesInterval) {
}

void Client::sendMessages() {
    socket.send_to(boost::asio::buffer(TRANSMISSION_BEGIN_MESSAGE), server_endpoint);
    while (true) {
        boost::this_thread::sleep(boost::posix_time::microseconds(intervalBetweenPackages));
        updateSendingStats();

        if (totalSentMessages != numberOfPackagesToSend) {
            socket.send_to(boost::asio::buffer(sendBuf), server_endpoint);
        } else {
            socket.send_to(boost::asio::buffer(TRANSMISSION_END_MESSAGE), server_endpoint);
            calculateTransmissionSpeed();
            showTransmissionSpeed();
            break;
        }
    }
}

void Client::receiveMessages() {

    while (true) {
        boost::system::error_code error;

        size_t len = socket.receive_from(boost::asio::buffer(recvBuf), server_endpoint, 0, error);
        std::string receivedMessage = std::string(recvBuf.data());

        if (receivedMessage.find(SERVER_DISCONNECTED_MESSSAGE) != std::string::npos) {
            std::cout << "Transmission ended" << std::endl;
            transmissionEnd = true;
            break;
        }

        if (len > 0) {
            updateReceiveStats();
        }
    }
}


void Client::transmitToServer() {

    try {
        socket.open(udp::v4());

        boost::thread userInputThread(boost::bind(&Client::processUserInput, this));
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

void Client::processUserInput() {
    while (!transmissionEnd) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        std::cin >> userCommand;

        if (userCommand == "speed") {
            calculateTransmissionSpeed();
            showTransmissionSpeed();
        }
    }
}

void Client::showTransmissionSpeed() {
    std::cout << std::endl;
    std::cout << "Current receive speed: " << currentReceiveSpeed << " Bytes per second" << std::endl;
    std::cout << "Current sending speed: " << currentSendingSpeed << " Bytes per second \n" << std::endl;

    std::cout << "Total receive speed: " << totalReceiveSpeed << " Bytes per second" << std::endl;
    std::cout << "Total sending speed: " << totalSendingSpeed << " Bytes per second \n" << std::endl;

    std::cout << "Received packages: " << totalReceivedMessages << std::endl;
    std::cout << "Sent packages: " << totalSentMessages << std::endl;
}

void Client::updateSendingStats() {
    ++totalSentMessages;

    if (totalSentMessages == 1) {
        startOfSending = boost::posix_time::microsec_clock::local_time();
        temporaryEndOfResponse = startOfSending;
    }

    endOfSending = boost::posix_time::microsec_clock::local_time();

    if (totalSentMessages >= 99 && totalSentMessages % PRECISION == 99) {
        temporaryStartOfResponse = temporaryEndOfResponse;
        temporaryEndOfResponse = endOfSending;
    }
}

void Client::updateReceiveStats() {
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

void Client::calculateTransmissionSpeed() {
    boost::posix_time::time_duration totalResponseDurationPosix = endOfSending - startOfSending;
    boost::posix_time::time_duration totalReceiveDurationPosix = endOfReceive - startOfReceive;
    boost::posix_time::time_duration currentResponseDurationPosix = temporaryEndOfResponse - temporaryStartOfResponse;
    boost::posix_time::time_duration currentReceiveDurationPosix = temporaryEndOfReceive - temporaryStartOfReceive;

    long long totalResponseDuration = totalResponseDurationPosix.total_nanoseconds();
    long long totalReceiveDuration = totalReceiveDurationPosix.total_nanoseconds();
    long long currentResponseDuration = currentResponseDurationPosix.total_nanoseconds();
    long long currentReceiveDuration = currentReceiveDurationPosix.total_nanoseconds();

    totalReceiveSpeed = (totalReceivedMessages * RECEIVE_BUFFER_SIZE * NANOSECONDS_PER_SECOND) / totalReceiveDuration;
    totalSendingSpeed = (totalSentMessages * MESSAGE_SIZE * NANOSECONDS_PER_SECOND) / totalResponseDuration;

    currentReceiveSpeed = (PRECISION * RECEIVE_BUFFER_SIZE * NANOSECONDS_PER_SECOND) / currentReceiveDuration;
    currentSendingSpeed = (PRECISION * MESSAGE_SIZE * NANOSECONDS_PER_SECOND) / currentResponseDuration;

    if (totalReceivedMessages < PRECISION) {
        currentReceiveSpeed = totalReceiveSpeed;
    }

    if (totalSentMessages < PRECISION) {
        currentSendingSpeed = totalSendingSpeed;
    }
}