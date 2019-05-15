//
// Created by dominik on 5/15/19.
//

#include "UDPClient.h"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
/*
using boost::asio::ip::udp;


UDPClient::UDPClient(boost::asio::io_service &io_service):
        socket_(io_service, udp::endpoint(udp::v4(), 13))
{
    start_receive();
}

void UDPClient::start_receive()
{
    socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&UDPClient::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
}

void UDPClient::handle_receive(const boost::system::error_code& error,
                    std::size_t /*bytes_transferred*//*)
{
    if (!error || error == boost::asio::error::message_size)
    {
        boost::shared_ptr<std::string> message(
                new std::string(make_daytime_string()));

        socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                              boost::bind(&UDPClient::handle_send, this, message,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));

        start_receive();
    }
}

//void UDPClient::handle_send(boost::shared_ptr<std::string> /*message*///,
            /*     const boost::system::error_code& /*error*///,
            /*     std::size_t /*bytes_transferred*///)
/*{
}
*/
