// \file  AbstractFactory.h
// \brief Declaration of classes for the pattern Abstract Factory

#pragma once

#include "FactoryManager.h"
#include "RegisterFactoryGuard.h"
#include "osCore/Misc/MacroHelpers.h"

/**
 * \defgroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY Abstract Factory
 *
 * \brief Contains classes that realizes the pattern abstract factory
 *
 * Provide an interface for creating families of related or dependent objects without specifying their concrete classes.\n
 * More information at http://www.dofactory.com/net/abstract-factory-design-pattern
 * \image html abstractfactory.png "UML Diagram"
 * This generic implementation allows the instantiation of concrete realizations without knowing them (aka without
 * including their related header)
 * \n
 * \par Registering a concrete realization
 * To register a concrete realization, add the following lines in the core implementation (cpp file):
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 #include "osCore/DesignPattern/AbstractFactory.h
 ...
 OS_REGISTER_FACTORY(MyBaseClass, MyConcreteRealization, "my identifier" [, ctor args]);
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * \n
 * Where:
 *  -   MyBaseClass : base class (or familiy) of the concrete realizations (or products)
 *  -   MyConcreteRealization : concrete realization (or product)
 *  -   "my identifier" : unique string that identifies the concrete realization
 *  -   [, ctor args] : argument to pass (by value) to the instantiation
 *
 * \par Instantiating a concrete realization
 * To instantiate a concrete realization, add the following lines:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 #include "osCore/DesignPattern/AbstractFactory.h
 ...
 using namespace nscore = cho::osbase::core;
 ...
 auto pConcreteRealization = nscore::TheFactoryManager.createInstance<MyBaseClass>("My identifier" [, ctor args])
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * \n
 * Where:
 *  -   MyBaseClass : base class (or familiy) of the concrete realizations (or products)
 *  -   "my identifier" : unique string that identifies the concrete realization
 *  -   [, ctor args] : argument to pass (by value) to the instantiation
 *
 * \ingroup PACKAGE_DESIGNPATTERN
 */

/**
 * \brief Registering of a factory
 * \param[in]	_baseClass      Base class
 * \param[in]	_concreteClass  Concrete class
 *
 * \remark	    for the ellipsis parameters:
 *              - 1st paramater: concrete name
 *              - other paramaters: ctor args
 * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
 */
#define OS_REGISTER_FACTORY(_baseClass, _concreteClass, ...) OS_REGISTER_FACTORY_N(_baseClass, _concreteClass, __COUNTER__, __VA_ARGS__)

/**
 * \copydoc OS_REGISTER_FACTORY
 * \param[in]	_index		    Index of the factory (used to call different ctors for a same concrete class
 * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
 */
#define OS_REGISTER_FACTORY_N(_baseClass, _concreteClass, _index, ...)                                                                     \
    OS_REGISTER_FACTORY_EXPAND(OS_REGISTER_FACTORY_MACRO(__VA_ARGS__)(_baseClass, _concreteClass, _index, __VA_ARGS__))

/**
 * \brief Macro mandatory in caller createInstance for concrete factories declared in static libraries
 * \param[in]	_baseClass      Base class
 * \param[in]	_concreteClass  Concrete class
 * \param[in]	_index		    Index of the factory (used to call different ctors for a same concrete class
 * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
 */
#define OS_LINK_FACTORY_N(_baseClass, _concreteClass, _index)                                                                              \
    extern OSBASECORE_NS::RegisterFactoryGuard OS_VARIABLE_FACTORY_LINKER(_baseClass, _concreteClass, _index);                             \
    bool OS_STATIC_JOIN(b, OS_VARIABLE_FACTORY_LINKER(_baseClass, _concreteClass, _index)) =                                               \
        OS_VARIABLE_FACTORY_LINKER(_baseClass, _concreteClass, _index).link();

/** \cond */
#define OSBASECORE_NS cho::osbase::core
#define OS_VARIABLE_FACTORY_LINKER(_baseClass, _concreteClass, _index)                                                                     \
    OS_STATIC_JOIN(g_variable_factory_linker_, OS_STATIC_JOIN(_baseClass, OS_STATIC_JOIN(_, OS_STATIC_JOIN(_concreteClass, _index))))

#define OS_REGISTER_FACTORY_EXPAND(_x) _x
#define OS_REGISTER_FACTORY_SEQ()                                                                                                          \
    _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS,    \
        _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAMEARGS, _NAME
#define OS_REGISTER_FACTORY_ARGN(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _n, ...) _n

#define OS_REGISTER_FACTORY_NAME_OR_NAMEARGS(...)  OS_REGISTER_FACTORY_NAME_OR_NAMEARGS_(__VA_ARGS__, OS_REGISTER_FACTORY_SEQ())
#define OS_REGISTER_FACTORY_NAME_OR_NAMEARGS_(...) OS_REGISTER_FACTORY_EXPAND(OS_REGISTER_FACTORY_ARGN(__VA_ARGS__))
#define OS_REGISTER_FACTORY_MACRO(...)                                                                                                     \
    OS_REGISTER_FACTORY_EXPAND(OS_STATIC_JOIN(OS_REGISTER_FACTORY, OS_REGISTER_FACTORY_NAME_OR_NAMEARGS(__VA_ARGS__)))

#define OS_REGISTER_FACTORY_NAME(_baseClass, _concreteClass, _index, _concreteName)                                                        \
    OSBASECORE_NS::RegisterFactoryGuard OS_VARIABLE_FACTORY_LINKER(_baseClass, _concreteClass, _index) =                                   \
        OSBASECORE_NS::RegisterFactoryGuard::create<_baseClass, _concreteClass>(_concreteName);
#define OS_REGISTER_FACTORY_NAMEARGS(_baseClass, _concreteClass, _index, _concreteName, ...)                                               \
    OSBASECORE_NS::RegisterFactoryGuard OS_VARIABLE_FACTORY_LINKER(_baseClass, _concreteClass, _index) =                                   \
        OSBASECORE_NS::RegisterFactoryGuard::create<_baseClass, _concreteClass, __VA_ARGS__>(_concreteName);
/** \endcond */