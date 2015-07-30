
#include <string>
#include <iostream>

#include "common/Log.h"
#include "common/Network.h"
#include "Server.h"

using std::string;
using std::cin;

void main(int argc, char* argv[])
{
	Log::Setup();

	if( argc != 3)
	{
		TRACE("Please add port and max number of accept posts.");
		TRACE("(ex) 17000 100");
		return;
	}

	u_short port = static_cast<u_short>( atoi(argv[1]) );
	int maxPostAccept = atoi(argv[2]);

	TRACE("Input : port : %d, max accept : %d", port, maxPostAccept);

	if (!Network::Initialize())
	{
		ERROR_MSG("Network::Initialize() failed");
		return;
	}

	Server::New();
	
	if (!Server::Instance()->Create(port, maxPostAccept))
	{
		ERROR_MSG("Server::Create() failed");
		Network::Deinitialize();
		return;
	}

#ifndef _DEBUG
	Log::EnableTrace(false);
#endif

	string input;
	bool loop = true;
	while(loop)
	{
		std::getline(cin, input);

		if(input == "`client_size")
		{
			TRACE(" Number of Clients : %d", Server::Instance()->GetNumClients());
		}
		if(input == "`accept_size")
		{
			TRACE(" Number of Accept posts : %d", Server::Instance()->GetNumPostAccepts());
		}
		else if(input == "`enable_trace")
		{
			Log::EnableTrace(true);
		}
		else if(input == "`disable_trace")
		{
			Log::EnableTrace(false);
		}
	}

	Server::Delete();

	Network::Deinitialize();

	Log::Cleanup();

	return;
}
