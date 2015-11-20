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

ClientMan::ClientMan()
    : m_hNoClients(CreateEvent(NULL, TRUE, FALSE, NULL))
{ 
    InitializeCriticalSection(&m_CSForClients); 
}

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

    for (auto client : m_listClient)
    {
        client->PostConnect(ip, port);
    }
}

void ClientMan::ShutdownClients()
{
    CritSecLock lock(m_CSForClients);

    for (auto client : m_listClient)
    {
        client->Shutdown();
    }
}

void ClientMan::RemoveClients()
{
    EnterCriticalSection(&m_CSForClients);
    for (auto client : m_listClient)
    {
        client->Close();
    }
    const bool isEmpty = m_listClient.empty();
    if (!isEmpty)
    {
        ResetEvent(m_hNoClients);
    }
    LeaveCriticalSection(&m_CSForClients);

    if (!isEmpty)
    {
        WaitForSingleObject(m_hNoClients, INFINITE);
    }
}

void ClientMan::Send(const std::string& msg)
{
    CritSecLock lock(m_CSForClients);

    for (auto client : m_listClient)
    {
        client->PostSend(msg.c_str(), msg.length());
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

        if (m_listClient.empty())
        {
            SetEvent(m_hNoClients);
        }
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
