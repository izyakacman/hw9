#include "server.hpp"

#include <iostream>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

namespace ba = boost::asio;

Session::Session(boost::asio::ip::tcp::socket socket, std::mutex& mutex, StaticCommand& static_cmd) :
    socket_(std::move(socket)), 
    data_{},
    static_cmd_{ static_cmd },
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

                std::istringstream iss{ s };

                while (iss >> s)
                {
                    bool res;

                    {
                        std::lock_guard<std::mutex> lock(mutex_);
                        res = current_cmd_->ProcessCommand(s);
                    }

                    if (res)
                    {
                        if (current_cmd_ == &static_cmd_)
                        {
                            current_cmd_ = &static_cmd_;
                        }
                        else
                        {
                            current_cmd_ = &static_cmd_;
                        }
                    }
                }

                Read();
            }
            else
                static_cmd_.ProcessCommand(EndOfFileString);
        });
}

TcpServer::TcpServer(boost::asio::io_context& io_context, short port, StaticCommand& static_cmd, std::mutex& mutex) :
    acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), 
    static_cmd_{ static_cmd },
    mutex_{ mutex }
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
                std::make_shared<Session>(std::move(socket), mutex_, static_cmd_)->Start();
            }

            Accept();
        });
}
