// \brief Declaration of the class Runner

#pragma once
#include <sstream>

namespace NS_OSBASE::application {
    /*
     * \class Runner
     */
    template <typename T>
    T Runner::getData() {
        return m_options.getData<T>();
    }

    template <typename T>
    void Runner::sendData(const T &data) {
        m_options.setData(data);
    }

} // namespace NS_OSBASE::application
