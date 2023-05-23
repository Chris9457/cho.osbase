// \brief Declaration of the data exchanged when exetuting the process

#pragma once
#include "osCore/Serialization/CoreKeySerializer.h"
#include "osData/Uri.h"

namespace NS_OSBASE::broker {

    struct Input {
        unsigned short port;
    };

    struct Output {
        data::Uri uri;
    };

    struct Settings {
        std::optional<Input> input;
        std::optional<Output> output;
    };
} // namespace NS_OSBASE::broker
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::broker::Input, port)
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::broker::Output, uri)
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::broker::Settings, input, output)
