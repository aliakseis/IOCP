#include "IOEvent.h"
#include "Client.h"
#include "Packet.h"

#include "common/CachedAlloc.h"

namespace {

CachedAlloc eventAllocator(sizeof(IOEvent));

}

/* static */ IOEvent* IOEvent::Create(Type type, Client* client, Packet* packet)
{
    IOEvent* event = static_cast<IOEvent*>(eventAllocator.get());

	ZeroMemory(event, sizeof(IOEvent));
	event->m_Client = client;
	event->m_Type = type;
	event->m_Packet = packet;

	return event;	
}

/* static */ void IOEvent::Destroy(IOEvent* event)
{
    eventAllocator.put(event);
}

