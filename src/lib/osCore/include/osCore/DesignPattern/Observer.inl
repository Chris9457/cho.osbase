// \file  Observer.inl
// \brief Implementation of template methods and classes of the pattern Observer

namespace cho::osbase::core {

    /*
     * \class TypedObserver
     */
    template <typename TMessage>
    TypedObserver<TMessage>::TypedObserver() noexcept : m_marker(std::make_shared<bool>(true)) {
    }

    template <typename TMessage>
    TypedObserver<TMessage>::TypedObserver(const TypedObserver<TMessage> &other) noexcept : m_marker(std::make_shared<bool>(true)) {
        *this = other;
    }

    template <typename TMessage>
    TypedObserver<TMessage>::TypedObserver(TypedObserver<TMessage> &&other) noexcept : m_marker(std::make_shared<bool>(true)) {
        *this = std::move(other);
    }

    template <typename TMessage>
    TypedObserver<TMessage>::~TypedObserver() {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        auto const pObservables = m_pObservables;

        for (auto &&pObservable : m_pObservables) {
            pObservable->detach<TMessage>(*this);
        }
    }

    template <typename TMessage>
    TypedObserver<TMessage> &TypedObserver<TMessage>::operator=(const TypedObserver<TMessage> &other) noexcept {
        if (this == &other)
            return *this;

        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        auto const pLObservables = m_pObservables;
        auto const pRObservables = other.m_pObservables;

        for (auto &&pLObservable : pLObservables) {
            pLObservable->detach<TMessage>(*this);
        }

        for (auto &&pRObservable : pRObservables) {
            pRObservable->attach<TMessage>(*this);
        }

        return *this;
    }

    template <typename TMessage>
    TypedObserver<TMessage> &TypedObserver<TMessage>::operator=(TypedObserver<TMessage> &&other) noexcept {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);

        auto const pLObservables = m_pObservables;
        auto const pRObservables = other.m_pObservables;

        for (auto &&pLObservable : pLObservables) {
            pLObservable->detach<TMessage>(*this);
        }

        for (auto &&pRObservable : pRObservables) {
            pRObservable->attach<TMessage>(*this);
            pRObservable->detach<TMessage>(other);
        }

        return *this;
    }

    template <typename TMessage>
    typename TypedObserver<TMessage>::WeakMarker TypedObserver<TMessage>::getWeakMarker() const noexcept {
        return m_marker;
    }

    template <typename TMessage>
    void TypedObserver<TMessage>::addObservable(Observable &observable) {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        m_pObservables.push_back(&observable);
    }

    template <typename TMessage>
    void TypedObserver<TMessage>::removeObservable(Observable &observable) {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        m_pObservables.erase(
            std::remove_if(
                m_pObservables.begin(), m_pObservables.end(), [&observable](auto pObservable) { return pObservable == &observable; }),
            m_pObservables.end());
    }

    /*
     * \class Observable
     */
    template <typename... T>
    void Observable::attach(){};

    template <typename TMessage>
    void Observable::attach(TypedObserver<TMessage> &typedObserver) {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        const std::type_index indexFamily(typeid(TMessage));
        ObserverFamilies::iterator itFamily = m_observerFamilies.find(indexFamily);

        if (itFamily == m_observerFamilies.end())
            itFamily = m_observerFamilies.insert({ indexFamily, std::make_shared<ObserverContainer<TMessage>>(*this) }).first;

        std::shared_ptr<ObserverContainer<TMessage>> pObserverContainer =
            std::dynamic_pointer_cast<ObserverContainer<TMessage>>(itFamily->second);

        if (pObserverContainer != nullptr) {
            pObserverContainer->add(typedObserver);
            typedObserver.addObservable(*this);
        }
    }

    template <typename TTypedObserver, typename... TObserver>
    void Observable::attach(TTypedObserver &typedObserver, TObserver &...observer) {
        attach(typedObserver);
        attach(observer...);
    }

    template <typename... TMessages>
    void Observable::attachAll(Observer<TMessages...> &observer) {
        attach<TypedObserver<TMessages>...>(dynamic_cast<TypedObserver<TMessages> &>(observer)...);
    }

    template <typename TObserver>
    void Observable::attachAll(TObserver &observer) {
        using TMessage = typename TObserver::Head::Type;
        attach<TMessage>(static_cast<typename TObserver::Head &>(observer));
        attachAll(static_cast<typename TObserver::Queue &>(observer));
    }

    template <typename TMessage>
    void Observable::detach(TypedObserver<TMessage> &observer) {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        removeTypedObserver(observer);
        removeFromTypedObserver(observer);
    }

    template <typename TTypedObserver, typename... TObserver>
    void Observable::detach(TTypedObserver &typedObserver, TObserver &...observer) {
        detach(observer...);
        detach(typedObserver);
    }

    template <typename... T>
    void Observable::detach() {
    }

    template <typename... TMessages>
    void Observable::detachAll(Observer<TMessages...> &observer) {
        detach<TypedObserver<TMessages>...>(dynamic_cast<TypedObserver<TMessages> &>(observer)...);
    }

    template <typename TObserver>
    void Observable::detachAll(TObserver &observer) {
        using TMessage = typename TObserver::Head::Type;
        detachAll(static_cast<typename TObserver::Queue &>(observer));
        detach<TMessage>(static_cast<typename TObserver::Head &>(observer));
    }

    template <typename TMessage>
    void Observable::notify(const TMessage &msg) {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        auto pObserverContainer = getObserverContainer<TMessage>();
        if (pObserverContainer == nullptr)
            return;

        // While notifying, some observers can be removed
        // To handle this feature:
        //  - duplicate the current list of observers to avoid dirty iterators
        //  - get the list of weak markers to check if the observer is not expired
        const typename ObserverContainer<TMessage>::Observers observers = pObserverContainer->getObservers(); // create a copy of observers
        std::vector<typename TypedObserver<TMessage>::WeakMarker> weakMarkers;
        for (auto &&pObserver : observers) {
            weakMarkers.push_back(pObserver->getWeakMarker());
        }

        auto itWeakMarker = weakMarkers.cbegin();
        for (auto pObserver : observers) {
            if (!itWeakMarker->expired()) {
                pObserver->update(*this, msg);
            }

            ++itWeakMarker;
        }
    }

    template <typename TMessage>
    void Observable::removeTypedObserver(TypedObserver<TMessage> &typedObserver) {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        auto pObserverContainer = getObserverContainer<TMessage>();
        if (pObserverContainer == nullptr)
            return;

        pObserverContainer->remove(typedObserver);
        if (pObserverContainer->isEmpty()) {
            const std::type_index indexFamily(typeid(TMessage));
            ObserverFamilies::iterator itFamily = m_observerFamilies.find(indexFamily);
            m_observerFamilies.erase(itFamily);
        }
    }

    template <typename TMessage>
    void Observable::removeFromTypedObserver(TypedObserver<TMessage> &typedObserver) {
        typedObserver.removeObservable(*this);
    }

    template <typename TMessage>
    std::shared_ptr<typename Observable::ObserverContainer<TMessage>> Observable::getObserverContainer() const {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        const std::type_index indexFamily(typeid(TMessage));
        auto const itFamily = m_observerFamilies.find(indexFamily);

        if (itFamily == m_observerFamilies.cend())
            return nullptr;

        auto pObserverContainer = std::dynamic_pointer_cast<ObserverContainer<TMessage>>(itFamily->second);
        return pObserverContainer;
    }

    /*
     * \class Observable::ObserverContainer
     */
    template <typename TMessage>
    Observable::ObserverContainer<TMessage>::ObserverContainer(Observable &observable) : m_observable(observable) {
    }

    template <typename TMessage>
    Observable::ObserverContainer<TMessage>::~ObserverContainer() {
        for (auto &&pTypedObserver : m_observers)
            pTypedObserver->removeObservable(m_observable);
    }

    template <typename TMessage>
    void Observable::ObserverContainer<TMessage>::add(TypedObserver<TMessage> &observer) {
        if (std::find_if(m_observers.begin(), m_observers.end(), [&observer](auto pObserver) { return &observer == pObserver; }) ==
            m_observers.end()) {
            m_observers.push_back(&observer);
        }
    }

    template <typename TMessage>
    void Observable::ObserverContainer<TMessage>::remove(TypedObserver<TMessage> &observer) {
        m_observers.erase(
            std::remove_if(m_observers.begin(), m_observers.end(), [&observer](auto pObserver) { return &observer == pObserver; }),
            m_observers.end());
    }

    template <typename TMessage>
    size_t Observable::ObserverContainer<TMessage>::getCount() const {
        return m_observers.count();
    }

    template <typename TMessage>
    bool Observable::ObserverContainer<TMessage>::isEmpty() const {
        return m_observers.empty();
    }

    template <typename TMessage>
    const typename Observable::ObserverContainer<TMessage>::Observers &Observable::ObserverContainer<TMessage>::getObservers() const {
        return m_observers;
    }

    template <typename TMessage>
    void Observable::ObserverContainer<TMessage>::attachAll(Observable &observable) {
        for (auto &&pTypedObserver : m_observers) {
            observable.attach<TMessage>(*pTypedObserver);
        }
    }

    template <typename TMessage>
    void Observable::ObserverContainer<TMessage>::detachAll(Observable &observable) {
        for (auto &&pTypedObserver : m_observers) {
            observable.detach<TMessage>(*pTypedObserver);
        }
    }

} // namespace cho::osbase::core
