// \brief Declaration of the function maker for Echange classes

#pragma once

namespace NS_OSBASE::data {
    template <class TExchange>
    std::shared_ptr<TExchange> makeExchange(const std::string &scheme = TExchange::defaultScheme);
}

#include "ExchangeMaker.inl"
