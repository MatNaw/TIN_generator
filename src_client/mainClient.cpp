#include <iostream>
#include "Client.h"

int main(int argc, char *argv[]) {

    if (argc != 5) {
        std::cerr << "Required arguments: <host> <portNumber> <numberOfPackagesToSent> <interval between packages in microseconds>"
                  << std::endl;
        return 1;
    }

    boost::asio::io_service io_service;

    std::string host = argv[1];
    std::string port = argv[2];
    long long numberOfPackagesToSend = std::stoll(argv[3]);
    long long desiredSendingSpeed = std::stol(argv[4]);

    Client client(io_service, host, port, numberOfPackagesToSend, desiredSendingSpeed);

    client.transmitToServer();

    return 0;
}