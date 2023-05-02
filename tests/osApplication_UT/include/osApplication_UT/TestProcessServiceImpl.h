// \brief Declaration of the class TestProcessImpl

#pragma once
#include "TestProcessSkeleton.h"
#include "osCore/DesignPattern/Singleton.h"

namespace NS_OSBASE::application::ut {

    class TestProcessImpl final : public process::TestProcessServiceSkeleton, public core::Singleton<TestProcessImpl> {
        friend core::Singleton<TestProcessImpl>;

    public:
        void noReturnNoArg() override;
        char retCharNoArg() override;
        unsigned char retUnsignedCharNoArg() override;
        short retShortNoArg() override;
        unsigned short retUnsignedShortNoArg() override;
        int retIntNoArg() override;
        unsigned retUnsignedIntNoArg() override;
        long long retLongNoArg() override;
        unsigned long long retUnsignedLongNoArg() override;
        float retShortFloatingNoArg() override;
        double retFloatingNoArg() override;
        std::string retStringNoArg() override;
        bool retBoolNoArg() override;
        NS_OSBASE::data::Uri retUriNoArg() override;
        process::CustomType retCustomTypeNoArg() override;
        std::array<int, 4> retArrayFixNoArg() override;
        std::vector<std::string> retArrayNotFixNoArg() override;
        NS_OSBASE::data::AsyncData<std::vector<std::string>> retAsyncNoArg() override;
        NS_OSBASE::data::AsyncPagedData<std::vector<bool>> retAsyncPagedNoArg() override;
        process::TestAllBasicTypes retArgs(char _char,
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
            NS_OSBASE::data::Uri _uri) override;
        process::ResultAsync retAsyncArg(process::AsyncStruct asyncValues) override;
        std::string retNoArgConst() const override;
        NS_OSBASE::data::AsyncData<std::vector<int>> fwdAsyncData() override;

        void setFwdData(const std::vector<int> &data);

    protected:
        void doConnect() override;
        void doDisconnect() override;

    private:
        TestProcessImpl();

        data::AsyncData<std::vector<std::string>> m_asyncStrings;
        data::AsyncPagedData<std::vector<bool>> m_asyncBools;
        data::AsyncData<std::vector<int>> m_asyncFwdData;
    };

#define TheTestProcessImpl TestProcessImpl::getInstance()
} // namespace NS_OSBASE::application::ut
