#pragma once
#include <Windows.h>

class Client;
class Packet
{
private:
	enum
	{
		MAX_BUFF_SIZE = 1024,
	};
	
public:
	static Packet* Create(Client* sender, const BYTE* buff, DWORD size);
	static void Destroy(Packet* packet);

public:
	Client* GetSender() const { return m_Sender; }
    DWORD GetSize() const { return m_Size; }
	BYTE* GetData() { return m_Data; }

private:
	Packet();
	~Packet();
    Packet(const Packet&) = delete;
    Packet& operator=(const Packet&) = delete;

private:
	Client* m_Sender;
	DWORD m_Size;
	BYTE m_Data[MAX_BUFF_SIZE];
};
