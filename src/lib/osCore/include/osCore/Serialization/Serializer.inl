// \brief Declaration of the class Serializer

#pragma once
#include "Serializable.h"

namespace cho::osbase::core {

    /*
     * \class Serializer<TValue>
     */
    template <typename TValue>
    TValue Serializer<TValue>::getValue(std::istream &is) {
        std::streamsize size = 0;
        if (!is.read(reinterpret_cast<char *>(&size), sizeof(size))) {
            throw SerializerException("Unable to read the size!");
        }

        std::vector<char> buffer(size);
        TValue value{};

        size_t pos = 0;
        if (!is.read(buffer.data(), size) || !Serializable<TValue>::read(value, buffer, pos)) {
            throw SerializerException("Unable to deserialize the stream");
        }
        return value;
    }

    template <typename TValue>
    void Serializer<TValue>::setValue(std::ostream &os, const TValue &value) {
        std::vector<char> buffer;

        if (!Serializable<TValue>::write(value, buffer)) {
            throw SerializerException("Unable to serialize the value");
        }

        auto const size = static_cast<std::streamsize>(buffer.size());
        os.write(reinterpret_cast<const char *>(&size), sizeof(size));
        os.write(buffer.data(), size);
        if (!os.good()) {
            throw SerializerException("Unable to write the stream!");
        }
    }

    /*
     * stream operator
     */
    template <typename TValue>
    std::istream &operator>>(std::istream &is, BinaryValue<TValue> &value) {
        value.set(Serializer<TValue>::getValue(is));
        return is;
    }

    template <typename TValue>
    std::ostream &operator<<(std::ostream &os, const BinaryValue<TValue> &value) {
        Serializer<TValue>::setValue(os, value.get());
        return os;
    }
} // namespace cho::osbase::core
