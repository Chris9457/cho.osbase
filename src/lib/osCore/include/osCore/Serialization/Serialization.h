// \file  Serialization.h
// \brief Main header for the osCore package

#pragma once

/**
 * \defgroup PACKAGE_SERIALIZATION Serialization
 *
 * \brief This package gathers classes that realizes generic serializations
 *
 * This package provides generic ways to serialize and deserialize data using a container organization such as the format Xml or Json
 *
 * \par Creating a Key
 * To create a key, use the function makeKey():
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 auto const myKey1 = makeKey("myKey"); // Return a KeyValue<std::string, void>
 auto const myKey2 = makeKey(L"myKey"); // Return a KeyValue<std::wstring, void>
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Creating a Key-value
 * To create a key-value, use the function makeKeyValue():
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 auto const myKeyValue1 = makeKeyValue("myKey", 123); // Return a KeyValue<std::string, int>
 auto const myKeyValue2 = makeKeyValue(L"myKey", MyStruct({...})); // Return a KeyValue<std::wstring, MyStruct>
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Getting the available key-stream families (concrete realization)
 * The available realizations are retrieved through the function getKeyStreamFamilies().
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 std::vector<std::string> keyStreamFamilies = getKeyStreamFamilies();
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Instantiating a concrete key-stream
 * A key-stream is mounted by invoking the function makeKeyStream():
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 auto pKeyStream1 = makeKeyStream("<family_name>"); // <family_name> is obtained with the function getKeyStreamFamilies()
 auto pKeyStream2 = makeKeyStream("<family_name>", std::ifstream("path_to_file")); // Provide also the input stream to read
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Key-serializing / deserializing a key-value
 * Key-values are serialized / deserialized by using the stream-operators.\n
 * Note: only the value associated to a key-stream strategy serialization can be used, see below to create key-stream strategy serialization
 * \n
 * Example of key-serializations:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 auto pKeyStream = makeKeyStream("<family_name>");
 ...
 const std::string myKeyInt = "my_key_int";
 const std::string myKeyFloat = "my_key_float";
 const std::string myKeyString = "my_key_string";
 const std::string myKeyArray = "my_key_array";

 *pKeyStream << makeKeyValue(myKeyInt, 55);								// serialize the integer 55
 *pKeyStream << makeKeyValue(myKeyFloat, 1.2f);							// serialize the float 1.2
 *pKeyStream << makeKeyValue(myKeyString, "toto");						// serialize the string "toto"
 *pKeyStream << makeKeyValue(myKeyArray, std::vector<int>({1, 2, 3}));	// serialize the array {1, 2, 3}
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * \n
 * Example of key-deserializations (using the previous example)
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 auto keyValueInt = makeKeyValue<std::stringint>(myKeyInt);
 auto keyValueFloat = makeKeyValue<float>(myKeyFloat);
 auto keyValueString = makeKeyValue<std::string>(myKeyString);
 auto keyValueArray = makeKeyValue<int[]>(myKeyArray);

 *pKeyStream >> keyValueInt;	// deserialize the int value, ie 55
 *pKeyStream >> keyValueFloat;	// deserialize the float value, ie 1.2
 *pKeyStream >> keyValueString;	// deserialize the string value, ie "toto"
 *pKeyStream >> keyValueArray;	// deserialize the array value, ie {1, 2, 3}
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Creating key-stream strategy serialization
 * As mentionned above, only value associated to a key-stream strategy serialization can be serialized / deserialized.\n
 * To create a strategy, the struct KeySerializer must be specialized.\n
 * \n
 * The following example illustrates the different steps to implement a custom key-serialization:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
        ...
        #include "osCore/Serialization/KeyStream.h"
        #include "osCoreImpl/CoreImpl.h" // for the link with the concrete realizations of key-streams
        using namespace NS_OSBASE;

        // Declaration of the custom data (struct)
        // Remark: MyCustomData contains MySubCustomData
        struct MySubCustomData {
            std::string strField;

            // useful to check the equality
            bool operator==(const MySubCustomData &other) const {
                return strField == other.strField;
            }
        };
        struct MyCustomData {
            int intField = 0;
            MySubCustomData subCustomDataField;

            // useful to check the equality
            bool operator==(const MyCustomData &other) const {
                return intField == other.intField && subCustomDataField == other.subCustomDataField;
            }
        };


        // Implementation of the key-stream serializer strategies
        // It consists for each custom data to implement the get/set Value
        template <>
        struct core::KeySerializer<std::string, MySubCustomData> {

            // Deserialization
            static MySubCustomData getValue(core::KeyStream<std::string> &stream, const MySubCustomData &defaultValue) {
                return MySubCustomData{ stream.getKeyValue("strFieldName", std::string("default_value")) };
            }

            // Serialization
            static bool setValue(core::KeyStream<std::string> &stream, const MySubCustomData &value) {
                return stream.setKeyValue("strFieldName", value.strField);
            }
        };

        template <>
        struct core::KeySerializer<std::string, MyCustomData> {

            // Deserialization
            static MyCustomData getValue(core::KeyStream<std::string> &stream, const MyCustomData &defaultValue) {
                return MyCustomData{ stream.getKeyValue("intFieldName", 0),
                    stream.getKeyValue("subCustomDataFieldName", MySubCustomData{ "default_value" }) };
            }

            // Serialization
            static bool setValue(core::KeyStream<std::string> &stream, const MyCustomData &value) {
                return stream.setKeyValue("intFieldName", value.intField) && stream.setKeyValue("subCustomDataFieldName",
                                                                                                value.subCustomDataField);
            }
        };


        // Some helper functions
        void serialize(const core::KeyStreamPtr<std::string> &pKeyStream, const std::string &key, const MyCustomData &myCustomData) {
            *pKeyStream << core::makeKeyValue(key, myCustomData);
        }

        MyCustomData deserialize(const core::KeyStreamPtr<std::string> &pKeyStream, const std::string &key) {
            auto keyValue = core::makeKeyValue<MyCustomData>(key);
            *pKeyStream >> keyValue;
            return keyValue.getValue();
        }

        OS_CORE_IMPL_LINK(); // mandatory to force the link with the concrete realizations of the key-streams

        int main() {
            const MyCustomData myCustomData{ 2, { "toto" } };

            // Loop with Xml & Json concrete realization
            for (auto &&keyStreamFamily : core::getKeyStreamFamilies())
            {
                auto const pKeyStream = core::makeKeyStream(keyStreamFamily);
                std::string keyName = "customData";

                serialize(pKeyStream, keyName, myCustomData);
                MyCustomData otherCustomData = deserialize(pKeyStream, keyName);

                if (!(otherCustomData == myCustomData))
                    throw std::exception();
            }
        }
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * \n
 * In this example, the main focus is on the implementations of the specialization KeySerializer.\n
 * This template class has 2 template parameters:
 *  -# type of the key ==> std string for each
 *  -# type of the value to serialize / deserialize ==> MySubCustomData & MyCustomData\n
 *  .
 * <b>Deserialization</b>\n
 * That consists to implement the method getValue(). In the above example, this method calls <em>stream.getKeyValue</em> for each
 * field of the custom data.\n
 * \n
 * <b>Serialization</b>\n
 * That consists to implement the method setValue(). In the above example, this method calls <em>stream.setKeyValue</em> for each
 * field of the custom data.\n
 * \n
 * <b>Recursive serialization / deserialization</b>\n
 * The construction implemented above shows that the recursivity on the serialization / deserialization is installed by the implementation
 * of each "sub-" custom structure, until reaching the fundamental types for which the serialization & deserialization is internally
 implemented
 * by the concrete realizations.
 *
 * \ingroup PACKAGE_OSCORE
 */

#include "Converters.h"
#include "CoreKeySerializer.h"
#include "KeySerializer.h"
#include "KeyStream.h"
#include "KeyValue.h"
#include "Serializer.h"
