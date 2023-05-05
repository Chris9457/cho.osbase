// \brief Declaration of the class Runner

#pragma once
#include <sstream>

namespace NS_OSBASE::application {
    /*
     * \class Runner
     */
    template <typename T>
    void Runner::sendData(const T &data) {
        if (!m_data.isConnected()) {
            return;
        }

        auto const pStream = core::makeJsonStream();
        pStream->setValue(data);

        std::ostringstream oss;
        oss << *pStream;

        m_data.set(oss.str());
    }

} // namespace NS_OSBASE::application
