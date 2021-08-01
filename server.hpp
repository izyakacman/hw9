#pragma once

#include <stdint.h>

#include <boost/asio.hpp>

#include "command.h"

class Session : public std::enable_shared_from_this<Session>
{
public:

    Session(boost::asio::ip::tcp::socket socket, std::mutex& mutex, StaticCommand& static_cmd);

    void Start();

private:

    void Read();

    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    StaticCommand& static_cmd_;
    DynamicCommand dynamic_cmd_;
    ICommand* current_cmd_ = &static_cmd_;
    std::mutex& mutex_;
};

class TcpServer
{
public:

    TcpServer(boost::asio::io_context& io_context, short port, StaticCommand& static_cmd, std::mutex& mutex);

private:

    void Accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    StaticCommand& static_cmd_;
    std::mutex& mutex_;
};