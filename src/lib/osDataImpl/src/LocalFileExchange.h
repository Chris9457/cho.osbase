// \brief Declaration of the local file data exchange

#pragma once
#include "osCore/Misc/RecursiveNamedMutex.h"
#include "osData/IFileExchange.h"
#include <mutex>
#include <set>

namespace cho::osbase::data::impl {

    class LocalFileExchange : public IFileExchange {
    public:
        ~LocalFileExchange() override;

        Uri getUriOfCreator() const override;
        void open(const Uri &uri) override;
        void close() override;
        void create() override;
        void destroy() override;
        void push(const std::filesystem::path &path) const override;
        void setDelegate(IDelegatePtr pDelegate) noexcept override;
        AccessType getAccessType() const noexcept override;
        bool isWired() const noexcept override;

    private:
        void closeLocalPath();
        void destroyLocalPath();

        std::filesystem::path getLockFilePath() const;
        void watchLocalPath();

        void removeFolder();
        void removeLock();

        std::filesystem::path m_localPath;
        IDelegateWPtr m_pDelegate;
        std::atomic<AccessType> m_accessType = AccessType::CreateOpen;
        std::set<std::string> m_localFiles;

        std::thread m_thLocalPathChange;
        mutable core::recursive_named_mutex m_mutexWatch;
        std::atomic_bool m_bEndWatchLocalPath = false;
        mutable std::atomic_bool m_bPushed    = false;
    };
} // namespace cho::osbase::data::impl