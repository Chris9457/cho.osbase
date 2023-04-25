// \file  Observer.h
// \brief Declaration of classes of the pattern observer

#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <typeindex>
#include <vector>

/**
 * \defgroup PACKAGE_DESIGNPATTERN_OBSERVER Observer
 *
 * \brief Contains classes that realizes the pattern subject - observer
 *
 * The Observer design pattern defines a one-to-many dependency between objects so that when one object changes state, all its dependents
 * are notified and updated automatically.
 *
 * \par Creating an observer
 * To create an observer, the targeted class must inherit (in public) from the class Observer and passing in template parameters the list
 * of all types of messages to observe.\n
 * Thus, this class declares and implements all method "update" associated with each type of messages.\n
 * \n
 * Example :
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 class MyObserver : public <base_namespace>::osbase::core::Observer<Message1, Message2, Message3>
 {
 public:
         ...
         void update(const tools::Observable& rObservable, const Message1& msg) override { ... }
         void update(const tools::Observable& rObservable, const Message2& msg) override { ... }
         void update(const tools::Observable& rObservable, const Message3& msg) override { ... }
         ...
 };
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Creating an observable
 * To create an observable, the targeted class must inherit (in public) from the class Observable
 * \n
 * Example :
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 class MyObservable : public <base_namespace>::osbase::core::::Observable
 {
 public:
         ...
 };
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Subscription / Unsubsription to an observable
 * An observer subsribes to an observable by invoking its method <b>attach</b> or <b>attachAll</b>.\n
 * \n
 * Example:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 MyObserver myObserver;
 myObservable.attach<Message1>(myObserver); // myObserver is notified (update called) each time myObservable notifies Message1
 myObservable.attachAll(myObserver);        // myObserver is notified (update called) each time myObservable notifies Message1,
                                            // Message2 or Message3
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * \n
 * An observer unsubsribes to an observable by invoking its method <b>detach</b> or <b>detachAll</b>.\n
 * \n
 * Example:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 MyObserver myObserver;
 myObservable.detach<Message1>(myObserver); // myObserver isn't notified anymore(update called) when myObservable notifies Message1
 myObservable.detachAll(myObserver);        // myObserver isn't notified anymore(update called) when myObservable notifies Message1,
                                            // Message2 or Message3
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * More information at http://www.dofactory.com/net/observer-design-pattern
 *
 * \image html observer.png "UML Diagram"
 * \ingroup PACKAGE_DESIGNPATTERN
 */

namespace NS_OSBASE::core {

    class Observable;

    /**
     * \brief This class represents an observer in the pattern subject - observer. The template parameter indicate the type of message to
     * observe
     *
     * \ingroup PACKAGE_DESIGNPATTERN_OBSERVER
     */
    template <class TMessage>
    class TypedObserver {
        friend class Observable;

    public:
        using Type = TMessage; //!< alias of the template parameter

        TypedObserver() noexcept;
        TypedObserver(const TypedObserver &other) noexcept; //!< copy ctor
        TypedObserver(TypedObserver &&other) noexcept;      //!< movable ctor

        virtual ~TypedObserver(); //!< Dtor

        /**
         * \brief Method called by observables for notifications
         * \param	rObservable	Caller of this method
         * \param	msg			Message to notify - contains specific informations
         */
        virtual void update(const Observable &rObservable, const TMessage &msg) = 0;

        TypedObserver &operator=(const TypedObserver &other) noexcept; //!< copy assignment
        TypedObserver &operator=(TypedObserver &&other) noexcept;      //!< move assignment

    private:
        using WeakMarker = std::weak_ptr<bool>;

        void addObservable(Observable &observable);
        void removeObservable(Observable &observable);
        WeakMarker getWeakMarker() const noexcept;

        std::vector<Observable *> m_pObservables;
        std::recursive_mutex m_mutex;
        std::shared_ptr<bool> m_marker;
    };

    /**
     * \brief Helper class allowing to observe multiple message in one declaration
     *
     * \ingroup PACKAGE_DESIGNPATTERN_OBSERVER
     */
    template <typename... TMessages>
    class Observer {};

    /** \cond */
    template <typename TMessage, typename... TMessages>
    class Observer<TMessage, TMessages...> : public TypedObserver<TMessage>, public Observer<TMessages...> {
    public:
        using Head  = TypedObserver<TMessage>;
        using Queue = Observer<TMessages...>;
    };
    /** \endcond */

    /**
     * \brief This class represents a subject in the pattern subject - observer
     *
     * \ingroup PACKAGE_DESIGNPATTERN_OBSERVER
     */
    class Observable {

        template <typename T>
        friend class TypedObserver;

        friend void swap(Observable &lhs, Observable &rhs) noexcept;

    public:
        template <typename TMessage>
        void attach(TypedObserver<TMessage> &typedObserver); //!< Attaches the typed observer taking account the type of message observed
        template <typename... TMessages>
        void attachAll(Observer<TMessages...> &observer); //!< Attaches typed the observer for all observable messages
        template <typename TObserver>
        void attachAll(TObserver &observer); //!< Attaches the observer for all observable messages
        template <typename TMessage>
        void detach(TypedObserver<TMessage> &typedObserver); //!< Detaches the typed observer taking account the type of message observed
        template <typename... TMessages>
        void detachAll(Observer<TMessages...> &observer); //!< Detaches the typed observer for all observable messages
        template <typename TObserver>
        void detachAll(TObserver &observer); //!< Detaches the observer for all observable messages

    protected:
        Observable();
        Observable(const Observable &other) noexcept; //!< copy ctor
        Observable(Observable &&other) noexcept;      //!< covable ctor
        virtual ~Observable() noexcept;

        Observable &operator=(const Observable &obs) noexcept; //!< copy assignment
        Observable &operator=(Observable &&obs) noexcept;      //!< move assignment

        template <class TMessage>
        void notify(const TMessage &msg); //!< Notify the message <b>msg</b> of type <b>TMessage</b>

    private:
        class AbstractObserverContainer {
        public:
            virtual ~AbstractObserverContainer();
            virtual void attachAll(Observable &observable) = 0;
            virtual void detachAll(Observable &observable) = 0;
        };

        template <class TMessage>
        class ObserverContainer : public AbstractObserverContainer {
        public:
            using Observers = std::vector<TypedObserver<TMessage> *>;

            ObserverContainer(Observable &observable);
            ~ObserverContainer();

            void add(TypedObserver<TMessage> &typedObserver);
            void remove(TypedObserver<TMessage> &typedObserver);
            size_t getCount() const;
            bool isEmpty() const;
            const Observers &getObservers() const;
            void attachAll(Observable &observable) override;
            void detachAll(Observable &observable) override;

        private:
            Observable &m_observable;
            Observers m_observers;
        };

        using ObserverFamilies = std::map<std::type_index, std::shared_ptr<AbstractObserverContainer>>;

        template <typename TTypedObserver, typename... TObserver>
        void attach(TTypedObserver &typedObserver, TObserver &...observer);
        template <typename... T>
        void attach();

        template <typename TTypedObserver, typename... TObserver>
        void detach(TTypedObserver &typedObserver, TObserver &...observer);
        template <typename... T>
        void detach();

        template <typename TMessage>
        void removeTypedObserver(TypedObserver<TMessage> &typedObserver);

        template <typename TMessage>
        void removeFromTypedObserver(TypedObserver<TMessage> &typedObserver);

        template <typename TMessage>
        std::shared_ptr<ObserverContainer<TMessage>> getObserverContainer() const;

        ObserverFamilies m_observerFamilies;
        mutable std::recursive_mutex m_mutex;
    };

} // namespace NS_OSBASE::core

#include "Observer.inl"
