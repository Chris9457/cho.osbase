// \file  SharedMemory.cpp
// \brief Implementation of the class SharedMemory

#include "osCore/Interprocess/SharedMemory.h"
#include <Windows.h>

namespace cho::osbase::core {

    /*
     * \class SharedMemory
     */
    SharedMemory<void, true>::SharedMemory(const std::string &name, size_t size)
        : m_name(name), m_size(size), m_hMapFile(nullptr), m_pBuffer(nullptr), m_hMutex(nullptr) {
        const std::string mutexName = "mutex - " + name;
        m_hMutex                    = ::OpenMutexA(SYNCHRONIZE, TRUE, mutexName.c_str());
        if ((m_hMutex == nullptr) && (::GetLastError() == ERROR_FILE_NOT_FOUND))
            m_hMutex = ::CreateMutexA(nullptr, FALSE, mutexName.c_str());
    }

    SharedMemory<void, true>::~SharedMemory() {
        if (m_pBuffer != nullptr)
            ::UnmapViewOfFile(m_pBuffer);

        if (m_hMapFile != nullptr)
            ::CloseHandle(m_hMapFile);

        if (m_hMutex != nullptr) {
            unlock();
            ::CloseHandle(m_hMutex);
        }
    }

    bool SharedMemory<void, true>::create() {
        if (m_hMapFile != nullptr)
            return false;

        HANDLE hMapFile =
            ::CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(m_size), m_name.c_str());
        if (hMapFile == nullptr)
            return false;

        if (::GetLastError() == ERROR_ALREADY_EXISTS) {
            ::CloseHandle(hMapFile);
            return false;
        }

        m_hMapFile = hMapFile;

        if (m_hMapFile != nullptr)
            m_pBuffer = ::MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, m_size);

        return m_pBuffer != nullptr;
    }

    bool SharedMemory<void, true>::open() {
        if (m_hMapFile != nullptr)
            return false;

        m_hMapFile = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, m_name.c_str());

        if (m_hMapFile != nullptr)
            m_pBuffer = ::MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, m_size);

        return m_pBuffer != nullptr;
    }

    void SharedMemory<void, true>::getBuffer(void *pBuffer) const {
        if ((m_pBuffer != nullptr) && (pBuffer != nullptr))
            memcpy(pBuffer, m_pBuffer, m_size);
    }

    void SharedMemory<void, true>::setBuffer(const void *pBuffer) {
        if ((m_pBuffer != nullptr) && (pBuffer != nullptr))
            memcpy(m_pBuffer, pBuffer, m_size);
    }

    void SharedMemory<void, true>::lock() {
        if (m_hMutex != nullptr)
            ::WaitForSingleObject(m_hMutex, INFINITE);
    }

    void SharedMemory<void, true>::unlock() {
        if (m_hMutex != nullptr)
            ::ReleaseMutex(m_hMutex);
    }

    /*
     * \class SharedMemoryLocker
     */
    SharedMemoryLocker::SharedMemoryLocker(SharedMemory<void> &sharedMemory) : m_sharedMemory(sharedMemory) {
        m_sharedMemory.lock();
    }

    SharedMemoryLocker::~SharedMemoryLocker() {
        m_sharedMemory.unlock();
    }
} // namespace cho::osbase::core
