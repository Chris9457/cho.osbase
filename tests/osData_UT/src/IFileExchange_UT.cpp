// \brief Unit test for all IFileExhange family

#include "osData/IFileExchange.h"
#include "gtest/gtest.h"
#include <fstream>
#include <future>

using namespace std::chrono_literals;

namespace cho::osbase::data::ut {

    static const std::vector<std::string> schemeFamilies{ Uri::schemeFile() };

    class FileExchangeDelegate : public IFileExchange::IDelegate {
    public:
        void onConnected(const bool connected) override {
            m_promiseConnected.set_value(connected);
        }

        void onFailure(std::string &&failure) override {
            std::ignore = failure;
        }

        void onFileReceived(const std::filesystem::path &path) override {
            m_promisePath.set_value(path);
        }

        std::optional<bool> waitConnected(const std::chrono::milliseconds &timeout = 1s) {
            auto const guard  = core::make_scope_exit([this]() { std::promise<bool>().swap(m_promiseConnected); });
            auto futConnected = m_promiseConnected.get_future();
            if (futConnected.wait_for(timeout) == std::future_status::timeout) {
                return {};
            }

            return futConnected.get();
        }

        std::optional<std::filesystem::path> waitFileReceived(const std::chrono::milliseconds &timeout = 1s) {
            auto const guard = core::make_scope_exit([this]() { std::promise<std::filesystem::path>().swap(m_promisePath); });
            auto futPath     = m_promisePath.get_future();
            if (futPath.wait_for(timeout) == std::future_status::timeout) {
                return {};
            }

            return futPath.get();
        }

    private:
        std::promise<bool> m_promiseConnected;
        std::promise<std::filesystem::path> m_promisePath;
    };

    class IFIleExchange_UT : public testing::Test {
    protected:
        void SetUp() override {
            m_pCreatorDelegate = std::make_shared<FileExchangeDelegate>();
            m_pClientDelegate  = std::make_shared<FileExchangeDelegate>();

            for (auto &&schemeFamily : schemeFamilies) {
                auto const pCreator = makeFileExchange(schemeFamily);
                pCreator->setDelegate(m_pCreatorDelegate);
                pCreator->create();
                std::this_thread::sleep_for(10ms);

                auto const pClient = makeFileExchange(schemeFamily);
                pClient->setDelegate(m_pClientDelegate);
                pClient->open(pCreator->getUriOfCreator());
                std::this_thread::sleep_for(10ms);

                m_pCreatorDelegate->waitConnected();
                m_pClientDelegate->waitConnected();

                m_pCreators.insert({ schemeFamily, pCreator });
                m_pClients.insert({ schemeFamily, pClient });
            }
        }

        void TearDown() override {
            for (auto &&schemeFamily : schemeFamilies) {
                if (m_pClients[schemeFamily]->isWired()) {
                    m_pClients[schemeFamily]->close();
                    m_pCreatorDelegate->waitConnected(10s);
                    m_pClientDelegate->waitConnected(10s);
                }
            }

            m_pClients.clear();
            m_pCreators.clear();
        }

        IFileExchangePtr getCreator(const std::string &schemeFamily) const {
            return m_pCreators.find(schemeFamily)->second;
        }

        IFileExchangePtr getClient(const std::string &schemeFamily) const {
            return m_pClients.find(schemeFamily)->second;
        }

        std::shared_ptr<FileExchangeDelegate> getCreatorDelegate() const {
            return m_pCreatorDelegate;
        }

        std::shared_ptr<FileExchangeDelegate> getClientDelegate() const {
            return m_pClientDelegate;
        }

        static std::filesystem::path createFile() {
            auto path = std::filesystem::current_path() / ".test";
            std::ofstream ofs(path);
            ofs.close();
            return path;
        }

        static std::filesystem::path createDirectory() {
            auto path = std::filesystem::current_path() / "test";
            create_directory(path);
            auto const subPath = path / "sub";
            create_directory(subPath);

            auto const pathFile = subPath / ".test";
            std::ofstream ofs(pathFile);
            ofs.close();
            return path;
        }

    private:
        std::shared_ptr<FileExchangeDelegate> m_pCreatorDelegate;
        std::shared_ptr<FileExchangeDelegate> m_pClientDelegate;
        std::map<std::string, IFileExchangePtr> m_pCreators;
        std::map<std::string, IFileExchangePtr> m_pClients;
    };

    TEST_F(IFIleExchange_UT, make) {
        for (auto &&scheme : schemeFamilies) {
            auto const pFileExchange = makeFileExchange(scheme);
            ASSERT_NE(nullptr, pFileExchange);
        }
    }

    TEST_F(IFIleExchange_UT, createOK) {
        for (auto &&scheme : schemeFamilies) {
            auto const pFileExchange = makeFileExchange(scheme);

            ASSERT_NO_THROW(pFileExchange->create());
            ASSERT_NE(Uri::null(), pFileExchange->getUriOfCreator());
        }
    }

    TEST_F(IFIleExchange_UT, createKO) {
        for (auto &&scheme : schemeFamilies) {
            auto const pFileExchange = makeFileExchange(scheme);

            pFileExchange->create();
            ASSERT_THROW(pFileExchange->create(), FileExchangeException);
        }
    }

    TEST_F(IFIleExchange_UT, destroyOK) {
        for (auto &&scheme : schemeFamilies) {
            auto const pFileExchange = makeFileExchange(scheme);

            pFileExchange->create();
            ASSERT_NO_THROW(pFileExchange->destroy());
        }
    }

    TEST_F(IFIleExchange_UT, destroyKO) {
        for (auto &&scheme : schemeFamilies) {
            auto const pFileExchange = makeFileExchange(scheme);

            ASSERT_THROW(pFileExchange->destroy(), FileExchangeException);

            pFileExchange->create();
            ASSERT_NO_THROW(pFileExchange->destroy());
            ASSERT_THROW(pFileExchange->destroy(), FileExchangeException);
        }
    }

    TEST_F(IFIleExchange_UT, openOK) {
        for (auto &&scheme : schemeFamilies) {
            ASSERT_TRUE(getClient(scheme)->isWired());
        }
    }

    TEST_F(IFIleExchange_UT, openKO) {
        for (auto &&scheme : schemeFamilies) {
            auto const pFileExchange = makeFileExchange(scheme);

            auto const uri = getCreator(scheme)->getUriOfCreator();
            ASSERT_THROW(pFileExchange->open(uri), FileExchangeException);
            getClient(scheme)->close();

            getCreator(scheme)->destroy();
            ASSERT_THROW(pFileExchange->open(uri), FileExchangeException);
        }
    }

    TEST_F(IFIleExchange_UT, pushFileOK) {
        for (auto &&scheme : schemeFamilies) {
            auto path = createFile();

            const std::filesystem::path basePath = *getCreator(scheme)->getUriOfCreator().path;
            auto const expectedPath              = basePath / path.filename();

            getClient(scheme)->push(path);
            auto const receivedPath = getCreatorDelegate()->waitFileReceived();
            ASSERT_TRUE(receivedPath);
            ASSERT_EQ(expectedPath, *receivedPath);

            auto const clientReceivedPath = getClientDelegate()->waitFileReceived();
            ASSERT_FALSE(clientReceivedPath);

            remove(path);
        }
    }

    TEST_F(IFIleExchange_UT, pushFileKO) {
        for (auto &&scheme : schemeFamilies) {
            auto path = createFile();

            const std::filesystem::path basePath = *getCreator(scheme)->getUriOfCreator().path;
            auto const expectedPath              = basePath / path.filename();

            remove(path);
            ASSERT_THROW(getClient(scheme)->push(path), FileExchangeException);

            path = createFile();
            getClient(scheme)->close();
            ASSERT_THROW(getClient(scheme)->push(path), FileExchangeException);
            remove(path);
        }
    }

    TEST_F(IFIleExchange_UT, pushDirectoryOK) {
        for (auto &&scheme : schemeFamilies) {
            auto path = createDirectory();

            const std::filesystem::path basePath = *getCreator(scheme)->getUriOfCreator().path;
            auto const expectedPath              = basePath / path.filename();

            getClient(scheme)->push(path);
            auto const receivedPath = getCreatorDelegate()->waitFileReceived();
            ASSERT_TRUE(receivedPath);
            ASSERT_EQ(expectedPath, *receivedPath);

            auto const clientReceivedPath = getClientDelegate()->waitFileReceived();
            ASSERT_FALSE(clientReceivedPath);

            remove_all(path);
        }
    }

    TEST_F(IFIleExchange_UT, pushDirectoryKO) {
        for (auto &&scheme : schemeFamilies) {
            auto path = createDirectory();

            const std::filesystem::path basePath = *getCreator(scheme)->getUriOfCreator().path;
            auto const expectedPath              = basePath / path.filename();

            remove_all(path);
            ASSERT_THROW(getClient(scheme)->push(path), FileExchangeException);

            path = createDirectory();
            getClient(scheme)->close();
            ASSERT_THROW(getClient(scheme)->push(path), FileExchangeException);
            remove_all(path);
        }
    }

} // namespace cho::osbase::data::ut
