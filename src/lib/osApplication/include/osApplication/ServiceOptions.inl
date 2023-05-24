// \brief Define simple command parser class

#pragma once
#include <sstream>

namespace NS_OSBASE::application {

    /*
     * \class ServiceOptions
     */
    template <typename T>
    T ServiceOptions::getData() const {
        std::lock_guard lock(m_mutContent);
        auto const pStream = core::makeJsonStream(std::istringstream(m_content));
        return pStream->getValue(T{});
    }

    template <typename T>
    void ServiceOptions::setData(const T &data) {
        auto const pStream = core::makeJsonStream();
        pStream->setValue(data);

        std::ostringstream oss;
        oss << *pStream;

        std::lock_guard lock(m_mutContent);
        setContent(oss.str());
    }

} // namespace NS_OSBASE::application
