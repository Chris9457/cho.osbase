// \file  Singleton.h
// \brief Declaration of the class Singleton

#pragma once
#include "osCore/Interprocess/ProcessPID.h"
#include "osCore/Interprocess/SharedMemory.h"
#include "osCore/Misc/NonCopyable.h"
#include <mutex>
#include <sstream>

/**
 * \defgroup PACKAGE_DESIGNPATTERN_SINGLETON Singleton
 *
 * \brief Contains classes that realizes the pattern singleton
 *
 * Ensure a class has only one instance and provide a global point of access to it.
 *
 * More information at http://www.dofactory.com/net/singleton-design-pattern
 *
 * This generic implementation provides the following features :
 *	-   In a multi-tasking application, the creation of the first instance of a singleton must be thread-safe
 *	-   One and only one instance maximum must be present in the whole application, even if the implementation of
 *      the singleton is encapsulated in a sub-module (such as a dll). Indeed, in the case when a singleton is
 *      implemented in a static library, there are as many singletons
 *      as modules that include this library.
 *	-   A singleton can be instantiate at any time, even during the instantiation step of the global and static
 *      variables (before executing the function main()). Indeed during this step the order of the creation of
 *      global and static variables is not predictable, thus depending on the implementation of the singleton
 *      the instance of the singleton can be returned before the creation of this last one.
 *	-   A singleton is deleted only by the application, forbidden for other.
 * \n
 * \n
 * To create a class singleton, this last one must inherit from the class Singleton like this :
 *
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 class ConcreteSingleton : public Singleton<ConcreteSingleton>             // 1
 {
         friend class Singleton<ConcreteSingleton>;                        // 2

 public:
         void foo()
         ...

 private:
         ConcreteSingleton ();                                             // 3
         ~ConcreteSingleton ();
 };
 #define TheConcreteSingleton ConcreteSingleton::getInstance()             // 4
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * -#   inheritance from the class Singleton to gain access to all constraints
 *      and methods realized by this base-class
 * -#   necessary because the private ctor & dtor of the class ConcreteSingleton are called by its base-class
 * -#   the ctor & dtor of the class ConcreteSingleton are in private access to avoid (illegal) constructions /
 *      destructions
 * -#   not necessary but allows to write methods' invocation like this :
 *     TheConcreteSingleton.foo();
 * Instead of
 *     ConcreteSingleton::getInstance().foo();
 *
 * \image html singleton.png "UML Diagram"
 * \ingroup PACKAGE_DESIGNPATTERN
 */

namespace NS_OSBASE::core {

    /**
     * \brief Base class for all singletons
     * \ingroup PACKAGE_DESIGNPATTERN_SINGLETON
     */
    template <class T>
    class Singleton : private NonCopyable {
    public:
        static T &getInstance(); //!< Return the instance of the singleton

    protected:
        virtual ~Singleton();

    private:
        class LinkedInstance : public SharedMemory<T *> {
        public:
            LinkedInstance(const std::string &name);
        };

        static std::recursive_mutex &getLock();
        static const std::string &getInstanceName();

        static std::recursive_mutex *m_pDummyLock;
    };

} // namespace NS_OSBASE::core

/** \cond */
#include "Singleton.inl"
/** \endcond */
