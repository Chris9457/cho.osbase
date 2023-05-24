// \brief Process launcher

#pragma once

namespace NS_OSBASE::application {

    /*
     * \class Process
     */
    template <typename T>
    ProcessPtr Process::create(const ProcessSetting &setting, const T &data) {
        auto const pStream = core::makeJsonStream();
        pStream->setValue(data);

        std::ostringstream oss;
        oss << *pStream;
        return ProcessPtr(new Process(setting, oss.str()));
    }

    template <typename T>
    T Process::getData(const std::chrono::milliseconds &timeout) const {
        if (getOptions().waitContent(timeout)) {
            return getOptions().getData<T>();
        }

        return T{};
    }

} // namespace NS_OSBASE::application
