// osBase package
//
// \file  osrapidjsonstream.cpp
// \brief Implementation of the class RapidJsonStream

#include "RapidJsonStream.h"
#include "osCore/Serialization/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore/Serialization/Converters.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <fstream>

namespace NS_OSBASE::core::impl {

    OS_REGISTER_FACTORY_N(StringKeyStream, RapidJsonStream, 0, FACTORY_NAME_JSON_STREAM)
    OS_REGISTER_FACTORY_N(StringKeyStream, RapidJsonStream, 1, FACTORY_NAME_JSON_STREAM, std::string)

    /*
     * \class RapidJsonStream
     */
    RapidJsonStream::RapidJsonStream() = default;

    RapidJsonStream::RapidJsonStream(const std::string &jsonContent) : m_jsonContent(jsonContent) {
        m_document.Parse(m_jsonContent.c_str());
    }

    bool RapidJsonStream::getValue(const bool &bDefaultValue) {
        auto const pCurrentValue = getCurrentValue();
        return pCurrentValue != nullptr && pCurrentValue->Is<bool>() ? pCurrentValue->Get<bool>() : bDefaultValue;
    }

    int RapidJsonStream::getValue(const int &defaultValue) {
        auto const pCurrentValue = getCurrentValue();
        return pCurrentValue != nullptr && pCurrentValue->Is<int>() ? pCurrentValue->Get<int>() : defaultValue;
    }

    double RapidJsonStream::getValue(const double &defaultValue) {
        auto const pCurrentValue = getCurrentValue();
        return pCurrentValue != nullptr && (pCurrentValue->Is<double>() || pCurrentValue->Is<int>()) ? pCurrentValue->Get<double>()
                                                                                                     : defaultValue;
    }

    std::wstring RapidJsonStream::getValue(const std::wstring &strDefaultValue) {
        auto const pCurrentValue = getCurrentValue();
        return pCurrentValue != nullptr && pCurrentValue->IsString() ? type_cast<std::wstring>(std::string(pCurrentValue->GetString()))
                                                                     : strDefaultValue;
    }

    bool RapidJsonStream::setValue(const bool &bValue) {
        auto const pCurrentValue = getCurrentValue();
        if (pCurrentValue == nullptr || !pCurrentValue->Is<bool>() && !pCurrentValue->IsNull())
            return false;

        pCurrentValue->Set<bool>(bValue);
        return true;
    }

    bool RapidJsonStream::setValue(const int &value) {
        auto const pCurrentValue = getCurrentValue();
        if (pCurrentValue == nullptr || !pCurrentValue->Is<int>() && !pCurrentValue->IsNull())
            return false;

        pCurrentValue->Set<int>(value);
        return true;
    }

    bool RapidJsonStream::setValue(const double &value) {
        auto const pCurrentValue = getCurrentValue();
        if (pCurrentValue == nullptr || !pCurrentValue->Is<double>() && !pCurrentValue->IsNull())
            return false;

        pCurrentValue->Set<double>(value);
        return true;
    }

    bool RapidJsonStream::setValue(const std::wstring &wStrValue) {
        auto const pCurrentValue = getCurrentValue();
        if (pCurrentValue == nullptr || !pCurrentValue->IsString() && !pCurrentValue->IsNull())
            return false;

        pCurrentValue->SetString(rapidjson::StringRef(makeExistingString(type_cast<std::string>(wStrValue)).c_str()));
        return true;
    }

    KeyValue<std::string, void> RapidJsonStream::createKey(const std::string &key) {
        auto keyValue = openKey(key);
        if (!keyValue.isNull())
            return keyValue;

        auto const pCurrentValue = getCurrentValue();
        if (pCurrentValue == nullptr)
            return KeyValue<std::string, void>::null();

        if (pCurrentValue->IsNull())
            pCurrentValue->SetObject();
        else if (!pCurrentValue->IsObject())
            return KeyValue<std::string, void>::null();

        rapidjson::Value subValue;
        pCurrentValue->AddMember(rapidjson::StringRef(makeExistingString(key).c_str()), subValue, m_document.GetAllocator());
        pushKey(key);
        return makeKey(key);
    }

    KeyValue<std::string, void> RapidJsonStream::openKey(const std::string &key) {
        if (key.empty()) {
            auto const pCurrentValue = getCurrentValue();
            if (pCurrentValue == nullptr || !pCurrentValue->IsObject())
                return KeyValue<std::string, void>::null();

            auto const itFirstMember = pCurrentValue->MemberBegin();
            if (itFirstMember == pCurrentValue->MemberEnd())
                return KeyValue<std::string, void>::null();

            pushKey(itFirstMember->name.GetString());
        } else
            pushKey(key);

        if (getCurrentValue() != nullptr)
            return makeKey(key);

        m_keys.pop_back();
        return KeyValue<std::string, void>::null();
    }

    KeyValue<std::string, void> RapidJsonStream::getCurrentKey() {
        if (m_keys.empty())
            return makeKey(std::string());

        return makeKey(m_keys.back().first);
    }

    bool RapidJsonStream::isKeyExist(const std::string &key) const {
        keys_type tempKeys = m_keys;
        tempKeys.push_back(std::make_pair(key, -1));
        auto &self = const_cast<RapidJsonStream &>(*this);
        return self.getCurrentValue(tempKeys) != nullptr;
    }

    KeyValue<std::string, void> RapidJsonStream::firstKey(const KeyValue<std::string, void> &key) {
        auto const pCurrentValue = getCurrentValue(false);
        if (pCurrentValue == nullptr)
            return KeyValue<std::string, void>::null();

        auto const &keyName = key.getKey();
        if (keyName.empty() && pCurrentValue->IsArray()) {
            m_keys.back().second = 0;
            if (getCurrentValue() != nullptr) {
                return getCurrentKey();
            }
            return KeyValue<std::string, void>::null();
        }

        return openKey(key.getKey());
    }

    KeyValue<std::string, void> RapidJsonStream::nextKey(const KeyValue<std::string, void> &key) {
        auto const &keyName = key.getKey();
        if (!m_keys.back().first.empty() && m_keys.back().first == keyName)
            return KeyValue<std::string, void>::null();

        if (keyName.empty()) {
            auto pCurrentValue = getCurrentValue(false);
            if (pCurrentValue == nullptr)
                return KeyValue<std::string, void>::null();

            if (pCurrentValue->IsArray()) {
                if (static_cast<size_t>(m_keys.back().second) + 1 >= size())
                    return KeyValue<std::string, void>::null();

                ++m_keys.back().second;
                return getCurrentKey();
            }

            auto const lastKey = m_keys.back().first;
            m_keys.pop_back();
            pCurrentValue = getCurrentValue(false);
            auto itMember = pCurrentValue->FindMember(lastKey.c_str());
            if (itMember == pCurrentValue->MemberEnd())
                return KeyValue<std::string, void>::null();

            ++itMember;
            if (itMember == pCurrentValue->MemberEnd())
                return KeyValue<std::string, void>::null();

            pushKey(itMember->name.GetString());
            return makeKey(keyName);
        }

        pushKey(keyName);

        if (getCurrentValue() == nullptr)
            return KeyValue<std::string, void>::null();

        return makeKey(keyName);
    }

    bool RapidJsonStream::closeKey() {
        if (m_keys.empty())
            return false;

        m_keys.pop_back();
        return true;
    }

    void RapidJsonStream::rewind() {
        m_keys.clear();
    }

    std::vector<unsigned char> RapidJsonStream::getBuffer() const {
        rapidjson::StringBuffer buffer;
        buffer.Clear();
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        m_document.Accept(writer);

        auto const pStr   = buffer.GetString();
        auto const length = strlen(pStr);
        return { pStr, pStr + length + 1 };
    }

    KeyValue<std::string, void> RapidJsonStream::createArray(const std::string &key) {
        if (key.empty()) {
            auto keyValue = openKey(key);
            if (!keyValue.isNull())
                return keyValue;

            auto const pCurrentValue = getCurrentValue();
            if (pCurrentValue == nullptr)
                return KeyValue<std::string, void>::null();

            if (pCurrentValue->IsNull())
                pCurrentValue->SetArray();
            else if (!pCurrentValue->IsObject())
                return KeyValue<std::string, void>::null();

            pushKey(key);
            m_keys.back().second = 0;
            return makeKey(key);
        }

        auto result = createKey(key);
        if (result.isNull())
            return result;

        const auto pCurrentValue = getCurrentValue(false);
        if (pCurrentValue == nullptr || !pCurrentValue->IsArray() && !pCurrentValue->IsNull())
            return KeyValue<std::string, void>::null();

        if (pCurrentValue->IsNull())
            *pCurrentValue = rapidjson::Value(rapidjson::kArrayType);

        m_keys.back().second = 0;
        return result;
    }

    KeyValue<std::string, void> RapidJsonStream::openArray(const std::string &key) {
        if (key.empty()) {
            auto const pCurrentValue = getCurrentValue();
            if (pCurrentValue == nullptr || !pCurrentValue->IsArray())
                return KeyValue<std::string, void>::null();

            pushKey(key);
            m_keys.back().second = 0;
            return makeKey(key);
        }

        auto result = openKey(key);
        if (result.isNull())
            return result;

        auto const pCurrentValue = getCurrentValue(false);
        if (!pCurrentValue->IsArray()) {
            closeKey();
            return KeyValue<std::string, void>::null();
        }

        m_keys.back().second = 0;
        return result;
    }

    KeyValue<std::string, void> RapidJsonStream::createRow() {
        auto const pCurrentValue = getCurrentValue(false);
        if (pCurrentValue == nullptr || !pCurrentValue->IsArray())
            return KeyValue<std::string, void>::null();

        pCurrentValue->PushBack(rapidjson::Value(), m_document.GetAllocator());
        m_keys.back().second = static_cast<int>(size()) - 1;
        return getCurrentKey();
    }

    KeyValue<std::string, void> RapidJsonStream::openRow(const size_t index) {
        auto const previousIndex = m_keys.back().second;
        m_keys.back().second     = static_cast<int>(index);

        if (getCurrentValue() == nullptr) {
            m_keys.back().second = previousIndex;
            return KeyValue<std::string, void>::null();
        }

        return getCurrentKey();
    }

    bool RapidJsonStream::resetArray() {
        auto const pCurrentValue = getCurrentValue(false);
        if (pCurrentValue == nullptr || !pCurrentValue->IsArray())
            return false;

        pCurrentValue->Clear();
        m_keys.back().second = 0;
        return true;
    }

    size_t RapidJsonStream::size() const {
        auto &self               = const_cast<RapidJsonStream &>(*this);
        auto const pCurrentValue = self.getCurrentValue(false);
        if (pCurrentValue == nullptr || !pCurrentValue->IsArray() || pCurrentValue->IsNull())
            return 0;

        return static_cast<size_t>(pCurrentValue->Size());
    }

    rapidjson::Value *RapidJsonStream::getCurrentValue(const bool bWithIndex) {
        return getCurrentValue(m_keys, bWithIndex);
    }

    rapidjson::Value *RapidJsonStream::getCurrentValue(const keys_type &keys, const bool bWithIndex) {
        if (keys.empty())
            return &m_document;

        std::string path;
        for (auto itKey = keys.cbegin(); itKey != keys.cend(); ++itKey) {
            if (!itKey->first.empty())
                path += "/" + itKey->first;
            if ((itKey != --keys.cend() || bWithIndex) && itKey->second != -1)
                path += "/" + std::to_string(itKey->second);
        }

        return rapidjson::Pointer(path.c_str()).Get(m_document);
    }

    const std::string &RapidJsonStream::makeExistingString(const std::string &str) {
        return *m_stringKeepAlive.insert(str).first;
    }

    void RapidJsonStream::pushKey(const std::string &key) {
        m_keys.push_back(std::make_pair(key, -1));
    }

} // namespace NS_OSBASE::core::impl
