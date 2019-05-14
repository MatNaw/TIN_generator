#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_service io_service;

        udp::resolver resolver(io_service);
        udp::resolver::query query("localhost", "2048");
        udp::resolver::iterator iter = resolver.resolve(query);
        udp::endpoint receiver_endpoint = iter->endpoint();

        udp::socket socket(io_service);
        socket.open(udp::v4());

        boost::array<char, 1> send_buf  = {{ 0 }};
        std::string message = "does it work?";
        socket.send_to(boost::asio::buffer(message), receiver_endpoint);

        boost::array<char, 128> recv_buf;
        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(
                boost::asio::buffer(recv_buf), sender_endpoint);

        //std::cout.write(recv_buf.data(), len);
        std::cout<<recv_buf.data();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}