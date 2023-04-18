// \file  Factory.h
// \brief Declaration of classes Factory

#pragma once
#include <memory>

namespace cho::osbase::core {

    /**
     * \brief Generic base class of classes AbstractFactory
     * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
     */
    template <class TBase, bool = std::is_void<TBase>::value>
    class GenericAbstractFactory;

    template <class TBase>
    using GenericAbstractFactoryPtr = std::shared_ptr<GenericAbstractFactory<TBase>>; //!< Shared pointer on a generic abstract factory
    template <class TBase>
    using GenericAbstractFactoryWeakPtr = std::weak_ptr<GenericAbstractFactory<TBase>>; //!< Weak pointer on a generic abstract factory

    /**
     * \brief Specialization for "typeless" factory
     * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
     */
    template <>
    class GenericAbstractFactory<void, true> {
    public:
        virtual ~GenericAbstractFactory() = default; //!< Dtor
    };

    /**
     * \brief Specialization for typed factory
     *
     * \tparam TBase Base class of the instance created by the concrete factory
     * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
     */
    template <class TBase>
    class GenericAbstractFactory<TBase, false> : public GenericAbstractFactory<void, true> {
    public:
        virtual void destroy(TBase *pInstance) = 0; //!< Destroy the instance
    };

    /**
     * \brief This abstract class represents an abstract factory
     *
     * \tparam TBase Base class of the instance created by the concrete factory
     * \tparam TArgs Arguments to pass to the callee ctor
     * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
     */
    template <class TBase, typename... TArgs>
    class AbstractFactory : public GenericAbstractFactory<TBase, false> {
    public:
        virtual TBase *create(TArgs &&...args) = 0; //!< Creates a concrete instance of the base class TBase
    };

    /**
     * \brief This class represents a concrete factory
     *
     * \tparam TBase		Base class of the instance created by the concrete factory
     * \tparam TConcrete	Concrete class of the instance created by the concrete factory
     * \tparam TArgs		Arguments to pass to the callee ctor
     * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
     */
    template <class TBase, class TConcrete, bool, typename... TArgs>
    class Factory : public AbstractFactory<TBase, TArgs...> {
    public:
        TBase *create(TArgs &&...) override; //!< \copydoc AbstractFactory::create
        void destroy(TBase *pInstance) override;
    };

    /**
     * \brief	Specialization for singletons
     * \ingroup PACKAGE_DESIGNPATTERN_ABSTRACT_FACTORY
     */
    template <class TBase, class TConcrete>
    class Factory<TBase, TConcrete, true> : public AbstractFactory<TBase> {
    public:
        TBase *create() override; //!< \copydoc AbstractFactory::create
        void destroy(TBase *pInstance) override;
    };

    /** \} */

} // namespace cho::osbase::core

#include "Factory.inl"
