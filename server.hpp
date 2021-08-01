#pragma once

#include <stdint.h>

#include <boost/asio.hpp>

#include "commands_processor.hpp"

class Session : public std::enable_shared_from_this<Session>
{
public:

    Session(boost::asio::ip::tcp::socket socket, std::mutex& mutex, StaticCommandsProcessor& static_cmd_processor);

    void Start();

private:

    void Read();

    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    StaticCommandsProcessor& static_cmd_processor_;
    DynamicCommandsProcessor dynamic_cmd_processor_;
    ICommandsProcessor* current_cmd_processor_ = &static_cmd_processor_;
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