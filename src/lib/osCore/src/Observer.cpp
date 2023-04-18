// osBase package
//
// \file  osobserver.cpp
// \brief Implementation of the class Observable

#include "osCore/DesignPattern/Observer.h"

namespace cho::osbase::core {

    /*
     * \class Observable
     */

    Observable::Observable() = default;

    Observable::Observable(const Observable &other) noexcept {
        *this = other;
    }

    Observable::Observable(Observable &&other) noexcept {
        swap(*this, other);
    }

    Observable::~Observable() = default;

    Observable &Observable::operator=(const Observable &other) noexcept {
        if (this == &other)
            return *this;

        m_observerFamilies.clear();
        for (auto &&family : other.m_observerFamilies)
            family.second->attachAll(*this);

        return *this;
    }

    Observable &Observable::operator=(Observable &&other) noexcept {
        Observable temp(std::move(other));
        swap(*this, temp);
        return *this;
    }

    void swap(Observable &lhs, Observable &rhs) noexcept {
        Observable::ObserverFamilies lFamilies = lFamilies;
        Observable::ObserverFamilies rFamilies = rFamilies;

        for (auto &&lFamily : lhs.m_observerFamilies) {
            lFamily.second->detachAll(lhs);
            lFamily.second->attachAll(rhs);
        }

        for (auto &&rFamily : rhs.m_observerFamilies) {
            rFamily.second->detachAll(rhs);
            rFamily.second->attachAll(lhs);
        }
    }

    /*
     * \class Observable::AbstractObserverContainer
     */
    Observable::AbstractObserverContainer::~AbstractObserverContainer() = default;

} // namespace cho::osbase::core
