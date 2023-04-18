// \brief Implementation tests of INetwork

#include "osData/INetwork.h"
#include "gtest/gtest.h"

namespace cho::osbase::data::ut {

    class INetwork_UT : public testing::Test {
    protected:
        static bool isIPv4Address(const std::string &address) {
            static const std::regex regex("[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+");
            return std::regex_match(address, regex);
        }
    };

    TEST_F(INetwork_UT, maker) {
        auto const pNetwork = makeNetwork();
        ASSERT_NE(nullptr, pNetwork);
    }

    TEST_F(INetwork_UT, getLocalHost) {
        auto const pNetwork  = makeNetwork();
        const auto localHost = pNetwork->getLocalHost();
        ASSERT_TRUE(isIPv4Address(localHost));
        ASSERT_NE(static_cast<std::string>(localHost), "127.0.0.1");
    }

} // namespace cho::osbase::data::ut
