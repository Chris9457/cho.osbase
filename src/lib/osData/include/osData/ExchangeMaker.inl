// \brief Declaration of the function maker for Echange classes

#pragma once
#include "IFileExchange.h"
#include "IDataExchange.h"

namespace cho::osbase::data {
    namespace internal {
        template <class TExchange>
        struct ExchangeMaker;

        template <>
        struct ExchangeMaker<IDataExchange> {
            static IDataExchangePtr make(const std::string &scheme) {
                return makeDataExchange(scheme);
            }
        };

        template <>
        struct ExchangeMaker<IFileExchange> {
            static IDataExchangePtr make(const std::string &scheme) {
                return makeDataExchange(scheme);
            }
        };
    } // namespace internal

    template <class TExchange>
    std::shared_ptr<TExchange> makeExchange(const std::string &scheme) {
        return internal::ExchangeMaker<TExchange>::make(scheme);
    }

} // namespace cho::osbase::data
