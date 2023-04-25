// \brief Declaration of the class DataExchanged

#pragma once
#include "IDataExchange.h"
#include <future>
#include <mutex>
#include <functional>

namespace NS_OSBASE::data {
    /**
     * \brief Utility class that allow to acquire data through a IDataExchange instance
     * \ingroup PACKAGE_OSBASE_EXCHANGE
     */
    template <typename T, bool Paged = false>
    class AsyncData {
    public:
        using TCallbackReceiver   = std::function<void(T &&)>;       //!< type of the callback used when receiving data
        using TCallbackConnection = std::function<void(const bool)>; //!< type of the callback used when a connection or disconnection is
        //!< raised
        using TCallbackFailure = std::function<void(std::string &&)>; //!< type of the callback used when a failure occurs

        AsyncData() = default;                 // default ctor - create an "empty shell" of the exchanged data
        AsyncData(Uri &uri);                   //!< reserve a new uri and create the endpoint based on this uri - return the reserved uri
        AsyncData(const Uri &uri);             //!< create the endpoint based on the given uri - open it if required
        AsyncData(const AsyncData &other);     //!< copy ctor
        AsyncData(AsyncData &&other) noexcept; //!< move ctor
        ~AsyncData();                          // dtor - destroy the endpoint if created, close it otherwise (if a connection is current)

        AsyncData &operator=(const AsyncData &other);     //!< copy assignment
        AsyncData &operator=(AsyncData &&other) noexcept; //!< move assignment

        [[nodiscard]] bool isValid() const noexcept; //!< indicate if the instance can exchange value

        void create(const std::string &scheme = IDataExchange::defaultScheme); //!< create the channel for the transport of data - throw
                                                                               //!< DataExchangeException if it is already valid

        [[nodiscard]] bool isConnected() const; //!< indicate if the instance is connected to another endpoint

        // Connection method: available only for endpoints instantiated with an valid Uri (opening)
        void connect();                               //!< connect a non created endpoint (wait indefinitelly)
        void connect(TCallbackConnection &&callback); //!< connect a non created endpoint (call the callback on connection / disconnection)
        template <class Rep, class Period>
        [[nodiscard]] bool tryConnectFor(const std::chrono::duration<Rep, Period> &timeout_duration); //!< try to connect a non created
                                                                                                      //!< endpoint during an
                                                                                                      //!< elapsed time
        template <class Clock, class Duration>
        [[nodiscard]] bool tryConnectUntil(const std::chrono::time_point<Clock, Duration> &timeout_time); //!< try to connect a non created
                                                                                                          //!< endpoint
                                                                                                          //!< until an end time

        // Wait connection method: available only for endpoints that reserved a valid Uri (creation)
        void waitConnection();                               //!< wait indefinitely a connection (for created endpoint)
        void waitConnection(TCallbackConnection &&callback); //!< wait for a connection (for created endpoint - call the
                                                             //!< callback on connection / disconnection)
        template <class Rep, class Period>
        [[nodiscard]] bool waitConnectionFor(const std::chrono::duration<Rep, Period> &timeout_duration); //!< wait a connection during an
                                                                                                          //!< elapsed
                                                                                                          //!< time (for created endpoint)
        template <class Clock, class Duration>
        [[nodiscard]] bool waitConnectionUntil(const std::chrono::time_point<Clock, Duration> &timeout_time); //!< wait a connection until
                                                                                                              //!< an end
        //!< time (for created endpoint)

        // Wait disconnection methods: available for both (create and endpoint)
        void waitDisconnection(); //!< wait indefinitely a deconnection
        template <class Rep, class Period>
        [[nodiscard]] bool waitDisconnectionFor(const std::chrono::duration<Rep, Period> &timeout_duration); //!< wait a disconnection
                                                                                                             //!< during an
        //!< elapsed time (for created
        //!< endpoint)
        template <class Clock, class Duration>
        [[nodiscard]] bool waitDisconnectionUntil(const std::chrono::time_point<Clock, Duration> &timeout_time); //!< wait a
                                                                                                                 //!< disconnection
                                                                                                                 //!< until an
        //!< end time (for created
        //!< endpoint)

        T get(); //!< wait until a data is received and return it - throw DataExchangeException if a callback receiver has been set
        template <class Rep, class Period>
        [[nodiscard]] T getFor(const std::chrono::duration<Rep, Period> &timeout_duration); //!< wait until a data is received or a for
                                                                                            //!< duration and
                                                                                            //!< return it
        //!< - throw DataExchangeException if a callback receiver has
        //!< been set or the timeout is reached
        template <class Clock, class Duration>
        [[nodiscard]] T getUntil(const std::chrono::time_point<Clock, Duration> &timeout_time); //!< wait until a data is received or a
                                                                                                //!< until a time and
        //!< return it - throw DataExchangeException if a callback
        //!< receiver has been set or the timeout is reached

        void get(TCallbackReceiver &&callback);               //!< request the value tha will be set through the callback receiver - throw
                                                              //!< DataExchangeException if a a data is being retrieved
        [[nodiscard]] bool isValueReceived() const;           //!< indicate if a value has been received
        const TCallbackReceiver &getCallbackReceiver() const; //!< return the associated callback receiver

        void set(const T &value); //!< set a value - in override mode replace the current one even if it was not pushed
        void set(T &&value);      //!< set a value - in override mode replace the current one even if it was not pushed

        void waitValue(); //!< wait indifinitively the end of the current request (get or set)
        template <class Rep, class Period>
        [[nodiscard]] bool waitValueFor(const std::chrono::duration<Rep, Period> &timeout_duration); //!< wait for the end of the
                                                                                                     //!< current request (get or set)
                                                                                                     //!< during the duration
        template <class Clock, class Duration>
        [[nodiscard]] bool waitValueUntil(const std::chrono::time_point<Clock, Duration> &timeout_time); //!< wait for the end of the
                                                                                                         //!< current request (get or
                                                                                                         //!< set) until the timeout
                                                                                                         //!< time

        void reset(); //!< close or destroy the pending connection

        [[nodiscard]] Uri getUriOfCreator() const; //!< return the uri of the creator

        void setCallbackFailure(const TCallbackFailure &callback); //!< set the callback use when a failure occurs

        operator Uri() const; //!< cast opertor for uri

    private:
        class DataExchangeDelegate;
        using DataExchangeDelegatePtr = std::shared_ptr<DataExchangeDelegate>;

        AsyncData(const std::string &scheme, bool bCreated);

        void pushValueIn();
        void swap(AsyncData &other) noexcept;
        void onConnected(const bool bConnected);
        void onFailure(std::string &&failure) const;
        void onDataReceived(ByteBuffer &&buffer);

        IDataExchangePtr m_pDataExchange;
        DataExchangeDelegatePtr m_pDataExchangeDelegate;
        bool m_bCreated = true;

        std::optional<T> m_valueIn;
        mutable std::mutex m_mutexValueIn;

        std::optional<T> m_valueOut;
        mutable std::mutex m_mutexValueOut;
        std::condition_variable m_cvValueOut;

        bool m_bConnected = false;
        std::mutex m_mutexConnected;
        std::condition_variable m_cvConnected;

        TCallbackConnection m_callbackConnection;
        TCallbackReceiver m_callbackReceiver;
        TCallbackFailure m_callbackFailure;
        Uri m_uriOfCreator;
    };

    template <typename T>
    using AsyncPagedData = AsyncData<T, true>; // alias for paged async data

    template <typename T>
    AsyncData<T> makeAsyncData(); // reserved an uri and create an async data based on this new uri

    template <typename T>
    AsyncData<T> makeAsyncData(const Uri &uri); // create an async data based on the existing uri

    template <typename T>
    AsyncPagedData<T> makeAsyncPagedData(); // reserved an uri and create an async paged data based on this new uri

    template <typename T>
    AsyncPagedData<T> makeAsyncPagedData(const Uri &uri); // create an async paged data based on the existing uri
} // namespace NS_OSBASE::data

/**
 * \private
 */
template <typename T, bool Paged>
struct NS_OSBASE::core::KeySerializer<std::string, NS_OSBASE::data::AsyncData<T, Paged>, false> {
    static data::AsyncData<T, Paged> getValue(
        KeyStream<std::string> &keyStream, const data::AsyncData<T, Paged> &defaultValue);           //!< \private
    static bool setValue(KeyStream<std::string> &keyStream, const data::AsyncData<T, Paged> &value); //!< \private
};

#include "AsyncData.inl"
