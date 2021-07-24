#include "server.hpp"

#include <iostream>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

namespace ba = boost::asio;

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