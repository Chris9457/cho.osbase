// \brief Declaration of the class DataExchanged

#pragma once
#include "PagedDataExchange.h"
#include "osCore/Serialization/Serializer.h"

namespace cho::osbase::data {

    /*
     * \class AsyncData<T>::DataExchangeDelegate
     */
    template <typename T, bool Paged>
    class AsyncData<T, Paged>::DataExchangeDelegate : public IDataExchange::IDelegate {
    public:
        DataExchangeDelegate(AsyncData<T, Paged> &AsyncData) : m_AsyncData(AsyncData) {
        }

        void onConnected(const bool connected) override {
            m_AsyncData.onConnected(connected);
        }

        void onFailure(std::string &&failure) override {
            m_AsyncData.onFailure(std::move(failure));
        }

        void onDataReceived(ByteBuffer &&buffer) override {
            m_AsyncData.onDataReceived(std::move(buffer));
        }

    private:
        AsyncData<T, Paged> &m_AsyncData;
    };

    /*
     * \class AsyncData
     */
    template <typename T, bool Paged>
    AsyncData<T, Paged>::AsyncData(const std::string &scheme, bool bCreated)
        : m_pDataExchange(Paged ? makePagedDataExchange(scheme) : makeDataExchange(scheme)),
          m_pDataExchangeDelegate(std::make_shared<DataExchangeDelegate>(*this)),
          m_bCreated(bCreated) {
        m_pDataExchange->setDelegate(m_pDataExchangeDelegate);
    }

    template <typename T, bool Paged>
    AsyncData<T, Paged>::AsyncData(Uri &uri) : AsyncData<T, Paged>(uri.scheme.empty() ? Uri::schemeWebsocket() : uri.scheme, true) {
        m_pDataExchange->create();
        m_uriOfCreator = m_pDataExchange->getUriOfCreator();
        uri            = m_uriOfCreator;
    }

    template <typename T, bool Paged>
    AsyncData<T, Paged>::AsyncData(const Uri &uri) : AsyncData<T, Paged>(uri.scheme, false) {
        m_uriOfCreator = uri;
    }

    template <typename T, bool Paged>
    AsyncData<T, Paged>::AsyncData(const AsyncData &other) : AsyncData<T, Paged>(other.getUriOfCreator()) {
    }

    template <typename T, bool Paged>
    AsyncData<T, Paged>::AsyncData(AsyncData &&other) noexcept {
        swap(other);
    }

    template <typename T, bool Paged>
    AsyncData<T, Paged>::~AsyncData() {
        reset();
    }

    template <typename T, bool Paged>
    AsyncData<T, Paged> &AsyncData<T, Paged>::operator=(const AsyncData &other) {
        if (this == &other) {
            return *this;
        }

        AsyncData<T, Paged> temp(other);
        swap(temp);
        return *this;
    }

    template <typename T, bool Paged>
    AsyncData<T, Paged> &AsyncData<T, Paged>::operator=(AsyncData &&other) noexcept {
        AsyncData<T, Paged> temp(std::move(other));
        swap(temp);
        return *this;
    }

    template <typename T, bool Paged>
    bool AsyncData<T, Paged>::isValid() const noexcept {
        return m_pDataExchange != nullptr && m_pDataExchangeDelegate != nullptr;
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::create(const std::string &scheme) {
        Uri uri = { scheme };
        *this   = AsyncData(uri);
    }

    template <typename T, bool Paged>
    T AsyncData<T, Paged>::get() {
        return getUntil(std::chrono::time_point<std::chrono::steady_clock>::max());
    }

    template <typename T, bool Paged>
    template <class Rep, class Period>
    T AsyncData<T, Paged>::getFor(const std::chrono::duration<Rep, Period> &timeout_duration) {
        return getUntil(std::chrono::steady_clock::now() + timeout_duration);
    }

    template <typename T, bool Paged>
    template <class Clock, class Duration>
    T AsyncData<T, Paged>::getUntil(const std::chrono::time_point<Clock, Duration> &timeout_time) {
        if (!isValid()) {
            onFailure("AsyncData: unable to exchange value");
            return {};
        }

        if (m_callbackReceiver) {
            onFailure("AsyncData: callback already set");
            return {};
        }

        if (m_bCreated) {
            if (!waitConnectionUntil(timeout_time)) {
                onFailure("AsyncData: not connected");
                return {};
            }
        } else if (!tryConnectUntil(timeout_time)) {
            onFailure("AsyncData: not connected");
            return {};
        }

        if (!waitValueUntil(timeout_time)) {
            onFailure("AsyncData: value not received");
            return {};
        }

        std::lock_guard lockValueReceived(m_mutexValueOut);
        auto const guard = core::make_scope_exit([this]() { m_valueOut.reset(); });
        return std::move(m_valueOut.value());
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::get(TCallbackReceiver &&callback) {
        if (!isValid()) {
            onFailure("AsyncData: unable to exchange value");
            return;
        }

        if (!m_bCreated && !isConnected()) {
            m_pDataExchange->open(getUriOfCreator());
        }

        m_callbackReceiver = std::move(callback);

        if (m_callbackReceiver) {
            // forward the pending received value
            if (isValueReceived() || waitValueFor(std::chrono::milliseconds(0))) {
                std::lock_guard lock(m_mutexValueOut);
                auto const guard = core::make_scope_exit([this]() { m_valueOut.reset(); });
                m_callbackReceiver(std::move(m_valueOut.value()));
            }
        }
    }

    template <typename T, bool Paged>
    const typename AsyncData<T, Paged>::TCallbackReceiver &AsyncData<T, Paged>::getCallbackReceiver() const {
        return m_callbackReceiver;
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::set(const T &value) {
        auto tempValue = value;
        set(std::move(tempValue));
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::set(T &&value) {
        if (!isValid()) {
            onFailure("AsyncData: unable to exchange value");
            return;
        }

        if (!m_bCreated && !isConnected()) {
            connect();
        }

        {
            std::lock_guard lock(m_mutexValueIn);
            m_valueIn = std::move(value);
        }

        if (isConnected()) {
            pushValueIn();
        }
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::pushValueIn() {
        std::lock_guard lock(m_mutexValueIn);
        if (!m_valueIn.has_value()) {
            return;
        }

        auto const guard = core::make_scope_exit([this]() { m_valueIn.reset(); });
        ByteBuffer buffer;
        if (!core::Serializable<T, ByteBuffer::value_type>::write(std::move(m_valueIn.value()), buffer)) {
            onFailure("AsyncData: unable to write the buffer");
            return;
        }

        m_pDataExchange->push(buffer);
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::connect() {
        if (!tryConnectUntil(std::chrono::time_point<std::chrono::steady_clock>::max())) {
            onFailure("AsyncData: timeout connection");
        }
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::connect(TCallbackConnection &&callback) {
        if (!isValid()) {
            onFailure("AsyncData: unable to connect");
        }

        if (m_bCreated) {
            onFailure("AsyncData: connect methods available for non creator AsyncData");
        }

        if (!callback) {
            onFailure("AsyncData: callback not valid");
        }

        m_callbackConnection = callback;
        if (isConnected()) {
            m_callbackConnection(true);
        } else {
            m_pDataExchange->open(getUriOfCreator());
        }
    }

    template <typename T, bool Paged>
    template <class Rep, class Period>
    bool AsyncData<T, Paged>::tryConnectFor(const std::chrono::duration<Rep, Period> &timeout_duration) {
        return tryConnectUntil(std::chrono::steady_clock::now() + timeout_duration);
    }

    template <typename T, bool Paged>
    template <class Clock, class Duration>
    bool AsyncData<T, Paged>::tryConnectUntil(const std::chrono::time_point<Clock, Duration> &timeout_time) {
        if (!isValid()) {
            onFailure("AsyncData: unable to connect");
        }

        if (m_bCreated) {
            onFailure("AsyncData: connect methods available for non creator AsyncData");
        }

        if (isConnected()) {
            return true;
        }

        m_pDataExchange->open(m_uriOfCreator);
        std::unique_lock lock(m_mutexConnected);
        return m_cvConnected.wait_until(lock, timeout_time, [this]() { return m_bConnected; });
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::waitConnection() {
        if (!waitConnectionUntil(std::chrono::time_point<std::chrono::steady_clock>::max())) {
            onFailure("AsyncData: timeout waiting");
        }
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::waitConnection(TCallbackConnection &&callback) {
        if (!isValid()) {
            onFailure("AsyncData: unable to wait connection");
        }

        if (!m_bCreated) {
            onFailure("AsyncData: waitConnection methods available for creator AsyncData");
        }

        if (!callback) {
            onFailure("AsyncData: callback not valid");
        }

        m_callbackConnection = callback;
        if (isConnected()) {
            m_callbackConnection(true);
        }
    }

    template <typename T, bool Paged>
    template <class Rep, class Period>
    bool AsyncData<T, Paged>::waitConnectionFor(const std::chrono::duration<Rep, Period> &timeout_duration) {
        return waitConnectionUntil(std::chrono::steady_clock::now() + timeout_duration);
    }

    template <typename T, bool Paged>
    template <class Clock, class Duration>
    bool AsyncData<T, Paged>::waitConnectionUntil(const std::chrono::time_point<Clock, Duration> &timeout_time) {
        if (!isValid()) {
            onFailure("AsyncData: unable to connect");
        }

        if (!m_bCreated) {
            onFailure("AsyncData: waitConnection methods available for creator AsyncData");
        }

        if (isConnected()) {
            return true;
        }

        std::unique_lock lock(m_mutexConnected);
        return m_cvConnected.wait_until(lock, timeout_time, [this]() { return m_bConnected; });
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::waitDisconnection() {
        if (!waitDisconnectionUntil(std::chrono::time_point<std::chrono::steady_clock>::max())) {
            onFailure("AsyncData: timeout waiting disconnection");
        }
    }

    template <typename T, bool Paged>
    template <class Rep, class Period>
    bool AsyncData<T, Paged>::waitDisconnectionFor(const std::chrono::duration<Rep, Period> &timeout_duration) {
        return waitDisconnectionUntil(std::chrono::steady_clock::now() + timeout_duration);
    }

    template <typename T, bool Paged>
    template <class Clock, class Duration>
    bool AsyncData<T, Paged>::waitDisconnectionUntil(const std::chrono::time_point<Clock, Duration> &timeout_time) {
        if (!isValid()) {
            onFailure("AsyncData: unable to disconnect");
        }

        if (!isConnected()) {
            return true;
        }

        std::unique_lock lock(m_mutexConnected);
        return m_cvConnected.wait_until(lock, timeout_time, [this]() { return !m_bConnected; });
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::waitValue() {
        if (!waitValueUntil(std::chrono::time_point<std::chrono::steady_clock>::max())) {
            onFailure("AsyncData: timeout waiting");
        }
    }

    template <typename T, bool Paged>
    template <class Rep, class Period>
    bool AsyncData<T, Paged>::waitValueFor(const std::chrono::duration<Rep, Period> &timeout_duration) {
        return waitValueUntil(std::chrono::steady_clock::now() + timeout_duration);
    }

    template <typename T, bool Paged>
    template <class Clock, class Duration>
    bool AsyncData<T, Paged>::waitValueUntil(const std::chrono::time_point<Clock, Duration> &timeout_time) {
        if (isValueReceived()) {
            return true;
        }

        std::unique_lock lock(m_mutexValueOut);
        return m_cvValueOut.wait_until(lock, timeout_time, [this]() { return m_valueOut.has_value(); });
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::reset() {
        if (!isValid()) {
            return;
        }

        const bool connected = isConnected();
        if (m_bCreated) {
            m_pDataExchange->destroy();
        } else if (connected) {
            m_pDataExchange->close();
        }

        if (connected) {
            waitDisconnection();
        }
    }

    template <typename T, bool Paged>
    Uri AsyncData<T, Paged>::getUriOfCreator() const {
        return m_uriOfCreator;
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::setCallbackFailure(const TCallbackFailure &callback) {
        m_callbackFailure = callback;
    }

    template <typename T, bool Paged>
    AsyncData<T, Paged>::operator Uri() const {
        return getUriOfCreator();
    }

    template <typename T, bool Paged>
    bool AsyncData<T, Paged>::isConnected() const {
        return isValid() && m_pDataExchange->isWired();
    }

    template <typename T, bool Paged>
    bool AsyncData<T, Paged>::isValueReceived() const {
        std::lock_guard lock(m_mutexValueOut);
        return m_valueOut.has_value();
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::swap(AsyncData &other) noexcept {
        std::lock_guard lockIn(m_mutexValueIn);
        std::lock_guard lockOut(m_mutexValueOut);
        std::lock_guard lockOtherIn(other.m_mutexValueIn);
        std::lock_guard lockOtherOut(other.m_mutexValueOut);

        std::swap(m_pDataExchange, other.m_pDataExchange);
        if (m_pDataExchange != nullptr) {
            if (m_pDataExchangeDelegate == nullptr) {
                m_pDataExchangeDelegate = std::make_shared<DataExchangeDelegate>(*this);
            }
            m_pDataExchange->setDelegate(m_pDataExchangeDelegate);
        }
        if (other.m_pDataExchange != nullptr) {
            if (other.m_pDataExchangeDelegate == nullptr) {
                other.m_pDataExchangeDelegate = std::make_shared<DataExchangeDelegate>(*this);
            }
            other.m_pDataExchange->setDelegate(other.m_pDataExchangeDelegate);
        }

        std::swap(m_bCreated, other.m_bCreated);
        std::swap(m_valueIn, other.m_valueIn);
        std::swap(m_valueOut, other.m_valueOut);
        std::swap(m_bConnected, other.m_bConnected);
        std::swap(m_callbackReceiver, other.m_callbackReceiver);
        std::swap(m_callbackFailure, other.m_callbackFailure);
        std::swap(m_uriOfCreator, other.m_uriOfCreator);
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::onConnected(const bool bConnected) {
        if (bConnected) {
            pushValueIn();
        }

        if (m_callbackConnection) {
            m_callbackConnection(bConnected);
        } else {
            {
                std::lock_guard lock(m_mutexConnected);
                m_bConnected = bConnected;
            }
            m_cvConnected.notify_one();
        }
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::onFailure(std::string &&failure) const {
        if (m_callbackFailure) {
            m_callbackFailure(std::move(failure));
        } else {
            throw DataExchangeException(failure);
        }
    }

    template <typename T, bool Paged>
    void AsyncData<T, Paged>::onDataReceived(ByteBuffer &&buffer) {
        T value    = {};
        size_t pos = 0;
        if (!core::Serializable<T, ByteBuffer::value_type>::read(value, buffer, pos)) {
            onFailure("AsyncData: unable to read the buffer");
        }

        if (m_callbackReceiver) {
            m_callbackReceiver(std::move(value));
        } else {
            {
                std::lock_guard lock(m_mutexValueOut);
                m_valueOut = value;
            }
            m_cvValueOut.notify_one();
        }
    }

    /*
     * makers
     */
    namespace internal {
        template <typename T, bool Paged>
        AsyncData<T, Paged> _makeAsyncData() {
            Uri uri = { Uri::schemeWebsocket() };
            return AsyncData<T, Paged>(uri);
        }

        template <typename T, bool Paged>
        AsyncData<T, Paged> _makeAsyncData(const Uri &uri) {
            return AsyncData<T, Paged>(uri);
        }
    } // namespace internal

    template <typename T>
    AsyncData<T> makeAsyncData() {
        return internal::_makeAsyncData<T, false>();
    }

    template <typename T>
    AsyncData<T> makeAsyncData(const Uri &uri) {
        return internal::_makeAsyncData<T, false>(uri);
    }

    template <typename T>
    AsyncPagedData<T> makeAsyncPagedData() {
        return internal::_makeAsyncData<T, true>();
    }

    template <typename T>
    AsyncPagedData<T> makeAsyncPagedData(const Uri &uri) {
        return internal::_makeAsyncData<T, true>(uri);
    }
} // namespace cho::osbase::data

template <typename T, bool Paged>
cho::osbase::data::AsyncData<T, Paged>
cho::osbase::core::KeySerializer<std::string, cho::osbase::data::AsyncData<T, Paged>, false>::getValue(
    KeyStream<std::string> &keyStream, const data::AsyncData<T, Paged> &) {
    auto const uri = KeySerializer<std::string, data::Uri>::getValue(keyStream, data::Uri{});
    return data::AsyncData<T, Paged>(uri);
}

template <typename T, bool Paged>
bool cho::osbase::core::KeySerializer<std::string, cho::osbase::data::AsyncData<T, Paged>, false>::setValue(
    KeyStream<std::string> &keyStream, const data::AsyncData<T, Paged> &value) {
    return KeySerializer<std::string, data::Uri>::setValue(keyStream, value.getUriOfCreator());
}
