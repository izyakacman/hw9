#include <iostream>
#include <string>

#include "server.hpp"
#include "command.h"

using namespace std;

int main(int argn, char** argc)
{
	if (argn != 3)
	{
		cout << "Usage: bulk_server <port> <commands count>";
		return -1;
	}

	Command cmd{ static_cast<size_t>(atoll(argc[2])) };

	Server( static_cast<uint16_t>(stoi(argc[1])), cmd );
}
