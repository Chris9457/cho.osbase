// \brief Declaration of the class TestProcessServiceImpl

#include "osApplication_UT/TestProcessServiceImpl.h"

namespace NS_OSBASE::application::ut {

    /*
     * \class TestProcessImpl
     */
    TestProcessImpl::TestProcessImpl() {
    }

    void TestProcessImpl::noReturnNoArg() {
    }

    char TestProcessImpl::retCharNoArg() {
        return 'A';
    }

    unsigned char TestProcessImpl::retUnsignedCharNoArg() {
        return 0xff;
    }

    short TestProcessImpl::retShortNoArg() {
        return -1;
    }

    unsigned short TestProcessImpl::retUnsignedShortNoArg() {
        return 3;
    }

    int TestProcessImpl::retIntNoArg() {
        return -10;
    }

    unsigned TestProcessImpl::retUnsignedIntNoArg() {
        return 114u;
    }

    long long TestProcessImpl::retLongNoArg() {
        return -42ll;
    }

    unsigned long long TestProcessImpl::retUnsignedLongNoArg() {
        return 18ull;
    }

    float TestProcessImpl::retShortFloatingNoArg() {
        return -0.5f;
    }

    double TestProcessImpl::retFloatingNoArg() {
        return -0.7;
    }

    std::string TestProcessImpl::retStringNoArg() {
        return "toto";
    }

    bool TestProcessImpl::retBoolNoArg() {
        return true;
    }

    NS_OSBASE::data::Uri TestProcessImpl::retUriNoArg() {
        return type_cast<data::Uri>(std::string("ws://localhost:50210"));
    }

    process::CustomType TestProcessImpl::retCustomTypeNoArg() {
        return { true };
    }

    std::array<int, 4> TestProcessImpl::retArrayFixNoArg() {
        return { 0, 1, 2, 3 };
    }

    std::vector<std::string> TestProcessImpl::retArrayNotFixNoArg() {
        return { "titi", "toto" };
    }

    NS_OSBASE::data::AsyncData<std::vector<std::string>> TestProcessImpl::retAsyncNoArg() {
        return m_asyncStrings;
    }

    NS_OSBASE::data::AsyncPagedData<std::vector<bool>> TestProcessImpl::retAsyncPagedNoArg() {
        return m_asyncBools;
    }

    process::TestAllBasicTypes TestProcessImpl::retArgs(char _char,
        unsigned char _unsigned_char,
        short _short_integer,
        unsigned short _unsigned_short_integer,
        int _integer,
        unsigned _unsigned_integer,
        long long _long_integer,
        unsigned long long _unsigned_long_integer,
        float _short_floating,
        double _floating,
        std::string _string,
        bool _boolean,
        NS_OSBASE::data::Uri _uri) {
        return { _char,
            _unsigned_char,
            _short_integer,
            _unsigned_short_integer,
            _integer,
            _unsigned_integer,
            _long_integer,
            _unsigned_long_integer,
            _short_floating,
            _floating,
            _string,
            _boolean,
            _uri };
    }

    process::ResultAsync TestProcessImpl::retAsyncArg(process::AsyncStruct asyncValues) {
        auto const bools   = asyncValues.bools.get();
        auto const strings = asyncValues.strings.get();
        asyncValues.bools.reset();
        asyncValues.strings.reset();
        return { bools, strings };
    }

    std::string TestProcessImpl::retNoArgConst() const {
        return "test";
    }

    NS_OSBASE::data::AsyncData<std::vector<int>> TestProcessImpl::fwdAsyncData() {
        return m_asyncFwdData;
    }

    void TestProcessImpl::setFwdData(const std::vector<int> &data) {
        m_asyncFwdData.set(data);
    }

    void TestProcessImpl::doConnect(const std::string &, const unsigned short) {
        m_asyncStrings.create();
        m_asyncStrings.set({ "titi", "toto" });

        m_asyncBools = data::makeAsyncPagedData<std::vector<bool>>();
        m_asyncBools.set({ true, false, false });

        m_asyncFwdData.create();
    }

    void TestProcessImpl::doDisconnect() {
        m_asyncFwdData.reset();
        m_asyncStrings.reset();
        m_asyncBools.reset();
    }
} // namespace NS_OSBASE::application::ut
