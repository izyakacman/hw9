#include <iostream>
#include <string>

#include "server.hpp"
#include "command.h"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << "Usage: bulk_server <port> <commands count>";
		return -1;
	}

	try
	{
		StaticCommand static_cmd{ static_cast<size_t>(atoll(argv[2])) };

		boost::asio::io_context io_context;

		TcpServer server(io_context, static_cast<uint16_t>(stoi(argv[1])), static_cmd);

		io_context.run();

	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << "\n";
	}
}
