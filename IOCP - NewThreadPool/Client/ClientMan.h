#pragma once
#include <winsock2.h>
#include <vector>
#include <string>

#include "common/TSingleton.h"

class Client;


class ClientMan : public TSingleton<ClientMan>
{
private:
	static void CALLBACK WorkerRemoveClient(PTP_CALLBACK_INSTANCE /* Instance */, PVOID Context);

public:
	ClientMan();
	virtual ~ClientMan();

	void AddClients(int numClients);
	void ConnectClients(const char* ip, u_short port);
	void ShutdownClients();
	void RemoveClients();
	void PostRemoveClient(Client* client);
	void Send(const std::string& msg);

	bool IsAlive(const Client* client);
	size_t GetNumClients();

private:
	void RemoveClient(Client* client);


private:
	typedef std::vector<Client*> ClientList;
	ClientList m_listClient;

	CRITICAL_SECTION m_CSForClients;
};
