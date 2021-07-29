#include "server.hpp"

#include <iostream>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

namespace ba = boost::asio;

namespace{

void ReadThread(std::string s, ICommandsProcessor** cmd_ptr, StaticCommandsProcessor& static_cmd_processor, 
    DynamicCommandsProcessor& dynamic_cmd_processor, std::mutex& mutex)
{
    std::istringstream iss{ s };

    while (iss >> s)
    {
        std::lock_guard<std::mutex> lock(mutex);
        bool res = (*cmd_ptr)->ProcessCommand(s);

        if (res)
        {
            if (*cmd_ptr == &static_cmd_processor)
            {
                *cmd_ptr = &dynamic_cmd_processor;
            }
            else
            {
                *cmd_ptr = &static_cmd_processor;
            }
        }
    }
}

} // namespace

Session::Session(boost::asio::ip::tcp::socket socket, std::mutex& mutex, StaticCommandsProcessor& static_cmd_processor) :
    socket_(std::move(socket)), 
    data_{},
    static_cmd_processor_{ static_cmd_processor },
    mutex_{ mutex }
{
}

void Session::Start()
{
    Read();
}

void Session::Read()
{
    auto self(shared_from_this());

    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::string s{ data_, length };

                std::thread(ReadThread, std::move(s), &current_cmd_processor_, std::ref(static_cmd_processor_),
                    std::ref(dynamic_cmd_processor_), std::ref(mutex_)).detach();

                Read();
            }
            else
                static_cmd_processor_.ProcessCommand(EndOfFileString);
        });
}

TcpServer::TcpServer(boost::asio::io_context& io_context, short port, StaticCommandsProcessor& static_cmd_processor) :
    acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), 
    static_cmd_processor_{ static_cmd_processor }
{
    Accept();
}

void TcpServer::Accept()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<Session>(std::move(socket), mutex_, static_cmd_processor_)->Start();
            }

            Accept();
        });
}
