// \file  MyObserver.h
// \brief For unit tests

#pragma once
#include "osCore/DesignPattern/Observer.h"
#include <condition_variable>
#include <mutex>

namespace NS_OSBASE::core::ut {

    /**
     * \class MyObservable
     */
    class MyObservable : public NS_OSBASE::core::Observable {
    public:
        class TMessage1 {
        public:
            TMessage1(int value);
            int getValue() const;

        private:
            int m_value;
        };

        class TMessage2 {
        public:
            TMessage2(const std::string &name);
            const std::string &getName() const;

        private:
            std::string m_name;
        };

        class TMessage3 {
        public:
            TMessage3(double value);
            double getValue() const;

        private:
            double m_value;
        };

        void notifyMessage1(const TMessage1 &msg);
        void notifyMessage2(const TMessage2 &msg);
        void notifyMessage3(const TMessage3 &msg);
    };

    /**
     * \class MyObserver1
     */
    class MyObserver1 : public NS_OSBASE::core::Observer<MyObservable::TMessage1, MyObservable::TMessage2> {
    public:
        int getMessage1LastValue() const;
        const std::string &getMessage2LastName() const;

        void update(const NS_OSBASE::core::Observable &rObservable, const MyObservable::TMessage1 &msg) override;
        void update(const NS_OSBASE::core::Observable &rObservable, const MyObservable::TMessage2 &msg) override;

    private:
        int m_message1LastValue = 0;
        std::string m_message2LastName;
    };

    /**
     * \class MyObserver2
     */
    class MyObserver2 : public NS_OSBASE::core::Observer<MyObservable::TMessage1, MyObservable::TMessage3> {
    public:
        int getMessage1LastValue() const;
        double getMessage3LastValue() const;

        void update(const NS_OSBASE::core::Observable &rObservable, const MyObservable::TMessage1 &msg) override;
        void update(const NS_OSBASE::core::Observable &rObservable, const MyObservable::TMessage3 &msg) override;

    private:
        int m_message1LastValue    = 0;
        double m_message3LastValue = 0.;
    };

    /**
     * \class MyObserverDeleteAnOtherObserver
     */
    class MyObserverDeleteAnOtherObserver : public NS_OSBASE::core::Observer<MyObservable::TMessage1> {
    public:
        void setOtherObserver(NS_OSBASE::core::TypedObserver<MyObservable::TMessage1> *observer);

        int getMessage1LastValue() const;

        void update(const NS_OSBASE::core::Observable &rObservable, const MyObservable::TMessage1 &msg) override;

    private:
        NS_OSBASE::core::TypedObserver<MyObservable::TMessage1> *m_observer = nullptr;
        int m_message1LastValue                                                      = 0;
    };

    /**
     * \class MyObserverSimple
     */
    class MyObserverSimple : public NS_OSBASE::core::Observer<MyObservable::TMessage1> {
    public:
        int getMessage1LastValue() const;

        void update(const NS_OSBASE::core::Observable &rObservable, const MyObservable::TMessage1 &msg) override;

    private:
        int m_message1LastValue = 0;
    };

    /**
     * \class MyObserverDetachAnOtherObserver
     */
    class MyObserverDetachAnOtherObserver : public NS_OSBASE::core::Observer<MyObservable::TMessage1> {
    public:
        void setOtherObserver(NS_OSBASE::core::Observer<MyObservable::TMessage1> &observer);
        void resetOtherObserver();

        void setObservable(NS_OSBASE::core::Observable &observable);

        int getMessage1LastValue() const;

        void update(const NS_OSBASE::core::Observable &rObservable, const MyObservable::TMessage1 &msg) override;

    private:
        NS_OSBASE::core::Observer<MyObservable::TMessage1> *m_observer = nullptr;
        NS_OSBASE::core::Observable *m_observable                      = nullptr;
        int m_message1LastValue                                                 = 0;
    };

    /**
     * \class MyObserverAttachAnOtherObserver
     */
    class MyObserverAttachAnOtherObserver : public NS_OSBASE::core::Observer<MyObservable::TMessage1> {
    public:
        void setOtherObserver(NS_OSBASE::core::Observer<MyObservable::TMessage1> &observer);
        void resetOtherObserver();

        void setObservable(NS_OSBASE::core::Observable &observable);

        int getMessage1LastValue() const;

        void update(const NS_OSBASE::core::Observable &rObservable, const MyObservable::TMessage1 &msg) override;

    private:
        NS_OSBASE::core::Observable *m_observable                      = nullptr;
        NS_OSBASE::core::Observer<MyObservable::TMessage1> *m_observer = nullptr;
        int m_message1LastValue                                                 = 0;
    };

    /**
     * \class MyObserverWaitOnUpdate
     */
    class MyObserverWaitOnUpdate : public NS_OSBASE::core::Observer<MyObservable::TMessage1> {
    public:
        int getCount() const;
        void update(const NS_OSBASE::core::Observable &, const MyObservable::TMessage1 &msg) override;
        void go();

    private:
        int m_count = 0;
        std::mutex m_mutex;
        std::condition_variable m_cv;
    };
} // namespace NS_OSBASE::core::ut