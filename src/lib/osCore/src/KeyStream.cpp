// \file  LeyStream.cpp
// \brief Implementation of the class KeyStream

#include "osCore/Serialization/KeyStream.h"
#include "osCore/Serialization/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace cho::osbase::core {
    std::vector<std::string> getKeyStreamFamilies() {
        return TheFactoryManager.getFactoryNames<KeyStream<std::string>>();
    }

    KeyStreamPtr<std::string> makeKeyStream(const std::string &factoryName) {
        return TheFactoryManager.createInstance<KeyStream<std::string>>(factoryName);
    }

    KeyStreamPtr<std::string> makeKeyStream(const std::string &factoryName, std::istream &is) {
        const std::istream_iterator<char> itIs(is);
        const std::istream_iterator<char> itEnd;

        is.unsetf(std::ios_base::skipws);
        std::string xmlContent(itIs, itEnd);
        return TheFactoryManager.createInstance<KeyStream<std::string>>(factoryName, xmlContent);
    }

    KeyStreamPtr<std::string> makeKeyStream(const std::string &factoryName, std::istream &&is) {
        return makeKeyStream(factoryName, is);
    }

    // xml
    KeyStreamPtr<std::string> makeXmlStream() {
        return makeKeyStream(FACTORY_NAME_XML_STREAM);
    }

    KeyStreamPtr<std::string> makeXmlStream(std::istream &is) {
        return makeKeyStream(FACTORY_NAME_XML_STREAM, is);
    }

    KeyStreamPtr<std::string> makeXmlStream(std::istream &&is) {
        return makeXmlStream(is);
    }

    // json
    KeyStreamPtr<std::string> makeJsonStream() {
        return makeKeyStream(FACTORY_NAME_JSON_STREAM);
    }

    KeyStreamPtr<std::string> makeJsonStream(std::istream &is) {
        return makeKeyStream(FACTORY_NAME_JSON_STREAM, is);
    }

    KeyStreamPtr<std::string> makeJsonStream(std::istream &&is) {
        return makeJsonStream(is);
    }
    /*
     * stream operators
     */
    std::ostream &operator<<(std::ostream &os, const KeyStream<std::string> &keyStream) {
        return os << reinterpret_cast<const char *>(keyStream.getBuffer().data());
    }

} // namespace cho::osbase::core
