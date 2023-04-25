// \brief Declaration of the class HttpServer

#pragma once
#include "osWeb/IHttpServer.h"

namespace NS_OSBASE::web {

    /**
     *\brief Implementation of the interface IWebServer
     */
    class HttpServer : public IHttpServer {
    public:
        HttpServer();
    };
} // namespace NS_OSBASE::web
