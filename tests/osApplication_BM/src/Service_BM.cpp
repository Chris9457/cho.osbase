#include "Service_BM.h"
#include "Server.h"
#include "Service_BMImpl.h"
#include "osData/IBroker.h"
#include "benchmark/benchmark.h"

using namespace std::chrono_literals;

namespace cho::osbase::application::bm {

    class Service_RPC_BM : public benchmark::Fixture {
    public:
        void SetUp(const benchmark::State &state) override {
            m_asyncStruct.elts.resize(state.range(0), { 1.f, 2.f, 3.f, 4.f });
            m_pStub = makeStub(TheService_BMImpl.getTaskLoop());
            m_pStub->connect(TheServer.getBrokerUrl(), TheServer.getBrokerPort());
        }

        void TearDown(const benchmark::State &) override {
            m_pStub->disconnect();
        }

        auto getStub() const {
            return m_pStub;
        }

        auto const &getAsyncStruct() const {
            return m_asyncStruct;
        }

    private:
        IService_BMServicePtr m_pStub;
        AsyncStruct m_asyncStruct;
    };

    class Service_RPC_Async_BM : public Service_RPC_BM {
    public:
        void SetUp(const benchmark::State &state) override {
            Service_RPC_BM::SetUp(state);
            getStub()->noRetAsyncArg(TheServer.getAsyncBuffer());
            getStub()->noRetAsyncStruct(TheServer.getAsyncStruct());
            m_buffer = std::vector<unsigned char>(state.range(0) * 1024);
        }

        auto const &getBuffer() const {
            return m_buffer;
        }

    private:
        std::vector<unsigned char> m_buffer;
    };

    class Service_PubSub_BM : public benchmark::Fixture {
    public:
        void SetUp(const benchmark::State &state) override {
            auto const nbStubs = static_cast<size_t>(state.range(0));
            for (size_t index = 0; index < nbStubs; ++index) {
                auto pServiceObserver = std::make_shared<ServiceObserver>();

                auto const pStub = makeStub(TheService_BMImpl.getTaskLoop());
                pStub->connect(TheServer.getBrokerUrl(), TheServer.getBrokerPort());
                pStub->attachAll(*pServiceObserver);

                m_pStubObservers.emplace_back(std::make_pair(pStub, pServiceObserver));
            }
        }

        void TearDown(const benchmark::State &state) override {
            auto const nbStubs = static_cast<size_t>(state.range(0));
            for (size_t index = 0; index < nbStubs; ++index) {
                auto const pStub            = m_pStubObservers[index].first;
                auto const pServiceObserver = m_pStubObservers[index].second;
                pStub->detachAll(*pServiceObserver);
                pStub->disconnect();
            }

            m_pStubObservers.clear();
        }

        bool waitForEvent(const size_t index, const std::chrono::milliseconds &timeout = 100ms) {
            auto const pServiceObserver = m_pStubObservers[index].second;
            return pServiceObserver->waitForEvent(timeout);
        }

    private:
        class ServiceObserver : public core::Observer<EvtNoData, EvtData, EvtComplexData> {
        public:
            void update(const core::Observable &, const EvtNoData &) override {
                m_promiseReceivedEvt.set_value();
            }

            void update(const core::Observable &, const EvtData &) override {
                m_promiseReceivedEvt.set_value();
            }

            void update(const core::Observable &, const EvtComplexData &) override {
                m_promiseReceivedEvt.set_value();
            }

            bool waitForEvent(const std::chrono::milliseconds &timeout) {
                auto const guard = core::make_scope_exit([this]() { decltype(m_promiseReceivedEvt)().swap(m_promiseReceivedEvt); });
                auto const futReceivedEvt = m_promiseReceivedEvt.get_future();
                return futReceivedEvt.wait_for(timeout) == std::future_status::ready;
            }

            std::promise<void> m_promiseReceivedEvt;
        };
        using ServiceObserverPtr = std::shared_ptr<ServiceObserver>;

        std::vector<std::pair<IService_BMServicePtr, ServiceObserverPtr>> m_pStubObservers;
    };

    BENCHMARK_DEFINE_F(Service_RPC_BM, noRetArgs)(benchmark::State &state) {
        for (auto _ : state) {
            switch (state.range(0)) {
            case 0:
                if (state.range(1) == 0) {
                    getStub()->noRetNoArg();
                } else {
                    TheService_BMImpl.noRetNoArg();
                }
                break;
            case 1:
                if (state.range(1) == 0) {
                    getStub()->noRet1Arg(2);
                } else {
                    TheService_BMImpl.noRet1Arg(2);
                }
                break;
            case 2:
                if (state.range(1) == 0) {
                    getStub()->noRet2Arg(2, 5.);
                } else {
                    TheService_BMImpl.noRet2Arg(2, 5.);
                }
                break;
            case 4:
                if (state.range(1) == 0) {
                    getStub()->noRet4Arg(2, 5., "toto", 'a');
                } else {
                    TheService_BMImpl.noRet4Arg(2, 5., "toto", 'a');
                }
                break;
            case 8:
                if (state.range(1) == 0) {
                    getStub()->noRet8Arg(2, 5., "toto", 'a', 4, 3., "titi", 'm');
                } else {
                    TheService_BMImpl.noRet8Arg(2, 5., "toto", 'a', 4, 3., "titi", 'm');
                }
                break;
            }
        }
    }
    BENCHMARK_REGISTER_F(Service_RPC_BM, noRetArgs)->ArgsProduct({ { 0, 1, 2, 4, 8 }, { 0, 1 } })->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_RPC_BM, noRetComplexArg)(benchmark::State &state) {
        static ComplexStruct complexStruct = { { 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3 },
            { { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 } },
            "tutu" };

        for (auto _ : state) {
            if (state.range(0) == 0) {
                getStub()->noRet1ComplexArg(complexStruct);
            } else {
                TheService_BMImpl.noRet1ComplexArg(complexStruct);
            }
        }
    } // namespace cho::osbase::application::bm
    BENCHMARK_REGISTER_F(Service_RPC_BM, noRetComplexArg)->Arg(0)->Arg(1)->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_RPC_BM, retNoArg)(benchmark::State &state) {
        for (auto _ : state) {
            if (state.range(0) == 0) {
                getStub()->retNoArg();
            } else {
                TheService_BMImpl.retNoArg();
            }
        }
    } // namespace cho::osbase::application::bm
    BENCHMARK_REGISTER_F(Service_RPC_BM, retNoArg)->Arg(0)->Arg(1)->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_RPC_BM, retComplexArg)(benchmark::State &state) {
        static ComplexStruct complexStruct = { { 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3 },
            { { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 } },
            "tutu" };

        for (auto _ : state) {
            if (state.range(0) == 0) {
                getStub()->noRet1ComplexArg(complexStruct);
            } else {
                TheService_BMImpl.noRet1ComplexArg(complexStruct);
            }
        }
    } // namespace cho::osbase::application::bm
    BENCHMARK_REGISTER_F(Service_RPC_BM, retComplexArg)->Arg(0)->Arg(1)->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_RPC_BM, noRetBigArg)(benchmark::State &state) {
        const std::vector<int> v(state.range(0));

        for (auto _ : state) {
            if (state.range(1) == 0) {
                getStub()->noRetBigArg(v);
            } else {
                TheService_BMImpl.noRetBigArg(v);
            }
        }
    } // namespace cho::osbase::application::bm
    BENCHMARK_REGISTER_F(Service_RPC_BM, noRetBigArg)
        ->RangeMultiplier(2)
        ->Ranges({ { 8, 8 << 15 }, { 0, 1 } })
        ->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_RPC_BM, noRetSyncStruct)(benchmark::State &state) {
        for (auto _ : state) {
            auto const &asyncStruct = getAsyncStruct();
            getStub()->noRetSyncStruct(asyncStruct);
        }
    }
    BENCHMARK_REGISTER_F(Service_RPC_BM, noRetSyncStruct)->RangeMultiplier(10)->Range(200, 20000)->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_RPC_Async_BM, noRetAsyncArg)(benchmark::State &state) {
        for (auto _ : state) {
            TheServer.getAsyncBuffer().set(getBuffer());
            TheService_BMImpl.waitReceivedData();
        }
    }
    BENCHMARK_REGISTER_F(Service_RPC_Async_BM, noRetAsyncArg)->RangeMultiplier(10)->Range(100, 1000000)->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_RPC_Async_BM, noRetAsyncStruct)(benchmark::State &state) {
        for (auto _ : state) {
            TheServer.getAsyncStruct().set(getAsyncStruct());
            TheService_BMImpl.waitReceivedData();
        }
    }
    BENCHMARK_REGISTER_F(Service_RPC_Async_BM, noRetAsyncStruct)->RangeMultiplier(10)->Range(1000, 10000000)->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_RPC_Async_BM, noRetAsyncVolume)(benchmark::State &state) {
        auto const side = static_cast<unsigned short>(state.range(0));
        Volume volume{ { side, side, side }, std::vector<unsigned short>(side * side * side, 3) };

        std::mutex mut;
        for (auto _ : state) {
            std::cout << "Service_RPC_Async_BM/noRetAsyncVolume/" << side << std::endl;
            std::lock_guard lock(mut);
            data::AsyncPagedData<Volume> asyncVolume;
            auto const guard = core::make_scope_exit([&asyncVolume]() {});
            asyncVolume.create();
            asyncVolume.set(volume);
            getStub()->noRetVolume(asyncVolume);
        }
    }
    BENCHMARK_REGISTER_F(Service_RPC_Async_BM, noRetAsyncVolume)->Arg(512)->Unit(benchmark::kMicrosecond);

    BENCHMARK_DEFINE_F(Service_PubSub_BM, checkEvent)(benchmark::State &state) {
        for (auto _ : state) {
            TheService_BMImpl.invokeEvent(state.range(1) == 0   ? DataType::noData
                                          : state.range(1) == 1 ? DataType::simple
                                                                : DataType::complex);
            for (size_t index = 0; index < static_cast<size_t>(state.range(0)); ++index) {
                waitForEvent(index);
            }
        }
    }
    BENCHMARK_REGISTER_F(Service_PubSub_BM, checkEvent)->ArgsProduct({ { 2, 4, 8, 16, 32 }, { 0, 1, 2 } })->Unit(benchmark::kMicrosecond);

} // namespace cho::osbase::application::bm
