// \brief Declaration of the local file data exchange

#include "LocalFileExchange.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include <Windows.h>
#include <fstream>
#include <set>
#include <sstream>

using namespace std::chrono_literals;

namespace NS_OSBASE::data::impl {
    OS_REGISTER_FACTORY_N(IFileExchange, LocalFileExchange, 0, IFILEEXCHANGE_LOCALFILE_FACTORY_NAME)

    LocalFileExchange::~LocalFileExchange() {
        if (m_accessType == AccessType::CreateReadWrite) {
            std::lock_guard lock(m_mutexWatch);
            destroyLocalPath();
        } else if (m_accessType == AccessType::OpenReadWrite) {
            std::lock_guard lock(m_mutexWatch);
            closeLocalPath();
        }
    }

    Uri LocalFileExchange::getUriOfCreator() const {
        return type_cast<Uri>(m_localPath);
    }

    void LocalFileExchange::open(const Uri &uri) {
        m_bEndWatchLocalPath = false;

        if ((getAccessType() & AccessType::CreateOpen) != AccessType::CreateOpen) {
            throw FileExchangeException("Unable to open: in read/write mode");
        }

        try {
            m_localPath  = type_cast<std::filesystem::path>(uri);
            m_mutexWatch = core::recursive_named_mutex(m_localPath.filename().u8string());
        } catch (const BadUriException &e) {
            throw FileExchangeException(e.what());
        }

        if (!exists(m_localPath)) {
            throw FileExchangeException("Folder doesn't exist: " + m_localPath.u8string());
        }

        if (exists(getLockFilePath())) {
            throw FileExchangeException("Folder already opened: " + m_localPath.u8string());
        }

        m_accessType        = AccessType::OpenReadWrite;
        m_thLocalPathChange = std::thread([this]() { watchLocalPath(); });

        std::ofstream ofs(getLockFilePath());
        ofs.close();
    }

    void LocalFileExchange::close() {
        closeLocalPath();
    }

    void LocalFileExchange::create() {
        m_bEndWatchLocalPath = false;

        if ((getAccessType() & AccessType::CreateOpen) != AccessType::CreateOpen) {
            throw FileExchangeException("Unable to create: in read/write mode");
        }

        std::stringstream oss;
        auto const now = std::chrono::system_clock::now();
        oss << std::chrono::system_clock::to_time_t(now);
        m_localPath  = std::filesystem::current_path() / oss.str();
        m_mutexWatch = core::recursive_named_mutex(m_localPath.filename().u8string());

        try {
            create_directory(m_localPath);
        } catch (const std::filesystem::filesystem_error &e) {
            throw FileExchangeException(e.what());
        }

        m_accessType        = AccessType::CreateReadWrite;
        m_thLocalPathChange = std::thread([this]() { watchLocalPath(); });
    }

    void LocalFileExchange::destroy() {
        destroyLocalPath();
    }

    void LocalFileExchange::push(const std::filesystem::path &path) const {
        std::lock_guard lock(m_mutexWatch);
        if ((getAccessType() & AccessType::ReadWrite) != AccessType::ReadWrite) {
            throw FileExchangeException("Unable to push: in open/create mode");
        }

        if (!exists(path)) {
            throw FileExchangeException("Path doesn't exist");
        }

        // Copy file or directory
        try {
            auto const destPath    = m_localPath / path.filename();
            m_bPushed              = true;
            auto const copyOptions = is_directory(path)
                                         ? std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing
                                         : std::filesystem::copy_options::overwrite_existing;
            copy(path, destPath, copyOptions);

            auto &self        = const_cast<LocalFileExchange &>(*this);
            auto const itFile = self.m_localFiles.find(destPath.u8string());
            if (itFile == m_localFiles.cend()) {
                self.m_localFiles.insert(destPath.u8string());
            }
        } catch (const std::filesystem::filesystem_error &e) {
            throw FileExchangeException(e.what());
        }
    }

    void LocalFileExchange::setDelegate(IDelegatePtr pDelegate) noexcept {
        m_pDelegate = pDelegate;
    }

    IFileExchange::AccessType LocalFileExchange::getAccessType() const noexcept {
        return m_accessType;
    }

    bool LocalFileExchange::isWired() const noexcept {
        return exists(getLockFilePath());
    }

    void LocalFileExchange::closeLocalPath() {
        if ((getAccessType() & AccessType::ReadWrite) != AccessType::ReadWrite) {
            throw FileExchangeException("Unable to close: in open/create mode");
        }

        m_accessType         = AccessType::CreateOpen;
        m_bEndWatchLocalPath = true;
        removeLock();
        m_thLocalPathChange.join();
        m_localFiles.clear();
    }

    void LocalFileExchange::destroyLocalPath() {
        if ((getAccessType() & AccessType::ReadWrite) != AccessType::ReadWrite) {
            throw FileExchangeException("Unable to destroy: in open/create mode");
        }

        m_accessType         = AccessType::CreateOpen;
        m_bEndWatchLocalPath = true;
        removeFolder();
        m_thLocalPathChange.join();
        m_localFiles.clear();
    }

    std::filesystem::path LocalFileExchange::getLockFilePath() const {
        return m_localPath / ".lock";
    }

    void LocalFileExchange::watchLocalPath() {
        static auto constexpr timeout = 10ms;

        HANDLE hDir = CreateFile(m_localPath.wstring().c_str(),     // pointer to the file name
            FILE_LIST_DIRECTORY,                                    // access (read/write) mode
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // share mode
            nullptr,                                                // security descriptor
            OPEN_EXISTING,                                          // how to create
            FILE_FLAG_BACKUP_SEMANTICS,                             // file attributes
            nullptr                                                 // file with attributes to copy
        );

        if (hDir == nullptr) {
            return;
        }

        auto getLocalPathIterator = [this](std::filesystem::path path) {
            while (path != m_localPath) {
                auto const itLocalPath = m_localFiles.find(path.u8string());
                if (itLocalPath != m_localFiles.cend()) {
                    return itLocalPath;
                }

                path = path.parent_path();
            }

            return m_localFiles.end();
        };

        auto guard = core::make_scope_exit([this, hDir]() { CloseHandle(hDir); });
        while (!m_bEndWatchLocalPath) {
            wchar_t filename[MAX_PATH];
            uint8_t buffer[1024];
            DWORD nbBytesReturned = 0;
            if (!ReadDirectoryChangesW(hDir, // handle to directory
                    buffer,                  // read results buffer
                    1024,                    // length of buffer
                    TRUE,                    // monitoring option
                    FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE |
                        FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_FILE_NAME, // filter conditions
                    &nbBytesReturned,                                               // bytes returned
                    nullptr,                                                        // overlapped buffer
                    nullptr                                                         // completion routine
                    )) {
                break;
            }

            auto const pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(buffer);
            wcscpy_s(filename, L"");
            wcsncpy_s(filename, pInfo->FileName, pInfo->FileNameLength / 2);

            filename[pInfo->FileNameLength / 2] = NULL;
            auto const modifiedFilePath         = m_localPath / filename;

            switch (pInfo->Action) {
            case FILE_ACTION_ADDED:
                if (modifiedFilePath == getLockFilePath()) {
                    if (auto const pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                        pDelegate->onConnected(true);
                    }
                } else if (!m_bPushed) {
                    std::lock_guard lock(m_mutexWatch);
                    if (getLocalPathIterator(modifiedFilePath) == m_localFiles.cend()) {
                        if (auto const pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                            pDelegate->onFileReceived(modifiedFilePath);
                        }

                        m_localFiles.insert(modifiedFilePath.u8string());
                    }
                }
                break;
            case FILE_ACTION_REMOVED:
                if (modifiedFilePath == getLockFilePath()) {
                    if (auto const pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                        std::lock_guard lock(m_mutexWatch);
                        pDelegate->onConnected(false);
                    }
                } else if (!m_bEndWatchLocalPath) {
                    std::lock_guard lock(m_mutexWatch);
                    if (auto itLocalFile = getLocalPathIterator(modifiedFilePath); itLocalFile != m_localFiles.cend()) {
                        m_localFiles.erase(itLocalFile);
                    }
                }
                break;
            default:
                // not handled
                break;
            }
        }
    }

    void LocalFileExchange::removeFolder() {
        std::lock_guard lock(m_mutexWatch);
        try {
            remove_all(m_localPath);
            m_localPath.clear();
        } catch (std::filesystem::filesystem_error &e) {
            throw FileExchangeException(e.what());
        }
        m_accessType = AccessType::CreateOpen;
    }

    void LocalFileExchange::removeLock() {
        std::lock_guard lock(m_mutexWatch);
        try {
            remove(getLockFilePath());
        } catch (const std::filesystem::filesystem_error &e) {
            throw FileExchangeException(e.what());
        }
        m_accessType = AccessType::CreateOpen;
    }
} // namespace NS_OSBASE::data::impl
