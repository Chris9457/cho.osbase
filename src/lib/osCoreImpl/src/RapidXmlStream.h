// \file  osrapidxmlstream.h
// \brief Declaration of the class RapidXmlStream

#pragma once
#include "osCore/Serialization/KeyStream.h"
#include "rapidxml/rapidxml.hpp"
#include <memory.h>
#include <string>

namespace cho::osbase::core::impl {

    /**
     * \brief 	This class represents:
     *				- the concrete implementation of the class KeyStream<std::string>
     */
    class RapidXmlStream final : public KeyStream<std::string> {
    public:
        RapidXmlStream();
        RapidXmlStream(const std::string &xmlContent);

        bool getValue(const bool &bDefaultValue) override;
        bool setValue(const bool &bValue) override;

        int getValue(const int &defaultValue) override;
        bool setValue(const int &value) override;

        double getValue(const double &defaultValue) override;
        bool setValue(const double &value) override;

        std::wstring getValue(const std::wstring &strDefaultValue) override;
        bool setValue(const std::wstring &strValue) override;

        KeyValue<std::string, void> createKey(const std::string &key) override;
        KeyValue<std::string, void> openKey(const std::string &key) override;
        KeyValue<std::string, void> getCurrentKey() override;
        bool isKeyExist(const std::string &key) const override;
        KeyValue<std::string, void> firstKey(const KeyValue<std::string, void> &key) override;
        KeyValue<std::string, void> nextKey(const KeyValue<std::string, void> &key) override;
        bool closeKey() override;

        void rewind() override;

        KeyValue<std::string, void> createArray(const std::string &key) override;
        KeyValue<std::string, void> openArray(const std::string &key) override;
        KeyValue<std::string, void> createRow() override;
        KeyValue<std::string, void> openRow(const size_t index) override;
        bool resetArray() override;
        size_t size() const override;

        std::vector<unsigned char> getBuffer() const override;

    private:
        char *getValue() const;
        bool setValue(const std::string &value);
        KeyValue<std::string, void> openKey(const std::string &key, bool bCreate);
        rapidxml::xml_node<char> *getParentArrayNode() const;
        static bool isArrayNode(rapidxml::xml_node<char> *pNode);

        std::string m_xmlContent;
        rapidxml::xml_document<char> m_document;
        rapidxml::xml_node<char> *m_pCurrentNode;
    };

} // namespace cho::osbase::core::impl
