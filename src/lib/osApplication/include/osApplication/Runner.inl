// \brief Declaration of the class Runner

#pragma once
#include <sstream>

namespace NS_OSBASE::application {
    /*
     * \class Runner
     */
    template <typename T>
    T Runner::getData() {
        if (m_options.waitContent(m_timeout)) {
            return m_options.getData<T>();
        }

        return T{};
    }

    template <typename T>
    void Runner::sendData(const T &data) {
        m_options.setData(data);
    }

} // namespace NS_OSBASE::application
