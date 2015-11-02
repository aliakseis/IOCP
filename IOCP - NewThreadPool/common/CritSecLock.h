#pragma once

#include <Windows.h>

class CritSecLock
{
public:
    CritSecLock(CRITICAL_SECTION& critsec) : m_critsec(&critsec)
    {
        EnterCriticalSection(m_critsec);
    }
    ~CritSecLock()
    {
        LeaveCriticalSection(m_critsec);
    }
    CritSecLock(const CritSecLock&) = delete;
    CritSecLock& operator=(const CritSecLock&) = delete;

private:
    CRITICAL_SECTION* m_critsec;
};
