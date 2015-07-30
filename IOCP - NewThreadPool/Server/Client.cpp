#include "Client.h"
#include "common/Log.h"
#include "common/Network.h"

#include "common/CachedAlloc.h"

Client::Client()
    : m_pTPIO(NULL), m_Socket(INVALID_SOCKET), m_State(WAIT)
{
}

Client::~Client()
{
    Destroy();
}


bool Client::Create()
{
	m_State = WAIT;

	m_Socket = Network::CreateSocket(false, 0);
	if(m_Socket == INVALID_SOCKET)
	{
		ERROR_MSG("Could not create socket.");		
		return false;
	}
	return true;
}


void Client::Destroy()
{
	if( m_Socket != INVALID_SOCKET )
	{
		Network::CloseSocket(m_Socket);
		CancelIoEx(reinterpret_cast<HANDLE>(m_Socket), NULL);
		m_Socket = INVALID_SOCKET;
		m_State = DISCONNECTED;
	}

	if( m_pTPIO != NULL )
	{
		WaitForThreadpoolIoCallbacks( m_pTPIO, true );
		CloseThreadpoolIo( m_pTPIO );
		m_pTPIO = NULL;
	}
}