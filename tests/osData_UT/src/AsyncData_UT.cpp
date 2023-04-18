// \brief Unit tests of the class ExchangedData

#include "osData/AsyncData.h"
#include "../../osCore_UT/include/osCore_UT/ConstantValue.h"
#include "gtest/gtest.h"

using namespace std::chrono_literals;

OS_SERIALIZE_STRUCT(cho::osbase::core::ut::TestStruct, st, i, d, v)
OS_SERIALIZE_STRUCT(cho::osbase::core::ut::ComplexTestStruct, d, t, e, s)

namespace cho::osbase::data::ut {

    class AsyncData_UT : public testing::Test {
    protected:
        static constexpr std::chrono::milliseconds getTimeout(unsigned int timeout) {
            return std::chrono::milliseconds(timeout * TIMEOUT_FACTOR);
        }
    };

    template <typename T>
    class AsyncDataTyped_UT : public AsyncData_UT {};

    using value_types = testing::Types<bool,
        char,
        short,
        unsigned short,
        int,
        float,
        double,
        std::string,
        std::wstring,
        std::vector<int>,
        // core::ut::TestEnum,
        core::ut::TestStruct,
        core::ut::ComplexTestStruct>;
    TYPED_TEST_SUITE(AsyncDataTyped_UT, value_types);

    TEST_F(AsyncData_UT, ctor) {
        { // default ctor
            AsyncData<bool> endpoint;
            ASSERT_FALSE(endpoint.isValid());
        }

        { // ctor creator
            Uri uri;
            AsyncData<bool> endpoint(uri);
            ASSERT_TRUE(endpoint.isValid());
            ASSERT_TRUE(uri.isValid());
        }

        { // ctor endpoint for connection
            Uri uri;
            AsyncData<bool> creator(uri);
            auto const endpointUri = uri;
            AsyncData<bool> endpoint(endpointUri);
            ASSERT_TRUE(endpoint.isValid());
        }

        {
            // copy ctor
            Uri uri;
            const AsyncData<bool> creator(uri);
            AsyncData<bool> otherCreator(creator);

            ASSERT_TRUE(creator.isValid());
            ASSERT_TRUE(otherCreator.isValid());
        }

        {
            // move ctor
            Uri uri;
            AsyncData<bool> creator(uri);
            AsyncData<bool> otherCreator(std::move(creator));

            ASSERT_FALSE(creator.isValid());
            ASSERT_TRUE(otherCreator.isValid());
        }
    }

    TEST_F(AsyncData_UT, makers) {
        { // endpoint creation
            auto const endpoint = makeAsyncData<bool>();
            ASSERT_TRUE(endpoint.isValid());
            ASSERT_TRUE(endpoint.getUriOfCreator().isValid());
        }

        { // endpoint for connection
            auto const creator  = makeAsyncData<bool>();
            auto const endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
            ASSERT_TRUE(endpoint.getUriOfCreator().isValid());
        }
    }

    TEST_F(AsyncData_UT, copyAssignment) {
        auto const creator = makeAsyncData<bool>();
        auto otherCreator  = makeAsyncData<bool>();

        otherCreator = creator;
        ASSERT_TRUE(creator.isValid());
        ASSERT_TRUE(otherCreator.isValid());
    }

    TEST_F(AsyncData_UT, moveAssignment) {
        auto creator      = makeAsyncData<bool>();
        auto otherCreator = makeAsyncData<bool>();

        otherCreator = std::move(creator);
        ASSERT_FALSE(creator.isValid());
        ASSERT_TRUE(otherCreator.isValid());
    }

    TEST_F(AsyncData_UT, isValid) {
        auto creator = makeAsyncData<bool>();
        ASSERT_TRUE(creator.isValid());

        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        ASSERT_TRUE(endpoint.isValid());

        AsyncData<bool> otherEndpoint(std::move(endpoint));
        ASSERT_TRUE(otherEndpoint.isValid());
        ASSERT_FALSE(endpoint.isValid());

        auto otherCreator = makeAsyncData<bool>();
        otherCreator      = std::move(creator);
        ASSERT_TRUE(otherCreator.isValid());
        ASSERT_FALSE(creator.isValid());
    }

    TEST_F(AsyncData_UT, isConnected) {
        auto creator = makeAsyncData<bool>();
        ASSERT_FALSE(creator.isConnected());

        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();
        ASSERT_TRUE(endpoint.isConnected());

        creator.waitConnection();
        ASSERT_TRUE(creator.isConnected());
    }

    TEST_F(AsyncData_UT, waitConnection) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());

        ASSERT_THROW(endpoint.waitConnection(), DataExchangeException); // verify not available for endpoint

        endpoint.connect();
        ASSERT_NO_THROW(creator.waitConnection());
    }

    TEST_F(AsyncData_UT, waitConnectionWithCallback) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());

        std::promise<bool> promiseConnected;
        auto futConnected = promiseConnected.get_future();

        auto onConnected = [&promiseConnected](bool bConnected) { promiseConnected.set_value(bConnected); };

        ASSERT_THROW(endpoint.waitConnection(onConnected), DataExchangeException); // verify not available for endpoint
        ASSERT_THROW(creator.waitConnection(data::AsyncData<bool>::TCallbackConnection()), DataExchangeException); // verify valid callback
        ASSERT_THROW(data::AsyncData<bool>().waitConnection(onConnected), DataExchangeException);                  // verify endpoint valid

        // check connection
        ASSERT_NO_THROW(creator.waitConnection(onConnected));
        endpoint.connect();
        ASSERT_TRUE(futConnected.get());

        // check disconnection
        std::promise<bool>().swap(promiseConnected);
        futConnected = promiseConnected.get_future();
        endpoint.reset();
        ASSERT_FALSE(futConnected.get());
    }

    TEST_F(AsyncData_UT, waitConnectionFor) {
        auto creator = makeAsyncData<bool>();
        ASSERT_FALSE(creator.waitConnectionFor(getTimeout(100)));

        auto endpoint          = makeAsyncData<bool>(creator.getUriOfCreator());
        auto waitConnectionFor = [&endpoint](const std::chrono::milliseconds &timeout) { return endpoint.waitConnectionFor(timeout); };
        ASSERT_THROW(waitConnectionFor(getTimeout(100)), DataExchangeException);

        endpoint.connect();
        ASSERT_TRUE(creator.waitConnectionFor(1s));
    }

    TEST_F(AsyncData_UT, waitConnectionUntil) {
        auto getTimeEndFromNow = [](const std::chrono::milliseconds &ellapseTime) {
            return std::chrono::steady_clock::now() + ellapseTime;
        };

        auto creator = makeAsyncData<bool>();
        ASSERT_FALSE(creator.waitConnectionUntil(getTimeEndFromNow(getTimeout(100))));

        auto endpoint            = makeAsyncData<bool>(creator.getUriOfCreator());
        auto waitConnectionUntil = [&endpoint](const std::chrono::time_point<std::chrono::steady_clock> &timePoint) {
            return endpoint.waitConnectionUntil(timePoint);
        };
        ASSERT_THROW(waitConnectionUntil(getTimeEndFromNow(getTimeout(100))), DataExchangeException);

        endpoint.connect();
        ASSERT_TRUE(creator.waitConnectionUntil(getTimeEndFromNow(1s)));
    }

    TEST_F(AsyncData_UT, connect) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());

        ASSERT_THROW(creator.connect(), DataExchangeException); // verify not available for creator
        ASSERT_NO_THROW(endpoint.connect());
    }

    TEST_F(AsyncData_UT, connectWithCallback) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());

        std::promise<bool> promiseConnected;
        auto futConnected = promiseConnected.get_future();

        auto onConnected = [&promiseConnected](bool bConnected) { promiseConnected.set_value(bConnected); };

        ASSERT_THROW(creator.connect(onConnected), DataExchangeException); // verify not available for creator
        ASSERT_THROW(endpoint.connect(data::AsyncData<bool>::TCallbackConnection()), DataExchangeException); // verify valid callback
        ASSERT_THROW(data::AsyncData<bool>().connect(onConnected), DataExchangeException);                   // verify endpoint valid

        // check connection
        ASSERT_NO_THROW(endpoint.connect(onConnected));
        ASSERT_TRUE(futConnected.get());

        // check disconnection
        std::promise<bool>().swap(promiseConnected);
        futConnected = promiseConnected.get_future();
        creator.reset();
        ASSERT_FALSE(futConnected.get());
    }

    TEST_F(AsyncData_UT, tryConnectFor) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());

        ASSERT_TRUE(endpoint.tryConnectFor(getTimeout(100)));
    }

    TEST_F(AsyncData_UT, tryConnectUntil) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());

        auto const endTime = std::chrono::steady_clock::now() + getTimeout(100);
        ASSERT_TRUE(endpoint.tryConnectUntil(endTime));
    }

    TEST_F(AsyncData_UT, waitDisconnection) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();

        ASSERT_TRUE(endpoint.isConnected());
        auto const futCreatorDisconnected = std::async([&creator]() { creator.waitDisconnection(); });
        endpoint.reset();
        ASSERT_EQ(std::future_status::ready, futCreatorDisconnected.wait_for(getTimeout(100)));

        endpoint.connect();
        ASSERT_TRUE(endpoint.isConnected());

        creator.reset();
        auto const futEndPointDisconnected = std::async([&endpoint]() { endpoint.waitDisconnection(); });
        ASSERT_EQ(std::future_status::ready, futEndPointDisconnected.wait_for(getTimeout(100)));
    }

    TEST_F(AsyncData_UT, waitDisconnectionFor) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();

        ASSERT_TRUE(endpoint.isConnected());
        endpoint.reset();
        ASSERT_TRUE(creator.waitDisconnectionFor(getTimeout(100)));

        endpoint.connect();
        ASSERT_TRUE(endpoint.isConnected());

        creator.reset();
        ASSERT_TRUE(endpoint.waitDisconnectionFor(getTimeout(100)));
    }

    TEST_F(AsyncData_UT, waitDisconnectionUntil) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();

        ASSERT_TRUE(endpoint.isConnected());
        endpoint.reset();
        ASSERT_TRUE(creator.waitDisconnectionUntil(std::chrono::steady_clock::now() + getTimeout(100)));

        endpoint.connect();
        ASSERT_TRUE(endpoint.isConnected());

        creator.reset();
        ASSERT_TRUE(endpoint.waitDisconnectionUntil(std::chrono::steady_clock::now() + getTimeout(100)));
    }

    TEST_F(AsyncData_UT, isValueReceived) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();

        auto const value = core::ut::ConstantValue<bool, 0>::getValue();

        ASSERT_FALSE(endpoint.isValueReceived());

        creator.set(value);
        endpoint.waitValue();
        ASSERT_TRUE(endpoint.isValueReceived());
        auto const receivedValueFromCreator = endpoint.get();
        ASSERT_FALSE(endpoint.isValueReceived());
        ASSERT_FALSE(creator.isValueReceived());

        endpoint.set(value);
        creator.waitValue();
        ASSERT_TRUE(creator.isValueReceived());
        auto const valueReceivedFromEndpoint = creator.get();
        ASSERT_FALSE(creator.isValueReceived());
        ASSERT_FALSE(endpoint.isValueReceived());
    }

    TEST_F(AsyncData_UT, waitValue) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();

        auto const value = core::ut::ConstantValue<bool, 0>::getValue();

        creator.set(value);
        ASSERT_NO_THROW(endpoint.waitValue());

        endpoint.set(value);
        ASSERT_NO_THROW(creator.waitValue());
    }

    TEST_F(AsyncData_UT, waitValueFor) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();
        auto const value = core::ut::ConstantValue<bool, 0>::getValue();

        ASSERT_FALSE(endpoint.waitValueFor(getTimeout(100)));

        creator.set(value);
        ASSERT_TRUE(endpoint.waitValueFor(getTimeout(100)));

        endpoint.set(value);
        ASSERT_TRUE(creator.waitValueFor(getTimeout(100)));
    }

    TEST_F(AsyncData_UT, waitValueUntil) {
        auto getTimeEndFromNow = [](const std::chrono::milliseconds &ellapseTime) {
            return std::chrono::steady_clock::now() + ellapseTime;
        };
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();
        auto const value = core::ut::ConstantValue<bool, 0>::getValue();

        ASSERT_FALSE(endpoint.waitValueUntil(getTimeEndFromNow(getTimeout(100))));

        creator.set(value);
        ASSERT_TRUE(endpoint.waitValueUntil(getTimeEndFromNow(getTimeout(100))));

        endpoint.set(value);
        ASSERT_TRUE(creator.waitValueUntil(getTimeEndFromNow(getTimeout(100))));
    }

    TEST_F(AsyncData_UT, reset) {
        auto creator  = makeAsyncData<bool>();
        auto endpoint = makeAsyncData<bool>(creator.getUriOfCreator());
        endpoint.connect();

        ASSERT_TRUE(creator.isConnected());
        ASSERT_TRUE(endpoint.isConnected());

        endpoint.reset();
        ASSERT_FALSE(endpoint.isConnected());
        ASSERT_TRUE(creator.waitDisconnectionFor(getTimeout(100)));

        endpoint.connect();
        ASSERT_TRUE(endpoint.isConnected());
        ASSERT_TRUE(creator.waitConnectionFor(getTimeout(100)));

        creator.reset();
        ASSERT_FALSE(creator.isConnected());
        ASSERT_TRUE(endpoint.waitDisconnectionFor(getTimeout(100)));
    }

    TEST_F(AsyncData_UT, serialization) {
        const std::string keyTest = "test";
        auto creator              = makeAsyncData<bool>();
        creator.set(true);

        auto const pStream = core::makeJsonStream();
        auto out           = core::makeKeyValue<AsyncData<bool>>(keyTest);

        *pStream << core::makeKeyValue(keyTest, creator);
        *pStream >> out;

        auto pEndpoint = out.getValue();
        ASSERT_EQ(true, pEndpoint.get());
    }

    TEST_F(AsyncData_UT, streamBytes) {
        using bytes_t = std::vector<unsigned char>;

        auto creator  = makeAsyncData<bytes_t>();
        auto endpoint = makeAsyncData<bytes_t>(creator.getUriOfCreator());

        const bytes_t bytes(10, 2);
        creator.set(bytes);
        auto const receivedBytes = endpoint.get();
        ASSERT_EQ(bytes, receivedBytes);
    }

    TEST_F(AsyncData_UT, checkDoubleConnection) {
        auto creator   = makeAsyncData<bool>();
        auto endpoint1 = makeAsyncData<bool>(creator.getUriOfCreator());
        creator.set(true);
        ASSERT_TRUE(endpoint1.get());
        endpoint1.reset();

        ASSERT_FALSE(endpoint1.isConnected());

        // creator.create();
        auto endpoint2 = makeAsyncData<bool>(creator.getUriOfCreator());
        creator.set(true);
        endpoint2.get();
    }

    TYPED_TEST(AsyncDataTyped_UT, get_without_callback) {
        auto creator              = makeAsyncData<TypeParam>();
        auto const expectedValue1 = core::ut::ConstantValue<TypeParam, 0>::getValue();

        // creator ==> endpoint
        creator.set(expectedValue1);
        auto endpoint                       = makeAsyncData<TypeParam>(creator.getUriOfCreator());
        auto const receivedValueFromCreator = std::move(endpoint.get());
        ASSERT_EQ(expectedValue1, receivedValueFromCreator);

        // endpoint ==> creator
        auto const expectedValue2 = core::ut::ConstantValue<TypeParam, 1>::getValue();
        endpoint.set(expectedValue2);
        auto const receivedValueFromEndpoint = creator.get();
        ASSERT_EQ(expectedValue2, receivedValueFromEndpoint);
    }

    TYPED_TEST(AsyncDataTyped_UT, get_with_callback) {
        auto creator = makeAsyncData<TypeParam>();
        std::promise<TypeParam> promiseCreator;
        std::future<TypeParam> futCreator = promiseCreator.get_future();
        creator.get([&promiseCreator](TypeParam &&value) { promiseCreator.set_value(std::move(value)); });

        auto endpoint = makeAsyncData<TypeParam>(creator.getUriOfCreator());
        std::promise<TypeParam> promiseEndpoint;
        std::future<TypeParam> futEndpoint = promiseEndpoint.get_future();
        endpoint.get([&promiseEndpoint](TypeParam &&value) { promiseEndpoint.set_value(std::move(value)); });

        auto const expectedValue1 = core::ut::ConstantValue<TypeParam, 0>::getValue();
        creator.set(expectedValue1);
        auto const receivedValueFromCreator = futEndpoint.get();
        ASSERT_EQ(expectedValue1, receivedValueFromCreator);

        auto const expectedValue2 = core::ut::ConstantValue<TypeParam, 1>::getValue();
        endpoint.set(expectedValue2);
        auto const receivedValueFromEndpoint = futCreator.get();
        ASSERT_EQ(expectedValue2, receivedValueFromEndpoint);
    }

    TYPED_TEST(AsyncDataTyped_UT, getFor) {
        auto creator              = makeAsyncData<TypeParam>();
        auto const expectedValue1 = core::ut::ConstantValue<TypeParam, 0>::getValue();

        // creator ==> endpoint
        auto endpoint = makeAsyncData<TypeParam>(creator.getUriOfCreator());
        auto getFor   = [](AsyncData<TypeParam> &asyncData, const std::chrono::milliseconds &timeout) { return asyncData.getFor(timeout); };
        ASSERT_THROW(getFor(endpoint, getTimeout(100)), DataExchangeException);

        TypeParam receivedValueFromCreator{ core::ut::ConstantValue<TypeParam, 1>::getValue() };
        creator.set(expectedValue1);
        ASSERT_NO_THROW(receivedValueFromCreator = endpoint.getFor(getTimeout(100)));
        ASSERT_EQ(expectedValue1, receivedValueFromCreator);

        // endpoint ==> creator
        auto const expectedValue2 = core::ut::ConstantValue<TypeParam, 1>::getValue();
        ASSERT_THROW(getFor(creator, getTimeout(100)), DataExchangeException);

        TypeParam receivedValueFromEndpoint{ core::ut::ConstantValue<TypeParam, 0>::getValue() };
        endpoint.set(expectedValue2);
        ASSERT_NO_THROW(receivedValueFromEndpoint = creator.getFor(getTimeout(100)));
        ASSERT_EQ(expectedValue2, receivedValueFromEndpoint);
    }

    TYPED_TEST(AsyncDataTyped_UT, getUntil) {
        auto creator              = makeAsyncData<TypeParam>();
        auto const expectedValue1 = core::ut::ConstantValue<TypeParam, 0>::getValue();

        // creator ==> endpoint
        auto endpoint = makeAsyncData<TypeParam>(creator.getUriOfCreator());
        auto getUntil = [](AsyncData<TypeParam> &asyncData, const std::chrono::time_point<std::chrono::steady_clock> &timePoint) {
            return asyncData.getUntil(timePoint);
        };
        ASSERT_THROW(getUntil(endpoint, std::chrono::steady_clock::now() + getTimeout(100)), DataExchangeException);

        TypeParam receivedValueFromCreator{ core::ut::ConstantValue<TypeParam, 1>::getValue() };
        creator.set(expectedValue1);
        ASSERT_NO_THROW(receivedValueFromCreator = endpoint.getUntil(std::chrono::steady_clock::now() + getTimeout(100)));
        ASSERT_EQ(expectedValue1, receivedValueFromCreator);

        // endpoint ==> creator
        auto const expectedValue2 = core::ut::ConstantValue<TypeParam, 1>::getValue();
        ASSERT_THROW(getUntil(creator, std::chrono::steady_clock::now() + getTimeout(100)), DataExchangeException);

        TypeParam receivedValueFromEndpoint{ core::ut::ConstantValue<TypeParam, 0>::getValue() };
        endpoint.set(expectedValue2);
        ASSERT_NO_THROW(receivedValueFromEndpoint = creator.getUntil(std::chrono::steady_clock::now() + getTimeout(100)));
        ASSERT_EQ(expectedValue2, receivedValueFromEndpoint);
    }

    TYPED_TEST(AsyncDataTyped_UT, getCallbackReceiver) {
        auto creator = makeAsyncData<TypeParam>();
        auto value   = core::ut::ConstantValue<TypeParam, 0>::getValue();
        creator.get([&value](TypeParam &&v) { value = v; });

        auto const expectedValue = core::ut::ConstantValue<TypeParam, 1>::getValue();
        auto valueToSet          = expectedValue;
        creator.getCallbackReceiver()(std::move(valueToSet));
        ASSERT_EQ(expectedValue, value);
    }

    TYPED_TEST(AsyncDataTyped_UT, set_with_copy) {
        auto creator  = makeAsyncData<TypeParam>();
        auto endpoint = makeAsyncData<TypeParam>(creator.getUriOfCreator());

        auto const expectedValue = core::ut::ConstantValue<TypeParam, 0>::getValue();
        creator.set(expectedValue);

        auto const receivedValueFromCreator = std::move(endpoint.get());
        ASSERT_EQ(expectedValue, receivedValueFromCreator);
    }

    TYPED_TEST(AsyncDataTyped_UT, set_with_move) {
        auto creator  = makeAsyncData<TypeParam>();
        auto endpoint = makeAsyncData<TypeParam>(creator.getUriOfCreator());

        auto const expectedValue = core::ut::ConstantValue<TypeParam, 0>::getValue();
        auto valueToSet          = expectedValue;
        creator.set(std::move(valueToSet));

        auto const receivedValueFromCreator = std::move(endpoint.get());
        ASSERT_EQ(expectedValue, receivedValueFromCreator);
    }

    TYPED_TEST(AsyncDataTyped_UT, getSet_emitter_is_creator) {
        auto const expectedValue = core::ut::ConstantValue<TypeParam, 1>::getValue();
        auto emitter             = makeAsyncData<TypeParam>();

        std::thread thEmitter([&emitter, &expectedValue]() { emitter.set(expectedValue); });
        std::thread thReceiver([uri = emitter.getUriOfCreator(), &expectedValue]() {
            AsyncData<TypeParam> receiver(uri);
            auto const receivedValue = std::move(receiver.get());
            ASSERT_EQ(expectedValue, receivedValue);
        });

        thEmitter.join();
        thReceiver.join();
    }

    TYPED_TEST(AsyncDataTyped_UT, getSet_receiver_is_creator) {
        auto receiver = makeAsyncData<TypeParam>();
        auto emitter  = makeAsyncData<TypeParam>(receiver.getUriOfCreator());

        auto const expectedValue = core::ut::ConstantValue<TypeParam, 1>::getValue();

        std::thread thEmitter([&emitter, &expectedValue]() { emitter.set(expectedValue); });
        std::thread thReceiver([&receiver, &expectedValue]() {
            auto const receivedValue = std::move(receiver.get());
            ASSERT_EQ(expectedValue, receivedValue);
        });

        thEmitter.join();
        thReceiver.join();
    }

} // namespace cho::osbase::data::ut
