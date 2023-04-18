// \file  SharedMemory.inl
// \brief Implementation of the class SharedMemory

namespace cho::osbase::core {

    /*
     * \class SharedMemory
     */
    template <typename T>
    SharedMemory<T, false>::SharedMemory(const std::string &name) : SharedMemory<void, true>(name, sizeof(T)) {
    }

    template <typename T>
    T SharedMemory<T, false>::get() const {
        T value;
        getBuffer(&value);
        return value;
    }

    template <typename T>
    void SharedMemory<T, false>::set(const T &value) {
        setBuffer(&value);
    }

} // namespace cho::osbase::core
