// osBase package
//
// \file  osrapidxmlstream.cpp
// \brief Implementation of the class RapidXmlStream

#pragma warning(push)
#pragma warning(disable : 4100)

#include "RapidXmlStream.h"
#include "osCore/Serialization/FactoryNames.h"
#include "RapidJsonStream.h"
#include "RapidXmlPrint.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore/Misc/ScopeValue.h"
#include "osCore/Serialization/Converters.h"
#include <regex>

namespace cho::osbase::core::impl {

    OS_REGISTER_FACTORY_N(StringKeyStream, RapidXmlStream, 0, FACTORY_NAME_XML_STREAM)
    OS_REGISTER_FACTORY_N(StringKeyStream, RapidXmlStream, 1, FACTORY_NAME_XML_STREAM, std::string)

    namespace {
        constexpr const char *SUFFIX_ARRAY_TAG = "_array";
    }

    /*
     * \class RapidXmlStream
     */
    RapidXmlStream::RapidXmlStream() : RapidXmlStream(std::string()) {
    }

    RapidXmlStream::RapidXmlStream(const std::string &xmlContent) : m_xmlContent(xmlContent), m_pCurrentNode(nullptr) {
        if (!m_xmlContent.empty())
            m_document.parse<0>(m_xmlContent.data());

        m_pCurrentNode = &m_document;
    }

    bool RapidXmlStream::getValue(const bool &bDefaultValue) {
        const int defaultValue = bDefaultValue ? 1 : 0;
        const int value        = getValue(defaultValue);

        return value != 0;
    }

    bool RapidXmlStream::setValue(const bool &bValue) {
        std::stringstream ss;
        ss << std::noboolalpha << bValue;
        return setValue(ss.str());
    }

    int RapidXmlStream::getValue(const int &defaultValue) {
        char *value = getValue();
        if (value == nullptr)
            return defaultValue;

        return std::strtol(value, nullptr, 10);
    }

    bool RapidXmlStream::setValue(const int &value) {
        std::stringstream ss;
        ss << value;
        return setValue(ss.str());
    }

    double RapidXmlStream::getValue(const double &defaultValue) {
        char *value = getValue();
        if (value == nullptr)
            return defaultValue;

        return std::strtod(value, nullptr);
    }

    bool RapidXmlStream::setValue(const double &value) {
        std::stringstream ss;
        ss << value;
        return setValue(ss.str());
    }

    std::wstring RapidXmlStream::getValue(const std::wstring &strDefaultValue) {
        char *value = getValue();
        if (value == nullptr)
            return strDefaultValue;

        return type_cast<std::wstring>(std::string(value));
    }

    bool RapidXmlStream::setValue(const std::wstring &strValue) {
        return setValue(type_cast<std::string>(strValue));
    }

    KeyValue<std::string, void> RapidXmlStream::createKey(const std::string &key) {
        return openKey(key, true);
    }

    KeyValue<std::string, void> RapidXmlStream::openKey(const std::string &key) {
        return openKey(key, false);
    }

    KeyValue<std::string, void> RapidXmlStream::getCurrentKey() {
        if (m_pCurrentNode == nullptr)
            return KeyValue<std::string, void>::null();

        return makeKey(std::string(m_pCurrentNode->name()));
    }

    bool RapidXmlStream::isKeyExist(const std::string &key) const {
        if (m_pCurrentNode == nullptr)
            return false;

        return m_pCurrentNode->first_node(key.c_str()) != nullptr;
    }

    KeyValue<std::string, void> RapidXmlStream::firstKey(const KeyValue<std::string, void> &key) {
        return openKey(key.getKey());
    }

    KeyValue<std::string, void> RapidXmlStream::nextKey(const KeyValue<std::string, void> &key) {
        if (m_pCurrentNode == nullptr)
            return KeyValue<std::string, void>::null();

        auto const &keyName  = key.getKey();
        auto const pNextNode = m_pCurrentNode->next_sibling(keyName.empty() ? nullptr : keyName.c_str());
        if (pNextNode == nullptr)
            return KeyValue<std::string, void>::null();

        m_pCurrentNode = pNextNode;
        return makeKey(std::string(m_pCurrentNode->name()));
    }

    bool RapidXmlStream::closeKey() {
        if (m_pCurrentNode == nullptr || m_pCurrentNode == &m_document)
            return false;

        m_pCurrentNode = m_pCurrentNode->parent();
        if (m_pCurrentNode == nullptr)
            m_pCurrentNode = &m_document;

        if (isArrayNode(m_pCurrentNode))
            closeKey();

        return true;
    }

    void RapidXmlStream::rewind() {
        m_pCurrentNode = &m_document;
    }

    std::vector<unsigned char> RapidXmlStream::getBuffer() const {
        std::vector<unsigned char> buffer;
        rapidxml::print(std::back_inserter(buffer), m_document, 0);
        buffer.push_back(0);
        return buffer;
    }

    KeyValue<std::string, void> RapidXmlStream::createArray(const std::string &key) {
        return createKey(key + SUFFIX_ARRAY_TAG);
    }

    KeyValue<std::string, void> RapidXmlStream::openArray(const std::string &key) {
        return openKey(key + SUFFIX_ARRAY_TAG);
    }

    KeyValue<std::string, void> RapidXmlStream::createRow() {
        auto const pParentArrayNode = isArrayNode(m_pCurrentNode) ? m_pCurrentNode : getParentArrayNode();
        if (pParentArrayNode == nullptr)
            return KeyValue<std::string, void>::null();

        const std::string parentArrayNodeName = pParentArrayNode->name();
        const char *nodeName =
            m_document.allocate_string(parentArrayNodeName.substr(0, parentArrayNodeName.rfind(SUFFIX_ARRAY_TAG)).c_str());
        auto const pChildNode = m_document.allocate_node(rapidxml::node_element, nodeName);
        pParentArrayNode->append_node(pChildNode);
        m_pCurrentNode = pChildNode;

        return getCurrentKey();
    }

    KeyValue<std::string, void> RapidXmlStream::openRow(const size_t index) {
        static auto const nullKey = KeyValue<std::string, void>::null();

        auto const pParentArrayNode = isArrayNode(m_pCurrentNode) ? m_pCurrentNode : getParentArrayNode();
        if (pParentArrayNode == nullptr)
            return nullKey;

        auto sv             = make_scope_value(m_pCurrentNode, pParentArrayNode);
        size_t currentIndex = 0;
        auto arrayKey       = firstKey(nullKey);
        while (currentIndex != index && !arrayKey.isNull()) {
            ++currentIndex;
            arrayKey = nextKey(nullKey);
        }

        if (arrayKey.isNull())
            return nullKey;

        sv.release();
        return getCurrentKey();
    }

    bool RapidXmlStream::resetArray() {
        auto const pParentArrayNode = isArrayNode(m_pCurrentNode) ? m_pCurrentNode : getParentArrayNode();
        if (pParentArrayNode == nullptr)
            return false;

        pParentArrayNode->remove_all_nodes();
        return true;
    }

    size_t RapidXmlStream::size() const {
        if (!isArrayNode(m_pCurrentNode))
            return 0;

        size_t arraySize = 0;
        for (auto pCurrentNode = m_pCurrentNode->first_node(); pCurrentNode != nullptr; pCurrentNode = pCurrentNode->next_sibling())
            ++arraySize;
        return arraySize;
    }

    char *RapidXmlStream::getValue() const {
        if (m_pCurrentNode != nullptr)
            return m_pCurrentNode->value();

        return nullptr;
    }

    bool RapidXmlStream::setValue(const std::string &value) {
        if (m_pCurrentNode == nullptr)
            return false;

        const char *nodeValue = m_document.allocate_string(value.c_str());
        m_pCurrentNode->value(nodeValue);

        return true;
    }

    KeyValue<std::string, void> RapidXmlStream::openKey(const std::string &key, bool bCreate) {
        if (m_pCurrentNode == nullptr)
            return KeyValue<std::string, void>::null();

        auto pChildNode = m_pCurrentNode->first_node(key.empty() ? nullptr : key.c_str());
        if (pChildNode == nullptr) {
            if (!bCreate)
                return KeyValue<std::string, void>::null();

            const char *nodeName = m_document.allocate_string(key.c_str());
            pChildNode           = m_document.allocate_node(rapidxml::node_element, nodeName);
            m_pCurrentNode->append_node(pChildNode);
        }

        m_pCurrentNode = pChildNode;
        return makeKey(key);
    }

    rapidxml::xml_node<char> *RapidXmlStream::getParentArrayNode() const {
        if (m_pCurrentNode == nullptr)
            return nullptr;

        auto const pParentNode = m_pCurrentNode->parent();
        if (pParentNode == nullptr)
            return nullptr;

        return isArrayNode(pParentNode) ? pParentNode : nullptr;
    }

    bool RapidXmlStream::isArrayNode(rapidxml::xml_node<char> *pNode) {
        if (pNode == nullptr)
            return false;

        // if tag name ends with suffix array
        const std::string nodeName = pNode->name();
        auto const pos             = nodeName.rfind(SUFFIX_ARRAY_TAG);
        if (pos >= nodeName.size())
            return false;

        return nodeName.substr(nodeName.rfind(SUFFIX_ARRAY_TAG)) == SUFFIX_ARRAY_TAG;
    }

} // namespace cho::osbase::core::impl

#pragma warning(pop)
