#include <iostream>
#include "Client.h"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "Required arguments: <host> <portNumber>" << std::endl;
        return 1;
    }

    boost::asio::io_service io_service;

    std::string host = argv[1];
    std::string port = argv[2];

    Client client(io_service, host, port);

    client.transmitToServer();

    return 0;
}
