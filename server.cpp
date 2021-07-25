#include "server.hpp"

#include <iostream>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

namespace ba = boost::asio;
/*
void client_session(ba::ip::tcp::socket sock, Command& cmd, std::mutex& mutex)
{
    std::cout << "client!!!\n";

    while (true)
    {
        try
        {
            char data[1024];
            size_t len = sock.read_some(ba::buffer(data));
            std::string s{ data, len };
            //std::cout << "receive " << len << "=" << s << std::endl;
            //ba::write(sock, ba::buffer("pong", 4));
            std::istringstream iss{s};

            while (iss >> s)
            {
                std::lock_guard<std::mutex> lock(mutex);
                cmd.ProcessCommand(s);
            }
        }
        catch (const std::exception& e)
        {
            //std::cerr << "client_session exception! " << e.what() << std::endl;
            cmd.ProcessCommand(EndOfFileString);
            break;
        }
    }
}

void Server(uint16_t port, Command& cmd)
{
    ba::io_context io_context;
    ba::ip::tcp::endpoint ep( ba::ip::tcp::v4(), port );
    //std::cout << "ip " << ep.address() << std::endl;
    ba::ip::tcp::acceptor acc(io_context, ep);
    std::mutex mutex;

    while (true) 
    {
        auto sock = ba::ip::tcp::socket(io_context);
        acc.accept(sock);
        std::thread(client_session, std::move(sock), std::ref(cmd), std::ref(mutex)).detach();
    }
}
*/

/// //////////////////////////////////////////////////////////////////////////////////


void ReadThread(std::string s, Command* cmd_ptr, StaticCommand& static_cmd, DynamicCommand& dynamic_cmd, std::mutex& mutex)
{
    std::istringstream iss{ s };

    while (iss >> s)
    {
        std::lock_guard<std::mutex> lock(mutex);
        bool res = cmd_ptr->ProcessCommand(s);

        if (res)
        {
            if (cmd_ptr == &static_cmd)
            {
                std::cout << "dinamic cmd\n";
                cmd_ptr = &dynamic_cmd;
            }
            else
            {
                std::cout << "static cmd\n";
                cmd_ptr = &static_cmd;
            }
        }
    }
}

Session::Session(boost::asio::ip::tcp::socket socket, std::mutex& mutex, StaticCommand& static_cmd) :
    socket_(std::move(socket)), 
    static_cmd_{static_cmd},
    data_{},
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
                //std::cout << "receive " << length << "=" << std::string{ data_, length } << std::endl;

                std::string s{ data_, length };

                std::thread(ReadThread, std::move(s), currnet_cmd_, std::ref(static_cmd_), std::ref(dynamic_cmd_), std::ref(mutex_)).detach();

                Read();
            }
            else
                static_cmd_.ProcessCommand(EndOfFileString);
        });
}

TcpServer::TcpServer(boost::asio::io_context& io_context, short port, StaticCommand& static_cmd) :
    acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), 
    static_cmd_{ static_cmd }
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