// \file  osrapidjsonstream.h
// \brief Declaration of the class RapidJsonStream

#pragma once
#include "osCore/Serialization/KeyStream.h"
#include "rapidjson/document.h"
#include <set>
#include <stack>

namespace NS_OSBASE::core::impl {

    /**
     * \brief 	This class represents:
     *				- the concrete implementation of the class KeyStream<std::string>
     */
    class RapidJsonStream final : public KeyStream<std::string> {
    public:
        RapidJsonStream();
        RapidJsonStream(const std::string &jsonContent);

        bool getValue(const bool &bDefaultValue) override;
        bool setValue(const bool &bValue) override;

        int getValue(const int &defaultValue) override;
        bool setValue(const int &value) override;

        double getValue(const double &defaultValue) override;
        bool setValue(const double &value) override;

        std::wstring getValue(const std::wstring &strDefaultValue) override;
        bool setValue(const std::wstring &wStrValue) override;

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
        using keys_type = std::deque<std::pair<std::string, int>>;

        rapidjson::Value *getCurrentValue(const bool bWithIndex = true);
        rapidjson::Value *getCurrentValue(const keys_type &keys, const bool bWithIndex = true);
        const std::string &makeExistingString(const std::string &str);
        void pushKey(const std::string &key);

        rapidjson::Document m_document;
        keys_type m_keys;
        std::string m_jsonContent;
        std::set<std::string> m_stringKeepAlive;
    };

} // namespace NS_OSBASE::core::impl
