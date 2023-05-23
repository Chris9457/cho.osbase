// \brief Define simple command parser class

#pragma once
#include <sstream>

namespace NS_OSBASE::application {

    /*
     * \class ServiceOptions
     */
    template <typename T>
    T ServiceOptions::getData(const std::chrono::milliseconds &timeout) const {
        std::unique_lock lock(m_mutContent);
        if (m_cvContent.wait_for(lock, timeout, [this] { return m_bContentReceived; })) {
            m_bContentReceived = false;
            auto const pStream = core::makeJsonStream(std::istringstream(m_content));
            return pStream->getValue(T{});
        }

        return {};
    }

    template <typename T>
    void ServiceOptions::setData(const T &data) {
        auto const pStream = core::makeJsonStream();
        pStream->setValue(data);

        std::ostringstream oss;
        oss << *pStream;
        setContent(oss.str());
    }

} // namespace NS_OSBASE::application
