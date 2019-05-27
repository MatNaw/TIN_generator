#include "Server.h"

int main(int argc, char *argv[]) {

        if (argc != 2) {
            std::cerr << "Required arguments: <portNumber>" << std::endl;
            return 1;
        }

        int exposedPort = std::stoi(argv[1]);

        boost::asio::io_service io_service;

        Server server(io_service, exposedPort);

        server.start();

    return 0;
}
