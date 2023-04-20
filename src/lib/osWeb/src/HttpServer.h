// \brief Declaration of the class HttpServer

#pragma once
#include "osWeb/IHttpServer.h"

namespace cho::osbase::web {

    /**
     *\brief Implementation of the interface IWebServer
     */
    class HttpServer : public IHttpServer {
    public:
        HttpServer();
    };
} // namespace cho::osbase::web
