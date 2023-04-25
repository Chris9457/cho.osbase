// \brief Declaration of the class StructuredExceptionHandler

#pragma once

namespace NS_OSBASE::core {

    /*
     * \class StructuredExceptionHandler
     */
    template <typename TRet, typename TFunc, typename... TArgs>
    TRet StructuredExceptionHandler::checkStructuredException(const TFunc &f, TArgs &&...args) const {
        if constexpr (std::is_void_v<TRet>) {
            checkStructuredException([&f, &args...]() { f(std::forward<TArgs>(args)...); });
        } else {
            TRet ret;
            checkStructuredException([&f, &args..., &ret]() { ret = f(std::forward<TArgs>(args)...); });
            return ret;
        }
    }

} // namespace NS_OSBASE::core
