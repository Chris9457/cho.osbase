// \file  MyObserver.h
// \brief For unit tests

#include "Static/MyObserver.h"

namespace NS_OSBASE::core::ut {

    /*
     * \class MyObservable
     */
    void MyObservable::notifyMessage1(const TMessage1 &msg) {
        notify(msg);
    }

    void MyObservable::notifyMessage2(const TMessage2 &msg) {
        notify(msg);
    }

    void MyObservable::notifyMessage3(const TMessage3 &msg) {
        notify(msg);
    }

    MyObservable::TMessage1::TMessage1(int value) : m_value(value) {
    }
    int MyObservable::TMessage1::getValue() const {
        return m_value;
    }

    MyObservable::TMessage2::TMessage2(const std::string &name) : m_name(name) {
    }
    const std::string &MyObservable::TMessage2::getName() const {
        return m_name;
    }

    MyObservable::TMessage3::TMessage3(double value) : m_value(value) {
    }
    double MyObservable::TMessage3::getValue() const {
        return m_value;
    }

    /*
     * \class MyObserver1
     */
    int MyObserver1::getMessage1LastValue() const {
        return m_message1LastValue;
    }
    const std::string &MyObserver1::getMessage2LastName() const {
        return m_message2LastName;
    }

    void MyObserver1::update(const Observable &, const MyObservable::TMessage1 &msg) {
        m_message1LastValue = msg.getValue();
    }

    void MyObserver1::update(const Observable &, const MyObservable::TMessage2 &msg) {
        m_message2LastName = msg.getName();
    }

    /*
     * \class MyObserver2
     */
    int MyObserver2::getMessage1LastValue() const {
        return m_message1LastValue;
    }
    double MyObserver2::getMessage3LastValue() const {
        return m_message3LastValue;
    }

    void MyObserver2::update(const Observable &, const MyObservable::TMessage1 &msg) {
        m_message1LastValue = msg.getValue();
    }
    void MyObserver2::update(const Observable &, const MyObservable::TMessage3 &msg) {
        m_message3LastValue = msg.getValue();
    }

    /*
     * \class MyObserver_delete_an_other_observer
     */
    void MyObserverDeleteAnOtherObserver::setOtherObserver(TypedObserver<MyObservable::TMessage1> *observer) {
        m_observer = observer;
    }

    int MyObserverDeleteAnOtherObserver::getMessage1LastValue() const {
        return m_message1LastValue;
    }

    void MyObserverDeleteAnOtherObserver::update(const Observable &, const MyObservable::TMessage1 &msg) {
        delete m_observer;
        m_observer          = nullptr;
        m_message1LastValue = msg.getValue();
    }

    /*
     * \class MyObserverSimple
     */
    int MyObserverSimple::getMessage1LastValue() const {
        return m_message1LastValue;
    }

    void MyObserverSimple::update(const Observable &, const MyObservable::TMessage1 &msg) {
        m_message1LastValue = msg.getValue();
    }

    /*
     * \class MyObserverDetachAnOtherObserver
     */
    void MyObserverDetachAnOtherObserver::setOtherObserver(Observer<MyObservable::TMessage1> &observer) {
        m_observer = &observer;
    }
    void MyObserverDetachAnOtherObserver::resetOtherObserver() {
        m_observer = nullptr;
    }

    void MyObserverDetachAnOtherObserver::setObservable(Observable &observable) {
        m_observable = &observable;
    }

    int MyObserverDetachAnOtherObserver::getMessage1LastValue() const {
        return m_message1LastValue;
    }

    void MyObserverDetachAnOtherObserver::update(const Observable &, const MyObservable::TMessage1 &msg) {
        if (m_observable && m_observer)
            m_observable->detach<MyObservable::TMessage1>(*m_observer);
        m_message1LastValue = msg.getValue();
    }

    /*
     * \class MyObserverDetachAnOtherObserver
     */
    void MyObserverAttachAnOtherObserver::setOtherObserver(Observer<MyObservable::TMessage1> &observer) {
        m_observer = &observer;
    }
    void MyObserverAttachAnOtherObserver::resetOtherObserver() {
        m_observer = nullptr;
    }

    void MyObserverAttachAnOtherObserver::setObservable(Observable &observable) {
        m_observable = &observable;
    }

    int MyObserverAttachAnOtherObserver::getMessage1LastValue() const {
        return m_message1LastValue;
    }

    void MyObserverAttachAnOtherObserver::update(const Observable &, const MyObservable::TMessage1 &msg) {
        if (m_observable && m_observer)
            m_observable->attach<MyObservable::TMessage1>(*m_observer);
        m_message1LastValue = msg.getValue();
    }

    /*
     * \class MyObserverWaitOnUpdate
     */
    int MyObserverWaitOnUpdate::getCount() const {
        return m_count;
    }

    void MyObserverWaitOnUpdate::update(const Observable &, const MyObservable::TMessage1 &) {
        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_count;
        if (m_count == 1)
            m_cv.wait(lock);
    }

    void MyObserverWaitOnUpdate::go() {
        m_cv.notify_all();
    }
} // namespace NS_OSBASE::core::ut