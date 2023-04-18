// \file  Singleton.inl
// \brief Implementation of the class Singleton

namespace cho::osbase::core {
    /*
     *  \class Singleton<T>
     */

    template <class T>
    std::recursive_mutex *Singleton<T>::m_pDummyLock = &Singleton<T>::getLock();

    template <class T>
    Singleton<T>::~Singleton() {
    }

    template <class T>
    T &Singleton<T>::getInstance() {
        static T *pInstance = nullptr;

        if (pInstance == nullptr) {
            // Double-check if two threads would come here at the same time
            // Only one thread goes through, the others are waiting
            std::lock_guard<std::recursive_mutex> locker(getLock());

            if (pInstance == nullptr) {
                static LinkedInstance linkedInstance(getInstanceName());
                SharedMemoryLocker locker2(linkedInstance);

                pInstance = linkedInstance.get();

                if (pInstance == nullptr) {
                    static T instance;

                    pInstance = &instance;
                    linkedInstance.set(pInstance);

                    m_pDummyLock = nullptr; // To force the link on this variable
                }
            }
        }

        return *pInstance;
    }

    template <class T>
    std::recursive_mutex &Singleton<T>::getLock() {
        static std::recursive_mutex mtx;
        return mtx;
    }

    template <class T>
    const std::string &Singleton<T>::getInstanceName() {
        static const std::string instanceName = []() {
            std::stringstream ss;
            ss << std::string(typeid(Singleton<T>).name()) << " - " << getCurrentPID();

            return ss.str();
        }();

        return instanceName;
    }

    /*
     * \class LinkedInstance
     */
    template <class T>
    Singleton<T>::LinkedInstance::LinkedInstance(const std::string &name) : SharedMemory<T *>(name) {
        if (!SharedMemory<T *>::open()) {
            if (SharedMemory<T *>::create())
                SharedMemory<T *>::set((T *)nullptr);
        }
    }

} // namespace cho::osbase::core
