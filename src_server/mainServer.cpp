#include "Server.h"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "Required arguments: <portNumber> <interval between responses in microseconds>" << std::endl;
        return 1;
    }

    unsigned exposedPort = std::stoi(argv[1]);
    unsigned long long intervalBetweenResponses = std::stoull(argv[2]);

    boost::asio::io_service io_service;

    Server server(io_service, exposedPort, intervalBetweenResponses);

    server.start();

    return 0;
}
