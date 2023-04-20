// \brief Declaration of the class HttpServer

#include "HttpServer.h"
#include "httplib.h"

namespace cho::osbase::web {

    /*
     * \class HttpServer
     */
    HttpServer::HttpServer() {
        httplib::Server svr;
    }
} // namespace cho::osbase::web
