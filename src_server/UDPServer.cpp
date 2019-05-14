#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        udp::socket socket(io_service, udp::endpoint(udp::v4(), 2048));

        for (;;)
        {
            boost::array<char, 100> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;
            size_t len = socket.receive_from(boost::asio::buffer(recv_buf),
                                remote_endpoint, 0, error);
            if(strcmp(recv_buf.data(),""))
                std::cout << std::string(reinterpret_cast<const char*>(recv_buf.data()), len) << std::endl;

            if (error && error != boost::asio::error::message_size)
                throw boost::system::system_error(error);
            std::string message = "dziala dla mnie";//make_daytime_string();

            if(strcmp(recv_buf.data(),"quit"))
                message="Disconnected";

            boost::system::error_code ignored_error;
            socket.send_to(boost::asio::buffer(message),
                           remote_endpoint, 0, ignored_error);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}