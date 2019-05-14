#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;
using boost::thread;

int sendingMessages(const udp::endpoint& receiver_endpoint,udp::socket& socket){
    std::string message;
    while(std::cin>>message) {
        socket.send_to(boost::asio::buffer(message), receiver_endpoint);
        if(message=="quit") break;
    }
    return 0;

}

int receivingMessages(const udp::endpoint& receiver_endpoint,udp::socket& socket){
    std::string message;
    while(true) {
        boost::array<char, 128> recv_buf;
        udp::endpoint sender_endpoint;
        socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
        std::cout << recv_buf.data()<<std::endl;
        if(strcmp(recv_buf.data(),"Disconnected")) break;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    try
    {
        /*if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }*/

        boost::asio::io_service io_service;

        udp::resolver resolver(io_service);
        udp::resolver::query query("localhost", "2048");
        udp::resolver::iterator iter = resolver.resolve(query);
        udp::endpoint receiver_endpoint = iter->endpoint();

        udp::socket socket(io_service);
        socket.open(udp::v4());
        std::string message = "does it work?";


       // while(std::cin>>message) {
            //if(message=="quit") break;
            boost::thread sendThread(sendingMessages,receiver_endpoint,&socket);
            sendThread.join();

            //socket.send_to(boost::asio::buffer(message), receiver_endpoint);

           // boost::array<char, 128> recv_buf;
            udp::endpoint sender_endpoint;
            boost::thread recvThread(receivingMessages,sender_endpoint,&socket);
            recvThread.join();

            //std::cout.write(recv_buf.data(), len);
            //std::cout << recv_buf.data()<<std::endl;
        //}
        //socket.close();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}