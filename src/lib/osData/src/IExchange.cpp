// \brief Base interface to exhange information

#include "osData/IExchange.h"

namespace NS_OSBASE::data {
    IExchange::AccessType operator&(const IExchange::AccessType &lhs, const IExchange::AccessType &rhs) {
        return static_cast<IExchange::AccessType>(static_cast<std::underlying_type_t<IExchange::AccessType>>(lhs) &
                                                  static_cast<std::underlying_type_t<IExchange::AccessType>>(rhs));
    }

    IExchange::AccessType operator|(const IExchange::AccessType &lhs, const IExchange::AccessType &rhs) {
        return static_cast<IExchange::AccessType>(static_cast<std::underlying_type_t<IExchange::AccessType>>(lhs) |
                                                  static_cast<std::underlying_type_t<IExchange::AccessType>>(rhs));
    }
} // namespace NS_OSBASE::data
