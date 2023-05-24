// \brief Interface to communication with other modules of the same project

#pragma once
#include "osCore/DesignPattern/Observer.h"
#include "osData/Uri.h"
#include <memory>
#include <string>

#include "IMessaging.h"

/**
 * \defgroup PACKAGE_OSBASE_IMESSAGING Messaging Interface
 *
 * \brief Contains classes that enable the use of the IMessaging interface used as a messaging service
 *
 * The interface contains definition of delegates that are consumed by IMessaging as well as the IMessage interface that should be inherited
 * when creating a concrete implementation of a messaging method.
 *
 * \par Implementing a messaging system
 * To create a messaging system you need to inherit publicly of IMessage and implement all the pure virtual functions.
 * In order to call the methods of your implementation the consumer will need to create concrete implementation of the required delegates.
 *
 * \ingroup PACKAGE_OSDATA
 */

namespace NS_OSBASE::data {

    /**
     * \addtogroup PACKAGE_OSBASE_IMESSAGING
     * \{
     */

    class IMessaging;
    /**
     * Typedef of shared pointer to IMessaging
     */
    using IMessagingPtr = std::shared_ptr<IMessaging>;

    /**
     * \brief Interface used to define the contract required to implement a messaging service
     */
    class IMessaging : public core::Observable {
    public:
        using JsonText = std::string; //!< alias that reflecting the Json text received in the delegates

        class MessagingConnectionMsg {
        public:
            MessagingConnectionMsg(const bool bConnected);
            bool isConnected() const;

        private:
            bool m_bConnected;
        };

        /**
         * \brief Delegate interface passed to the invoke method in order to receive the result of the invoke
         */
        class IClientDelegate {
        public:
            virtual ~IClientDelegate(); //!< Dtor

            /**
             * \brief Function called when the result of the invoke is received
             * \param   json    Stringified JSON string containing the result of the invoke that can be parsed by the receiver
             */
            virtual void onResult(const JsonText &json) = 0;
        };
        using IClientDelegatePtr  = std::shared_ptr<IClientDelegate>; //!< alias of shared pointer to IClientDelegate
        using IClientDelegateWPtr = std::weak_ptr<IClientDelegate>;   //!< alias of weak pointer to IClientDelegate

        /**
         * \brief Delegate interface passed when registering an RPC call. Will be used to compute the result of the RPC call
         **/
        class ISupplierDelegate {
        public:
            virtual ~ISupplierDelegate(); //!< Dtor

            /**
             * \brief Function called when an invoke of the registered RPC call happens
             * \param   json    Stringified JSON string containing the parameters passed in the call
             */
            virtual std::string onCall(const JsonText &json) = 0;
        };
        using ISupplierDelegatePtr  = std::shared_ptr<ISupplierDelegate>; //!< alias of shared pointer to ISupplierDelegate
        using ISupplierDelegateWPtr = std::weak_ptr<ISupplierDelegate>;   // alias of weak pointer to ISupplierDelegate

        /**
         * \brief Delegate interface passed when subscribing to an event. The onEvent method will be called when an event happens
         **/
        class IEventDelegate {
        public:
            virtual ~IEventDelegate(); //!< Dtor

            /**
             * \brief Function called when the event the owner subscribed to happens
             * \param   json    Stringified JSON string containing the parameter sent with the event that can be parsed by the receiver
             */
            virtual void onEvent(const JsonText &json) = 0;
        };
        using IEventDelegatePtr  = std::shared_ptr<IEventDelegate>; //!< alias of shared pointer to IEventDelegate
        using IEventDelegateWPtr = std::weak_ptr<IEventDelegate>;   // alias of weak pointer to IEventDelegate

        /**
         * \brief Delegate interface passed to return errors that happen asynchronously. The onError method will be called when an error
         * happens
         */
        class IErrorDelegate {
        public:
            virtual ~IErrorDelegate(); //!< Dtor

            /**
             * \brief Function called when an error happens
             * \param   error    String containing details about the error
             */
            virtual void onError(const std::string &error) = 0;
        };
        using IErrorDelegatePtr  = std::shared_ptr<IErrorDelegate>; //!< alias of shared pointer to IErrorDelegate
        using IErrorDelegateWPtr = std::weak_ptr<IErrorDelegate>;   //!< alias of weak pointer to IErrorDelegate

        ~IMessaging() override; //!< dtor

        virtual void connect() = 0;    //!< Method to connect to the messaging service. Needs to be called before using any of the other
                                       //!< function.
        virtual void disconnect() = 0; //!< Method to graciously disconnect from the messaging service. No subsequent call should be done
                                       //!< after this.
        /**
         * \brief Register a RPC call in the messaging service.
         * \param   uri         Uri (name) of the RPC call
         * \param   pDelegate   Pointer to an ISupplierDelegate whose onCall method will be invoked when an invoke call for this uri happens
         * \param   pError      Pointer to an IErrorDelegate whose onError will be called when an error happen at registration time
         * \throws  MessagingException      Exception thrown when calling the function without being connected or when trying to register a
         * call that has already been registered
         */
        virtual void registerCall(const std::string &uri, ISupplierDelegatePtr pDelegate, IErrorDelegatePtr pError) = 0;

        /**
         * \brief Unregister a RPC call in the messaging service.
         * \param   uri         Uri (name) of the RPC call
         * \param   pError      Pointer to an IErrorDelegate whose onError will be called when an error happen at unregistration time
         * \throws  MessagingException      Exception thrown when calling the function without being connected or when trying to
         * unregistered a call that has not been registered
         */
        virtual void unregisterCall(const std::string &uri, IErrorDelegatePtr pError) = 0;

        /**
         * \brief Invoke a registered RPC call in the messaging service.
         * \param   uri             Uri (name) of the RPC call
         * \param   argsSerialized  Serialized version of the JSON object to send to the RPC method
         * \param   pDelegate       Pointer to an IClientDelegate whose onResult method will be invoked when the invoked method will return
         * a result. \param   pError          Pointer to an IErrorDelegate whose onError will be called when an error happen at invocation
         * time. \throws  MessagingException      Exception thrown when calling the function without being connected
         */
        virtual void invoke(
            const std::string &uri, const std::string &argsSerialized, IClientDelegatePtr pDelegate, IErrorDelegatePtr pError) const = 0;

        /**
         * \brief Subscribe to a topic in the messaging service.
         * \param   topic       Topic to subscribe to.
         * \param   pDelegate   Pointer to an IEventDelegate whose onEvent method will be invoked when an event happens.
         * \param   pError      Pointer to an IErrorDelegate whose onError will be called when an error happen at subscription time.
         * \throws  MessagingException      Exception thrown when calling the function without being connected or when subscribing to an
         * already subscribed topic
         */
        virtual void subscribe(const std::string &topic, IEventDelegatePtr pDelegate, IErrorDelegatePtr pError) = 0;

        /**
         * \brief Unsubscribe to a topic in the messaging service.
         * \param   topic       Topic to unsubscribe to.
         * \param   pError      Pointer to an IErrorDelegate whose onError will be called when an error happen at unsubscription time.
         * \throws  MessagingException      Exception thrown when calling the function without being connected
         */
        virtual void unsubscribe(const std::string &topic, IErrorDelegatePtr pError) = 0;

        /**
         * \brief Publish a message and parameters to a topic in the messaging service.
         * \param   topic           Topic to publish to.
         * \param   argsSerialized  Serialized arguments to send along the published message
         * \param   pError          Pointer to an IErrorDelegate whose onError will be called when an error happen at publish time.
         * \throws  MessagingException      Exception thrown when calling the function without being connected
         */
        virtual void publish(const std::string &topic, const std::string &argsSerialized, IErrorDelegatePtr pError) const = 0;

        static inline std::string DEFAULT_REALM = "osbase";
    };

    IMessagingPtr makeWampMessaging(const Uri &uri, const std::string &realm = IMessaging::DEFAULT_REALM); //!< create a IMessaging

    /** \} */
} // namespace NS_OSBASE::data
