// \file  SharedMemory.h
// \brief Declaration of the class SharedMemory

#pragma once
#include <mutex>
#include <string>

namespace cho::osbase::core {

    /**
     * \defgroup PACKAGE_SHAREDMEMORY Shared Memory
     *
     * \brief Set of classes to manage shared memories
     *
     * \ingroup PACKAGE_INTERPROCESS
     * \{
     */

    /**
     * \brief	This class provides access to a shared memory segment.
     *
     * This class provides access to a shared memory segment by multiple threads and processes.
     * It also provides a way for a single thread or process to lock the memory for exclusive access.
     */
    template <typename T, bool = std::is_void<T>::value>
    class SharedMemory;

    /**
     * \brief	This specialization implements the common "typeless" method
     */
    template <>
    class SharedMemory<void, true> {
    public:
        bool create(); //!< Create a new shared memory segment - Fails if the shared memory is already existing.
        bool open();   //!< Open an existing shared memory segment - Fails if the shared memory does not exist.
        void lock();   //!< Exclusive inter thread & process lock.
        void unlock(); //!< Unlock the access.

    protected:
        SharedMemory(const std::string &name, size_t size); //!< Ctor - name is the name of the shared memory, size the size of the
                                                            //!< associated buffer
        ~SharedMemory();                                    //!< Dtor

        void getBuffer(void *pBuffer) const; //!< Copy the shared memory buffer into pBuffer - no memory checked performed
        void setBuffer(const void *pBuffer); //!< Copy pBuffer into the shared memory buffer - no memory checked performed

    private:
        std::string m_name;
        size_t m_size;
        void *m_hMapFile;
        void *m_pBuffer;
        void *m_hMutex;
        std::recursive_mutex m_mutex;
    };

    /**
     * \brief	This specialization implements the "typed" methods
     */
    template <typename T>
    class SharedMemory<T, false> : public SharedMemory<void, true> {
    public:
        SharedMemory(const std::string &name); //!< Ctor - name is the name of the memory

        T get() const;            //!< Create an instance of T from the shared memory buffer
        void set(const T &value); //!< Dump "value" into the shared memory buffer
    };

    /**
     * \brief	This class helper is a RAII implementation for the lock feature
     */
    class SharedMemoryLocker {
    public:
        SharedMemoryLocker(SharedMemory<void> &sharedMemory); //!< Ctor
        ~SharedMemoryLocker();                                //!< Dtor

    private:
        SharedMemory<void> &m_sharedMemory;
    };

    /** \} */

} // namespace cho::osbase::core

/** \cond */
#include "sharedmemory.inl"
/** \endcond */