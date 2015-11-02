#include "ClientMan.h"
#include "Client.h"

#include "common/Log.h"
#include "common/CritSecLock.h"

#include <cassert>

/* static */ void CALLBACK
ClientMan::WorkerRemoveClient(PTP_CALLBACK_INSTANCE /* Instance */, PVOID Context)
{
    Client* client = static_cast<Client*>(Context);
    assert(client);

    ClientMan::Instance()->RemoveClient(client);
}

ClientMan::ClientMan() { InitializeCriticalSection(&m_CSForClients); }

ClientMan::~ClientMan()
{
    RemoveClients();

    DeleteCriticalSection(&m_CSForClients);
}

void ClientMan::AddClients(int numClients)
{
    CritSecLock lock(m_CSForClients);

    for (int i = 0; i < numClients; ++i)
    {
        Client* client = new Client();

        if (client->Create(0))
        {
            m_listClient.push_back(client);
        }
        else
        {
            delete client;
        }
    }
}

void ClientMan::ConnectClients(const char* ip, u_short port)
{
    CritSecLock lock(m_CSForClients);

    for (int i = 0; i != static_cast<int>(m_listClient.size()); ++i)
    {
        m_listClient[i]->PostConnect(ip, port);
    }
}

void ClientMan::ShutdownClients()
{
    CritSecLock lock(m_CSForClients);

    for (int i = 0; i != static_cast<int>(m_listClient.size()); ++i)
    {
        m_listClient[i]->Shutdown();
    }
}

void ClientMan::RemoveClients()
{
    CritSecLock lock(m_CSForClients);

    for (int i = 0; i != static_cast<int>(m_listClient.size()); ++i)
    {
        delete m_listClient[i];
    }
    m_listClient.clear();
}

void ClientMan::Send(const std::string& msg)
{
    CritSecLock lock(m_CSForClients);

    for (int i = 0; i != static_cast<int>(m_listClient.size()); ++i)
    {
        m_listClient[i]->PostSend(msg.c_str(), msg.length());
    }
}

void ClientMan::PostRemoveClient(Client* client)
{
    if (!TrySubmitThreadpoolCallback(ClientMan::WorkerRemoveClient, client, NULL))
    {
        ERROR_CODE(GetLastError(), "Could not start WorkerRemoveClient.");

        // This is not good. We should remove the client in a different thread to wait until its IO
        // operations are complete.
        // You need a fallback strategy. DO NOT JUST FOLLOW THIS EXAMPLE. YOU HAVE BEEN WARNED.
        RemoveClient(client);
    }
}

void ClientMan::RemoveClient(Client* client)
{
    CritSecLock lock(m_CSForClients);

    ClientList::iterator itor = find(m_listClient.begin(), m_listClient.end(), client);

    if (itor != m_listClient.end())
    {
        delete *itor;
        m_listClient.erase(itor);
    }
}

size_t ClientMan::GetNumClients()
{
    CritSecLock lock(m_CSForClients);

    return m_listClient.size();
}

bool ClientMan::IsAlive(const Client* client)
{
    CritSecLock lock(m_CSForClients);

    ClientList::const_iterator itor = find(m_listClient.begin(), m_listClient.end(), client);
    return itor != m_listClient.end();
}
