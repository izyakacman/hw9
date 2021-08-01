#include <iostream>
#include <string>

#include "server.hpp"
#include "commands_processor.hpp"

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
		StaticCommandsProcessor static_cmd_processor{ static_cast<size_t>(atoll(argv[2])) };

		boost::asio::io_context io_context;
        std::mutex mutex;

		TcpServer server(io_context, static_cast<uint16_t>(stoi(argv[1])), static_cmd_processor, mutex);

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait(
            [&io_context](boost::system::error_code const&, int)
            {
                // Stop the io_context. This will cause run()
                // to return immediately, eventually destroying the
                // io_context and any remaining handlers in it.
                io_context.stop();
            });

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> threads;
        threads.reserve(5);

        for (int i = 0; i < 5; ++i)
        {
            threads.emplace_back(
                [&io_context]
                {
                    io_context.run();
                });
        }

        // (If we get here, it means we got a SIGINT or SIGTERM)

        // Block until all the threads exit
        for (auto& t : threads)
            t.join();

	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << "\n";
	}
}
