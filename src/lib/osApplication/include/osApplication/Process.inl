// \brief Process launcher

#pragma once

namespace NS_OSBASE::application {

    /*
     * \class Process
     */
    template <typename T>
    T Process::getData() const {
        auto const pStream = core::makeJsonStream(std::istringstream(getData()));
        return pStream->getValue(T{});
    }

} // namespace NS_OSBASE::application
