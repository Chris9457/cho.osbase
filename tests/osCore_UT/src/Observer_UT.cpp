// \brief Unit test of the design pattern Observer

#include "Static/MyObserver.h"
#include "gtest/gtest.h"
#include <future>

namespace NS_OSBASE::core::ut {

    class Observer_UT : public testing::Test {};

    TEST_F(Observer_UT, notify) {
        MyObservable myObservable;
        MyObserver1 myObserver1;
        MyObserver2 myObserver2;

        myObservable.attachAll(myObserver1);
        myObservable.attachAll(myObserver2);

        myObservable.notifyMessage1(MyObservable::TMessage1(57));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 57);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 57);

        myObservable.notifyMessage2(MyObservable::TMessage2("test57"));
        ASSERT_EQ(myObserver1.getMessage2LastName(), "test57");

        myObservable.notifyMessage3(MyObservable::TMessage3(68.));
        ASSERT_EQ(myObserver2.getMessage3LastValue(), 68.);

        myObservable.detach<MyObservable::TMessage1>(myObserver2);
        myObservable.notifyMessage1(MyObservable::TMessage1(29));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 29);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 57);

        myObservable.detachAll(myObserver1);
        myObservable.detachAll(myObserver2);
    }

    TEST_F(Observer_UT, deleteAnObserverWhileANotify) {
        MyObservable myObservable;
        MyObserverDeleteAnOtherObserver myObserver1;
        myObservable.attachAll(myObserver1);

        MyObserverSimple *myObserver2(new MyObserverSimple);

        myObserver1.setOtherObserver(myObserver2);
        myObservable.attachAll(*myObserver2);

        myObservable.notifyMessage1(MyObservable::TMessage1(1));
        ASSERT_EQ(1, myObserver1.getMessage1LastValue());
    }

    TEST_F(Observer_UT, deleteAnObserverMultipleWhileANotify) {
        MyObservable myObservable;
        MyObserverDeleteAnOtherObserver myObserver1;
        myObservable.attachAll(myObserver1);

        MyObserver1 *myObserver2 = new MyObserver1;

        myObserver1.setOtherObserver(myObserver2);
        myObservable.attachAll(*myObserver2);

        myObservable.notifyMessage2(MyObservable::TMessage2("test57"));
        ASSERT_EQ(myObserver2->getMessage2LastName(), "test57");

        myObservable.notifyMessage1(MyObservable::TMessage1(1)); // myObserver2 is removed during the notification
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 1);

        myObservable.notifyMessage2(MyObservable::TMessage2("test00"));
    }

    TEST_F(Observer_UT, copyObserver) {
        MyObservable myObservable;

        MyObserverSimple *myObserver1 = new MyObserverSimple;
        myObservable.attachAll(*myObserver1);

        {
            MyObserverSimple myObserver2 = *myObserver1;

            myObservable.notifyMessage1(MyObservable::TMessage1(1));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 1);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 1);
        }

        myObservable.notifyMessage1(MyObservable::TMessage1(2));
        ASSERT_EQ(myObserver1->getMessage1LastValue(), 2);

        {
            MyObserverSimple myObserver2 = *myObserver1;

            myObservable.notifyMessage1(MyObservable::TMessage1(3));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 3);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 3);

            delete myObserver1;
            myObservable.notifyMessage1(MyObservable::TMessage1(4));
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 4);
        }

        myObservable.notifyMessage1(MyObservable::TMessage1(5));
    }

    TEST_F(Observer_UT, assignObserver) {
        MyObservable myObservable1, myObservable2;

        MyObserverSimple *myObserver1 = new MyObserverSimple;
        myObservable1.attachAll(*myObserver1);
        myObservable2.attachAll(*myObserver1);

        MyObserverSimple myObserver2;

        myObservable1.notifyMessage1(MyObservable::TMessage1(1));
        ASSERT_EQ(myObserver1->getMessage1LastValue(), 1);
        ASSERT_NE(myObserver2.getMessage1LastValue(), 1);

        myObservable2.notifyMessage1(MyObservable::TMessage1(2));
        ASSERT_EQ(myObserver1->getMessage1LastValue(), 2);
        ASSERT_NE(myObserver2.getMessage1LastValue(), 2);

        myObserver2 = *myObserver1;

        myObservable1.notifyMessage1(MyObservable::TMessage1(1));
        ASSERT_EQ(myObserver1->getMessage1LastValue(), 1);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 1);

        myObservable2.notifyMessage1(MyObservable::TMessage1(2));
        ASSERT_EQ(myObserver1->getMessage1LastValue(), 2);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 2);

        delete myObserver1;
        myObservable1.notifyMessage1(MyObservable::TMessage1(3));
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 3);

        myObservable2.notifyMessage1(MyObservable::TMessage1(4));
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 4);
    }

    TEST_F(Observer_UT, sameObserverAttachObservables) {
        MyObservable myObservable1;

        MyObserverSimple myObserver;
        myObservable1.attachAll(myObserver);

        {
            MyObservable myObservable2;
            myObservable2.attachAll(myObserver);

            myObservable1.notifyMessage1(MyObservable::TMessage1(1));
            ASSERT_EQ(myObserver.getMessage1LastValue(), 1);
            myObservable2.notifyMessage1(MyObservable::TMessage1(2));
            ASSERT_EQ(myObserver.getMessage1LastValue(), 2);

            myObservable1.detachAll(myObserver);
            myObservable1.notifyMessage1(MyObservable::TMessage1(3));
            ASSERT_NE(myObserver.getMessage1LastValue(), 3);
            myObservable2.notifyMessage1(MyObservable::TMessage1(4));
            ASSERT_EQ(myObserver.getMessage1LastValue(), 4);

            myObservable1.attachAll(myObserver);
            myObservable1.notifyMessage1(MyObservable::TMessage1(5));
            ASSERT_EQ(myObserver.getMessage1LastValue(), 5);
            myObservable2.notifyMessage1(MyObservable::TMessage1(6));
            ASSERT_EQ(myObserver.getMessage1LastValue(), 6);

            {
                MyObserverSimple myObserver2;
                myObservable1.attachAll(myObserver2);
                myObservable2.attachAll(myObserver2);
                myObservable1.notifyMessage1(MyObservable::TMessage1(7));
                ASSERT_EQ(myObserver.getMessage1LastValue(), 7);
                ASSERT_EQ(myObserver2.getMessage1LastValue(), 7);
                myObservable2.notifyMessage1(MyObservable::TMessage1(8));
                ASSERT_EQ(myObserver.getMessage1LastValue(), 8);
                ASSERT_EQ(myObserver2.getMessage1LastValue(), 8);
            }

            myObservable1.notifyMessage1(MyObservable::TMessage1(9));
            ASSERT_EQ(myObserver.getMessage1LastValue(), 9);
            myObservable2.notifyMessage1(MyObservable::TMessage1(10));
            ASSERT_EQ(myObserver.getMessage1LastValue(), 10);
        }

        myObservable1.notifyMessage1(MyObservable::TMessage1(7));
        ASSERT_EQ(myObserver.getMessage1LastValue(), 7);
    }

    TEST_F(Observer_UT, attachAnObserverWhileANotify) {
        MyObservable myObservable;
        MyObserverAttachAnOtherObserver myObserver1;
        myObserver1.setObservable(myObservable);
        myObservable.attachAll(myObserver1);

        MyObserverSimple myObserver2;
        myObserver1.setOtherObserver(myObserver2);

        // attach the observer myObserver2 during the notification
        // check that the attachment is take into acount AFTER the notification
        myObservable.notifyMessage1(MyObservable::TMessage1(1));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 1);
        ASSERT_NE(myObserver2.getMessage1LastValue(), 1);

        // Next notification => attachement taken into account
        myObserver1.resetOtherObserver();
        myObservable.notifyMessage1(MyObservable::TMessage1(2));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 2);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 2);

        myObserver1.setOtherObserver(myObserver2);
        myObservable.notifyMessage1(MyObservable::TMessage1(3));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 3);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 3);
    }

    TEST_F(Observer_UT, detachAnObserverWhileANotify) {
        MyObservable myObservable;
        MyObserverDetachAnOtherObserver myObserver1;
        myObserver1.setObservable(myObservable);
        myObservable.attachAll(myObserver1);

        MyObserverSimple myObserver2;
        myObservable.attachAll(myObserver2);

        myObservable.notifyMessage1(MyObservable::TMessage1(1));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 1);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 1);

        myObserver1.setOtherObserver(myObserver2);
        myObservable.notifyMessage1(MyObservable::TMessage1(2));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 2);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 2);

        myObserver1.resetOtherObserver();
        myObservable.notifyMessage1(MyObservable::TMessage1(3));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 3);
        ASSERT_NE(myObserver2.getMessage1LastValue(), 3);

        myObserver1.setOtherObserver(myObserver2);
        myObservable.notifyMessage1(MyObservable::TMessage1(4));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 4);
        ASSERT_NE(myObserver2.getMessage1LastValue(), 4);

        myObservable.detachAll(myObserver1);
        myObservable.attachAll(myObserver2);
        myObservable.attachAll(myObserver1);

        myObservable.notifyMessage1(MyObservable::TMessage1(5));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 5);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 5);

        myObserver1.resetOtherObserver();
        myObservable.notifyMessage1(MyObservable::TMessage1(6));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 6);
        ASSERT_NE(myObserver2.getMessage1LastValue(), 6);
    }

    TEST_F(Observer_UT, sameObserverAttachObservablesAndDetachAnObserverWhileANotify) {
        MyObservable myObservable1;
        MyObserverDetachAnOtherObserver myObserver1;
        myObserver1.setObservable(myObservable1);
        myObservable1.attachAll(myObserver1);

        MyObserverSimple myObserver2;
        myObservable1.attachAll(myObserver2);
        myObserver1.setOtherObserver(myObserver2);

        MyObservable myObservable2;
        myObservable2.attachAll(myObserver1);
        myObservable2.attachAll(myObserver2);

        myObservable1.notifyMessage1(MyObservable::TMessage1(1));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 1);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 1);

        myObservable2.notifyMessage1(MyObservable::TMessage1(2));
        ASSERT_EQ(myObserver1.getMessage1LastValue(), 2);
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 2);
    }

    TEST_F(Observer_UT, copyObservable) {
        MyObservable myObservable1;

        MyObserverSimple *myObserver1 = new MyObserverSimple;
        MyObserverSimple myObserver2;

        myObservable1.attachAll(*myObserver1);
        myObservable1.attachAll(myObserver2);

        {
            MyObservable myObservable2 = myObservable1;

            myObservable1.notifyMessage1(MyObservable::TMessage1(1));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 1);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 1);
            myObservable2.notifyMessage1(MyObservable::TMessage1(2));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 2);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 2);

            myObservable1.detachAll(*myObserver1);
            myObservable1.notifyMessage1(MyObservable::TMessage1(3));
            ASSERT_NE(myObserver1->getMessage1LastValue(), 3);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 3);
            myObservable2.notifyMessage1(MyObservable::TMessage1(4));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 4);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 4);

            delete myObserver1;
            myObservable1.notifyMessage1(MyObservable::TMessage1(5));
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 5);

            myObservable2.notifyMessage1(MyObservable::TMessage1(6));
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 6);
        }

        myObservable1.notifyMessage1(MyObservable::TMessage1(7));
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 7);
    }

    TEST_F(Observer_UT, assignObservable) {
        MyObservable myObservable1;

        MyObserverSimple *myObserver1 = new MyObserverSimple;
        MyObserverSimple myObserver2;

        myObservable1.attachAll(*myObserver1);
        myObservable1.attachAll(myObserver2);

        {
            MyObservable myObservable2;
            myObservable2.attachAll(*myObserver1);
            myObservable2.notifyMessage1(MyObservable::TMessage1(1));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 1);
            ASSERT_NE(myObserver2.getMessage1LastValue(), 1);

            myObservable2 = myObservable1;
            myObservable1.notifyMessage1(MyObservable::TMessage1(2));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 2);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 2);
            myObservable2.notifyMessage1(MyObservable::TMessage1(3));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 3);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 3);

            myObservable1.detachAll(*myObserver1);
            myObservable1.notifyMessage1(MyObservable::TMessage1(4));
            ASSERT_NE(myObserver1->getMessage1LastValue(), 4);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 4);
            myObservable2.notifyMessage1(MyObservable::TMessage1(5));
            ASSERT_EQ(myObserver1->getMessage1LastValue(), 5);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 5);

            delete myObserver1;
            myObservable1.notifyMessage1(MyObservable::TMessage1(6));
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 6);
            myObservable2.notifyMessage1(MyObservable::TMessage1(7));
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 7);
        }

        myObservable1.notifyMessage1(MyObservable::TMessage1(8));
        ASSERT_EQ(myObserver2.getMessage1LastValue(), 8);
    }

    TEST_F(Observer_UT, moveCopyObserver) {
        MyObservable myObservable;

        MyObserverSimple *myObserver1 = new MyObserverSimple;
        myObservable.attachAll(*myObserver1);

        {
            MyObserverSimple myObserver2 = std::move(*myObserver1);

            myObservable.notifyMessage1(MyObservable::TMessage1(1));
            ASSERT_NE(myObserver1->getMessage1LastValue(), 1);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 1);
        }

        myObservable.notifyMessage1(MyObservable::TMessage1(2));
        ASSERT_NE(myObserver1->getMessage1LastValue(), 2);
    }

    TEST_F(Observer_UT, moveAssignObserver) {
        MyObservable myObservable;

        MyObserverSimple *myObserver1 = new MyObserverSimple;
        myObservable.attachAll(*myObserver1);

        {
            MyObserverSimple myObserver2;
            myObserver2 = std::move(*myObserver1);

            myObservable.notifyMessage1(MyObservable::TMessage1(1));
            ASSERT_NE(myObserver1->getMessage1LastValue(), 1);
            ASSERT_EQ(myObserver2.getMessage1LastValue(), 1);
        }

        myObservable.notifyMessage1(MyObservable::TMessage1(2));
        ASSERT_NE(myObserver1->getMessage1LastValue(), 2);
    }

    TEST_F(Observer_UT, observableNotifyThreadSafe) {
        MyObservable myObservable;
        MyObserverWaitOnUpdate myObserver;

        myObservable.attachAll(myObserver);

        using namespace std::placeholders;
        auto notify = [&](const MyObservable::TMessage1 &msg) { myObservable.notifyMessage1(msg); };
        const MyObservable::TMessage1 msg(1);
        auto fut1 = std::async(notify, msg);
        auto fut2 = std::async(notify, msg);

        using namespace std::literals::chrono_literals;
        ASSERT_EQ(std::future_status::timeout, fut1.wait_for(1s));
        ASSERT_EQ(std::future_status::timeout, fut2.wait_for(1s));
        ASSERT_EQ(1, myObserver.getCount());

        myObserver.go();
        ASSERT_NE(std::future_status::timeout, fut1.wait_for(1s));
        myObserver.go();
        ASSERT_NE(std::future_status::timeout, fut2.wait_for(1s));
        ASSERT_EQ(2, myObserver.getCount());
    }
} // namespace NS_OSBASE::core::ut
