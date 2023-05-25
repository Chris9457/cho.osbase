// \brief Declaration of the class ServiceRunner

#pragma once
#include "Runner.h"

namespace NS_OSBASE::application {

    /**
     * \brief Specialization for services
     */
    template <class TService>
    class ServiceRunner : public Runner {
    public:
        ServiceRunner(int argc, char **argv);

        int run();
    };
} // namespace NS_OSBASE::application

#include "ServiceRunner.inl"
