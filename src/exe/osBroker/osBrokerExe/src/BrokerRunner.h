// \brief Declaration of the class BrokerRunner

#pragma once
#include "osApplication/Runner.h"

namespace NS_OSBASE::broker {

    /**
     * \brief Runner for the broker
     */
    class BrokerRunner : public application::Runner {
    public:
        BrokerRunner(int argc, char **argv);

        int run();
    };
} // namespace NS_OSBASE::broker
