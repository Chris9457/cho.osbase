// \brief Define simple command parser class

#pragma once
#include "osData/Uri.h"
#include "osData/AsyncData.h"
#include <optional>
#include <string>

/**
 * \addtogroup PACKAGE_SERVICE
 * \{
 */

namespace NS_OSBASE::application {

    /**
     * \brief Options of the service
     */
    class ServiceOptions {
    public:
        class IDelegate {
        public:
            virtual ~IDelegate()             = default;
            virtual void onOptionsReceived() = 0;
        };
        using IDelegatePtr  = std::shared_ptr<IDelegate>;
        using IDelegateWPtr = std::weak_ptr<IDelegate>;

        ServiceOptions();
        ~ServiceOptions();
        explicit ServiceOptions(const std::string &options);
        explicit ServiceOptions(const data::Uri &options);
        explicit ServiceOptions(int argc, char **argv);

        std::string makeCommandLine(const bool bLong = false) const; //!< builds a command line from a ServiceOptions struct

        std::string getContent() const;
        void setContent(const std::string &content);

        bool waitContent(const std::chrono::milliseconds &timeout) const;

        template <typename T>
        T getData() const;
        template <typename T>
        void setData(const T &data);

        void reset();

        void setDelegate(IDelegatePtr pDelegate);

    private:
        void onData(std::string &&data);

        IDelegateWPtr m_pDelegate;
        data::AsyncData<std::string> m_data;
        mutable std::mutex m_mutContent;
        mutable std::condition_variable m_cvContent;
        std::string m_content;
        mutable bool m_bContentReceived = false;
    };
} // namespace NS_OSBASE::application

#include "ServiceOptions.inl"
