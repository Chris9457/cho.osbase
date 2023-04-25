// \brief Declaration of the class HttpServer

#include "HttpServer.h"
#include "httplib.h"

namespace NS_OSBASE::web {

    /*
     * \class HttpServer
     */
    HttpServer::HttpServer() {
        httplib::Server svr;
    }
} // namespace NS_OSBASE::web
