// \brief implementation of the uri class

#include "osData/Uri.h"
#include <regex>
#include <sstream>

namespace {
    const std::regex uriRegex(
        "([a-z][a-z0-9+.-]*):(?:\\/\\/((?:(?=((?:[a-z0-9-._~!$&'()*+,;=:]|%[0-9A-F]{2})*))(\\3)@)?(?=(\\[[0-9A-F:.]{2,}"
        "\\]|(?:[a-z0-9-._~!$&'()*+,;=]|%[0-9A-F]{2})*))\\5(?::(?=(\\d*))\\6)?)(\\/(?=((?:[a-z0-9-._~!$&'()*+,;=:@\\/]|%[0-"
        "9A-F]{2})*))\\8)?|(\\/?(?!\\/)(?=((?:[a-z0-9-._~!$&'()*+,;=:@\\/]|%[0-9A-F]{2})*))\\10)?)(?:\\?(?=((?:[a-z0-9-._~!"
        "$&'()*+,;=:@\\/?]|%[0-9A-F]{2})*))\\11)?(?:#(?=((?:[a-z0-9-._~!$&'()*+,;=:@\\/?]|%[0-9A-F]{2})*))\\12)?",
        std::regex_constants::icase);

    constexpr size_t MATCH_INDEX_SCHEME                 = 1;
    constexpr size_t MATCH_INDEX_AUTHORITY              = 2;
    constexpr size_t MATCH_INDEX_USERINFO               = 4;
    constexpr size_t MATCH_INDEX_HOST                   = 5;
    constexpr size_t MATCH_INDEX_PORT                   = 6;
    constexpr size_t MATCH_INDEX_PATH_WITH_AUTHORITY    = 7;
    constexpr size_t MATCH_INDEX_PATH_WITHOUT_AUTHORITY = 9;
    constexpr size_t MATCH_INDEX_QUERY                  = 11;
    constexpr size_t MATCH_INDEX_FRAGMENT               = 12;
    constexpr size_t SIZE_MATCH                         = 13;

} // namespace

namespace cho::osbase::data {

    /*
     * \class Uri::Host
     */
    const std::unordered_map<std::string, std::string> Uri::Host::s_mapHost = { { "localhost", "127.0.0.1" } };

    Uri::Host::Host(const std::string &other) : m_host(other) {
    }

    Uri::Host &Uri::Host::operator=(const std::string &other) {
        m_host = other;
        return *this;
    }

    Uri::Host::operator std::string() const {
        if (auto const it = s_mapHost.find(m_host); it != s_mapHost.end()) {
            return it->second;
        }

        return m_host;
    }

    bool Uri::Host::empty() const {
        return m_host.empty();
    }

    bool Uri::Host::operator==(const std::string &other) const {
        return m_host == other || static_cast<std::string>(*this) == other;
    }

    bool Uri::Host::isLocal() const {
        return static_cast<std::string>(*this) == "127.0.0.1";
    }

    bool operator==(const std::string &lhs, const Uri::Host &rhs) {
        return rhs == lhs;
    }

    /*
     * \class Uri
     */
    Uri::Uri(const std::string &_scheme,
        const std::optional<Authority> &_authority,
        const std::optional<std::string> &_path,
        const std::optional<std::string> &_query,
        const std::optional<std::string> &_fragment)
        : scheme(_scheme), authority(_authority), path(_path), query(_query), fragment(_fragment) {
        if (path) {
            // Replace "\\" by "/"
            std::string strPath = *path;
            std::replace(strPath.begin(), strPath.end(), '\\', '/');
            path = strPath;
        }
    }

    bool Uri::operator==(const Uri &other) const noexcept {
        try {
            if (m_bNull || other.m_bNull) {
                return m_bNull && other.m_bNull;
            }

            return m_bNull && other.m_bNull || type_cast<std::string>(*this) == type_cast<std::string>(other);
        } catch (const BadUriException &) {
            return false;
        }
    }

    bool Uri::operator!=(const Uri &other) const noexcept {
        return !(*this == other);
    }

    bool Uri::isValid() const {

        return isValid(type_cast<std::string>(*this));
    }

    bool Uri::isValid(const std::string &strUri) {
        try {
            auto const uri = type_cast<Uri>(strUri);
            return true;
        } catch (const BadUriException &) {
            return false;
        }
    }

    const Uri &Uri::null() noexcept {
        static auto const nullUri = []() {
            Uri uri{};
            uri.m_bNull = true;
            return uri;
        }();
        return nullUri;
    }

    const std::string &Uri::schemeFile() noexcept {
        static const std::string schemeName = "file";
        return schemeName;
    }

    const std::string &Uri::schemeWebsocket() noexcept {
        static const std::string schemeName = "ws";
        return schemeName;
    }

    const std::string &Uri::schemeHyperTextTransferProtocol() noexcept {
        static const std::string schemeName = "http";
        return schemeName;
    }

    const std::string &Uri::schemeHyperTextTransferProtocolSecure() noexcept {
        static const std::string schemeName = "https";
        return schemeName;
    }

    const std::string &Uri::schemeFileTransferProtocol() noexcept {
        static const std::string schemeName = "ftp";
        return schemeName;
    }
} // namespace cho::osbase::data

namespace nsosbase = cho::osbase;
namespace nscore   = nsosbase::core;
namespace nsdata   = nsosbase::data;

nsdata::Uri nscore::KeySerializer<std::string, nsdata::Uri, false>::getValue(
    nscore::KeyStream<std::string> &keyStream, const nsdata::Uri &defaultValue) {
    try {
        return type_cast<nsdata::Uri>(keyStream.getValue(std::string()));
    } catch (const nsdata::BadUriException &) {
        return defaultValue;
    }
}

bool nscore::KeySerializer<std::string, nsdata::Uri, false>::setValue(KeyStream<std::string> &keyStream, const data::Uri &value) {
    return keyStream.setValue(type_cast<std::string>(value));
}

std::string type_converter<std::string, nsdata::Uri>::convert(const nsdata::Uri &uri) {
    if (uri == nsdata::Uri::null()) {
        return "";
    }

    std::ostringstream oss;

    oss << uri.scheme << ":";
    if (uri.authority) {
        oss << "//";
        if ((*uri.authority).userInfo) {
            oss << *uri.authority->userInfo << "@";
        }
        oss << static_cast<std::string>((*uri.authority).host);
        if ((*uri.authority).port) {
            oss << ":" << *uri.authority->port;
        }
    }
    if (uri.path) {
        oss << *uri.path;
    }

    if (uri.query) {
        oss << "?" << *uri.query;
    }

    return oss.str();
}

nsdata::Uri type_converter<nsdata::Uri, std::string>::convert(const std::string &strUri) {
    nsdata::Uri uri{};

    // check lexical validity
    if (std::smatch sm; std::regex_match(strUri, sm, uriRegex) && sm.size() == SIZE_MATCH) {
        uri.scheme = sm[MATCH_INDEX_SCHEME];
        if (sm[MATCH_INDEX_AUTHORITY].matched) {
            nsdata::Uri::Authority authority;
            if (sm[MATCH_INDEX_USERINFO].matched) {
                authority.userInfo = sm[MATCH_INDEX_USERINFO];
            }
            authority.host = sm[MATCH_INDEX_HOST];
            if (sm[MATCH_INDEX_PORT].matched) {
                authority.port = static_cast<unsigned short>(std::stoul(sm[MATCH_INDEX_PORT]));
            }
            uri.authority = authority;

            if (sm[MATCH_INDEX_PATH_WITH_AUTHORITY].matched) {
                uri.path = sm[MATCH_INDEX_PATH_WITH_AUTHORITY];
            }
        } else if (sm[MATCH_INDEX_PATH_WITHOUT_AUTHORITY].matched) {
            uri.path = sm[MATCH_INDEX_PATH_WITHOUT_AUTHORITY];
        }

        if (sm[MATCH_INDEX_QUERY].matched) {
            uri.query = sm[MATCH_INDEX_QUERY];
        }

        if (sm[MATCH_INDEX_FRAGMENT].matched) {
            uri.fragment = sm[MATCH_INDEX_FRAGMENT];
        }
    } else {
        throw nsdata::BadUriException("Uri not valid");
    }

    // check field validity
    const bool bUriFieldValid = uri.scheme == nsdata::Uri::schemeFile()
                                    ? !uri.authority || uri.authority->host.empty() || uri.authority->host == "localhost"
                                    : uri.authority && !uri.authority->host.empty();

    if (!bUriFieldValid) {
        throw nsdata::BadUriException("Authority not valid for '" + uri.scheme + "' scheme");
    }

    return uri;
}

std::wstring type_converter<std::wstring, cho::osbase::data::Uri>::convert(const cho::osbase::data::Uri &uri) {
    auto const strUri = type_cast<std::string>(uri);
    return type_cast<std::wstring>(strUri);
}

cho::osbase::data::Uri type_converter<cho::osbase::data::Uri, std::wstring>::convert(const std::wstring &wStrUri) {
    auto const strUri = type_cast<std::string>(wStrUri);
    return type_cast<cho::osbase::data::Uri>(strUri);
}

std::filesystem::path type_converter<std::filesystem::path, nsdata::Uri>::convert(const nsdata::Uri uri) {
    if (uri.scheme != nsdata::Uri::schemeFile()) {
        throw nsdata::BadUriException("The scheme of the uri is not 'file'");
    }

    if (!uri.path) {
        throw nsdata::BadUriException("The uri has no path");
    }

    return *uri.path;
}

nsdata::Uri type_converter<cho::osbase::data::Uri, std::filesystem::path>::convert(const std::filesystem::path &path) {
    return nsdata::Uri{ nsdata::Uri::schemeFile(), {}, path.string(), {}, {} };
}

size_t std::hash<nsdata::Uri>::operator()(const nsdata::Uri &uri) const {
    return std::hash<std::string>()(type_cast<std::string>(uri));
}

bool std::less<nsdata::Uri>::operator()(const nsdata::Uri &lhs, const nsdata::Uri &rhs) const {
    return std::less<>()(type_cast<std::string>(lhs), type_cast<std::string>(rhs));
}

std::ostream &operator<<(std::ostream &os, const cho::osbase::data::Uri &uri) {
    os << type_cast<std::string>(uri);
    return os;
}
