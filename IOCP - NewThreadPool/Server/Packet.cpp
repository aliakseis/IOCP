#include "Packet.h"

#include "common/CachedAlloc.h"

namespace {

CachedAlloc packetAllocator(sizeof(Packet));

}

/* static */ Packet* Packet::Create(Client* sender, const BYTE* buff, DWORD size)
{
    Packet* packet = static_cast<Packet*>(packetAllocator.get());
	packet->m_Sender = sender; 
	packet->m_Size = size;
	CopyMemory(packet->m_Data, buff, size);

	return packet;
}

/* static */ void Packet::Destroy(Packet* packet)
{
    packetAllocator.put(packet);
}

