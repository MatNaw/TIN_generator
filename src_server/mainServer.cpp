#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>



using boost::asio::ip::udp;
boost::mutex mut;
bool stopTransmission=false;

int sendingMessages(const udp::endpoint& receiver_endpoint,udp::socket* socket){
    std::string message;
    while(true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        //boost::unique_lock<boost::mutex> lock(mut);
        /*while(!response){
            cond.wait(lock);
        }*/
        std::cout << "Wyszedlem"<<std::endl;
        message="next packet";
        socket->send_to(boost::asio::buffer(message), receiver_endpoint);
        std::cout << "Wyslalem "<<std::endl;
        {
            boost::unique_lock<boost::mutex> lock(mut);
            std::cout<<stopTransmission<<std::endl;
            if(stopTransmission) break;
        }

    }
    message="Disconnected";
    socket->send_to(boost::asio::buffer(message), receiver_endpoint);
    return 0;

}

int receivingMessages(udp::endpoint& sender_endpoint,udp::socket* socket){
    std::string message;
    while(true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(250));
        boost::array<char, 128> recv_buf;
        boost::system::error_code error;
        //udp::endpoint sender_endpoint;
        //boost::unique_lock<boost::mutex> lock(mut);
        std::cout<<"Odbieram"<<std::endl;
        size_t len = socket->receive_from(boost::asio::buffer(recv_buf), sender_endpoint,0,error);
        std::cout << std::string(reinterpret_cast<const char*>(recv_buf.data()), len)<<std::endl;
        if(std::string(reinterpret_cast<const char*>(recv_buf.data()),len)=="quit") {
            boost::unique_lock<boost::mutex> lock(mut);
            stopTransmission = true;
            std::cout<<stopTransmission<<std::endl;
            break;
        }
        std::cout<<"Powiadomienie"<<std::endl;
        //response = true;
        //lock.unlock();
        //cond.notify_one();
    }
    return 0;
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
            if(std::string(reinterpret_cast<const char*>(recv_buf.data()), len)!="")
                std::cout << std::string(reinterpret_cast<const char*>(recv_buf.data()), len) << std::endl;

            if (error && error != boost::asio::error::message_size)
                throw boost::system::system_error(error);
            if(len>0){
                boost::thread sendThread(boost::bind(&sendingMessages,remote_endpoint,&socket));
                boost::thread recvThread(boost::bind(&receivingMessages,remote_endpoint,&socket));

                sendThread.join();
                recvThread.join();
            }
            /*
            if(std::string(reinterpret_cast<const char*>(recv_buf.data()), len)=="quit")
            {
                std::cout<<recv_buf.data();
                message="Ja wykurwiam";
            }
            boost::system::error_code ignored_error;
            socket.send_to(boost::asio::buffer(message),
                           remote_endpoint, 0, ignored_error);
                           */
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
