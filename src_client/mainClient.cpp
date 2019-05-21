#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>

using boost::asio::ip::udp;
using boost::thread;


int sendingMessages(const udp::endpoint& server_endpoint,udp::socket* socket){
    std::string message;
    int i=0;
    while(true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        std::cout << "Wyszedlem"<<std::endl;
        if(i!=100)
            message="client";
        else
            message="quit";
        socket->send_to(boost::asio::buffer(message), server_endpoint);
        std::cout << "Wyslalem"<<std::endl;
        if(message=="quit") break;
        ++i;
    }
    return 0;

}

int receivingMessages(udp::endpoint& server_endpoint,udp::socket* socket){
    std::string message;
    while(true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        boost::array<char, 100> recv_buf;
        boost::system::error_code error;
        std::cout<<"Odbieram"<<std::endl;
        size_t len = socket->receive_from(boost::asio::buffer(recv_buf), server_endpoint,0,error);
        std::cout << std::string(reinterpret_cast<const char*>(recv_buf.data()), len)<<std::endl;
        if(std::string(reinterpret_cast<const char*>(recv_buf.data()))=="Disconnected") break;
        std::cout<<"Powiadomienie"<<std::endl;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: client <host> <portnumber>" << std::endl;
            return 1;
        }

        boost::asio::io_service io_service;

        udp::resolver resolver(io_service);
        udp::resolver::query query(argv[1], argv[2]);
        udp::resolver::iterator iter = resolver.resolve(query);
        udp::endpoint server_endpoint = iter->endpoint();

        udp::socket socket(io_service);
        socket.open(udp::v4());
        std::string message = "does it work?";
        boost::thread sendThread(boost::bind(&sendingMessages,server_endpoint,&socket));
        boost::thread recvThread(boost::bind(&receivingMessages,server_endpoint,&socket));
        sendThread.join();
        recvThread.join();
        socket.close();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;

}
