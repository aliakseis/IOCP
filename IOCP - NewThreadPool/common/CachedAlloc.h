#pragma once

#include <windows.h>
#include <malloc.h>

class CCachedAlloc
{
public:
	CCachedAlloc(size_t size)
		: m_size(size)
	{
		if (m_size < sizeof(SLIST_ENTRY))
		{
			m_size = sizeof(SLIST_ENTRY);
		}
		m_pListHead = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER),
												MEMORY_ALLOCATION_ALIGNMENT);
		InitializeSListHead(m_pListHead);
	}
	~CCachedAlloc()
	{
		 _aligned_free(m_pListHead);
	}

	void put(void* pData)
	{
		InterlockedPushEntrySList(m_pListHead, (PSINGLE_LIST_ENTRY) pData);
	}

	void* get()
	{
		void* result = InterlockedPopEntrySList(m_pListHead);
		if (NULL == result)
		{
			result = _aligned_malloc(m_size,
									MEMORY_ALLOCATION_ALIGNMENT);
		}

		return result;
	}

	size_t getSize()
	{
		return m_size;
	}

private:
    CCachedAlloc(const CCachedAlloc&) = delete;
    CCachedAlloc& operator = (const CCachedAlloc&) = delete;

private:
	size_t m_size;
	PSLIST_HEADER m_pListHead;
};
