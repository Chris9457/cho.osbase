// \brief Declaration of the class LogViewer

#include "logviewer.h"
#include "logviewer.h"
#include "logviewerres.h"
#include <fstream>
#include <sstream>

namespace NS_OSBASE::logviewer {

    /*
     * \class LogViewer::LogServiceObserver
     */
    class LogViewer::LogServiceObserver : public core::Observer<application::ServiceConnectionMsg> {
    public:
        LogServiceObserver(LogViewer &logViewer) : m_logViewer(logViewer) {
        }

        void update(const core::Observable &, const application::ServiceConnectionMsg &msg) override {
            m_logViewer.onLogServiceConnection(msg.isConnected());
        }

    private:
        LogViewer &m_logViewer;
    };

    /*
     * \class LogViewer::LogServiceObserver
     */
    StreamWriter::StreamWriter(const Columns &columns) : m_columns(columns) {
    }

    void StreamWriter::write(const std::string &log) {
        if (!m_bColumsWritten) {
            m_bColumsWritten = true;

            bool bFirstHeader = true;
            std::ostringstream oss;
            for (auto const &header : m_columns.headers) {
                if (bFirstHeader) {
                    bFirstHeader = false;
                } else {
                    oss << m_columns.separator;
                }
                oss << header.name;
            }

            doWrite(oss.str());
        }

        doWrite(log);
    }

    /*
     * \class LogViewer::LogServiceObserver
     */
    class ConsoleWriter : public StreamWriter {
    public:
        ConsoleWriter(const Columns &columns, const std::string &color) : StreamWriter(columns) {
            std::string lowerColor;
            for (auto const &c : color) {
                lowerColor += static_cast<char>(std::tolower(c));
            }
            auto const itConsoleColor = consoleOutputColor.find(lowerColor);

            if (itConsoleColor != consoleOutputColor.cend()) {
                m_consoleColor = itConsoleColor->second;
            }
        }

        void doWrite(const std::string &log) override {
            static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            SetConsoleTextAttribute(hConsole, static_cast<WORD>(m_consoleColor));
            std::cout << log << std::endl;
            SetConsoleTextAttribute(hConsole, static_cast<WORD>(ConsoleColor::Darkgray));
        }

    private:
        ConsoleColor m_consoleColor = ConsoleColor::Darkgray;
    };

    class FileWriter : public StreamWriter {
    public:
        FileWriter(const Columns &columns, const std::filesystem::path &path) : StreamWriter(columns), m_ofs(path) {
        }

        void doWrite(const std::string &log) override {
            m_ofs << log << std::endl;
        }

    private:
        std::ofstream m_ofs;
    };

    /*
     * \class LogViewer
     */
    LogViewer::LogViewer() : m_pLogService(log::makeStub()), m_pLogServiceObserver(std::make_shared<LogServiceObserver>(*this)) {
        auto const pStream = core::makeJsonStream(std::ifstream(SETTING_FILE_NAME));
        m_settings         = pStream->getValue(Settings{});

        if (m_settings.filters.has_value()) {
            for (auto const &filter : m_settings.filters.value()) {
                if (filter.output.consoleColor.has_value()) {
                    m_streamWriters.push_back({ filter, std::make_shared<ConsoleWriter>(m_settings.columns, *filter.output.consoleColor) });
                }
                if (filter.output.fileName.has_value()) {
                    m_streamWriters.push_back({ filter, std::make_shared<FileWriter>(m_settings.columns, *filter.output.fileName) });
                }
            }
        }
    }

    LogViewer::~LogViewer() {
        m_pLogService->detachAll(*m_pLogServiceObserver);
    }

    void LogViewer::run() const {
        if (m_settings.input.fileName.has_value()) {
            runFile(m_settings.input.fileName.value());
        }

        if (m_settings.input.logServiceSettings.has_value()) {
            runStream(m_settings.input.logServiceSettings.value());
        }
    }

    void LogViewer::runFile(const std::filesystem::path &path) const {
        std::cout << "******************************************************" << std::endl;
        std::cout << "***              LOG VIEWER FILE                   ***" << std::endl;
        std::cout << "******************************************************" << std::endl;
        std::cout << std::endl;

        if (!exists(path)) {
            std::cout << "The file \"" << path.string() << "\" doesn't exist" << std::endl;
            return;
        }

        std::cout << "Read the file \"" << path.string() << "\"" << std::endl;
        std::cout << std::endl;

        std::ifstream inputFile(path);
        std::string jsonLine;
        while (std::getline(inputFile, jsonLine)) {
            onLog(std::string(jsonLine));
        }

        std::cout << std::endl;
    }

    void LogViewer::runStream(const LogServiceSetting &logServiceSettings) const {
        static const std::string strStop = "stop";
        {
            std::lock_guard lock(m_mutLog);
            std::cout << "******************************************************" << std::endl;
            std::cout << "***              LOG VIEWER STREAMING              ***" << std::endl;
            std::cout << "******************************************************" << std::endl;
            std::cout << std::endl;

            std::cout << "Type \"stop\" to stop the Log Viewer" << std::endl;
            std::cout << std::endl;

            auto const brokerUri = data::Uri(logServiceSettings.address);
            m_pLogService->attachAll(*m_pLogServiceObserver);
            m_pLogService->connect(brokerUri.authority->host, *brokerUri.authority->port);
        }

        auto const guard = core::make_scope_exit([this]() {
            std::lock_guard lock(m_mutLog);
            m_pLogService->disconnect();
            m_pLogService->detachAll(*m_pLogServiceObserver);
        });

        while (true) {
            std::cout << std::endl;
            std::string line;
            std::getline(std::cin, line);
            std::cout << std::endl;
            if (line == strStop) {
                break;
            }
            std::cout << "Type \"stop\" to stop the Log Viewer" << std::endl;
        }
    }

    void LogViewer::onLogServiceConnection(const bool bConnected) {
        if (bConnected) {
            std::cout << "*** Connected ***" << std::endl;
            std::cout << std::endl;
            m_asyncLog = data::AsyncData<std::string>(m_pLogService->getOutputLogUri());
            m_asyncLog.get([this](std::string &&log) { onLog(std::move(log)); });
        } else {
            std::cout << "*** Disconnected ***" << std::endl;
            std::cout << std::endl;
        }
    }

    void LogViewer::onLog(std::string &&log) const {
        std::lock_guard lock(m_mutLog);

        if (m_settings.filters.has_value()) {
            auto const pStream = core::makeJsonStream(std::istringstream(log));
            for (auto const &filter : *m_settings.filters) {
                if (isFilteredOK(*pStream, filter)) {
                    writeLog(*pStream);
                    for (auto const streamWriter : m_streamWriters) {
                        if (streamWriter.first == filter) {
                            streamWriter.second->write(writeLog(*pStream));
                        }
                    }
                    break;
                }
            }
        }
    }

    std::string LogViewer::writeLog(core::StringKeyStream &stream) const {

        std::ostringstream oss;

        bool bFirstField = true;
        for (auto const &header : m_settings.columns.headers) {
            auto const fieldValue = getFieldValue(stream, header.field);

            if (bFirstField) {
                bFirstField = false;
            } else {
                oss << m_settings.columns.separator;
            }

            if (fieldValue.has_value()) {
                oss << *fieldValue;
            }
        }

        return oss.str();
    }

    std::optional<std::string> LogViewer::getFieldValue(core::StringKeyStream &stream, const Field &field) {
        size_t nbClose   = 0;
        auto const guard = core::make_scope_exit([&stream, &nbClose]() {
            while (nbClose != 0) {
                stream.closeKey();
                --nbClose;
            }
        });

        std::string key;
        std::istringstream iss(field.key);
        while (std::getline(iss, key, '/')) {
            if (key.empty()) {
                continue;
            }

            if (!stream.isKeyExist(key)) {
                return {};
            }
            stream.openKey(key);
            ++nbClose;
        }

        std::string value = "defaultValue";
        if (field.type.has_value()) {
            auto const type = field.type.value();
            if (type == "string") {
                value = stream.getValue(value);
            } else if (type == "int") {
                value = std::to_string(stream.getValue(0));
            } else if (type == "double") {
                value = std::to_string(stream.getValue(0.));
            } else if (type == "bool") {
                value = std::to_string(stream.getValue(false));
            }
        } else {
            value = stream.getValue(value);
        }

        return value;
    }

    void LogViewer::setConsoleOutpuColor(const std::string &color) {
        static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        std::string lowerColor;
        for (auto const &c : color) {
            lowerColor += static_cast<char>(std::tolower(c));
        }
        auto const itConsoleColor = consoleOutputColor.find(lowerColor);

        if (itConsoleColor != consoleOutputColor.cend()) {
            SetConsoleTextAttribute(hConsole, static_cast<WORD>(itConsoleColor->second));
        }
    }

    bool LogViewer::isFilteredOK(core::StringKeyStream &stream, const Filter &filter) {
        auto isOperationOK = [&stream](const Operation &operation) -> bool { return performOperation(stream, operation); };

        if (filter.operation.has_value()) {
            return isOperationOK(*filter.operation);
        }

        if (filter.and.has_value()) {
            return std::all_of(filter.and->cbegin(), filter.and->cend(), isOperationOK);
        }

        if (filter.or.has_value()) {
            return std::any_of(filter.or->cbegin(), filter.or->cend(), isOperationOK);
        }

        return false;
    }

    bool LogViewer::performOperation(core::StringKeyStream &stream, const Operation &operation) {
        std::string lowerOperand;
        for (auto const &c : operation.operand) {
            lowerOperand += static_cast<char>(std::tolower(c));
        }
        auto const itOperand = operands.find(lowerOperand);

        if (itOperand == operands.cend()) {
            return false;
        }

        auto const operand = itOperand->second;
        if (operand == Operands::containKey || operand == Operands::inKey) {
            size_t nbClose                         = 0;
            auto const guard                       = core::make_scope_exit([&stream, &nbClose]() {
                while (nbClose != 0) {
                    stream.closeKey();
                    --nbClose;
                }
            });
            auto const [keyContainer, keyToToTest] = operand == Operands::containKey ? std::make_pair(operation.field.key, operation.value)
                                                                                     : std::make_pair(operation.value, operation.field.key);
            std::string key;
            std::istringstream iss(keyContainer);
            while (std::getline(iss, key, '/')) {
                if (key.empty()) {
                    continue;
                }

                if (!stream.isKeyExist(key)) {
                    return false;
                }

                stream.openKey(key);
                ++nbClose;
            }

            return stream.isKeyExist(keyToToTest);
        }

        auto const fieldValue = getFieldValue(stream, operation.field);
        if (!fieldValue.has_value()) {
            return false;
        }

        auto const &lhs = fieldValue.value();
        auto const &rhs = operation.value;

        switch (operand) {
        case Operands::eq:
            return std::equal_to()(lhs, rhs);
        case Operands::ne:
            return std::not_equal_to()(lhs, rhs);
        case Operands::lt:
            return std::less()(lhs, rhs);
        case Operands::le:
            return std::less_equal()(lhs, rhs);
        case Operands::gt:
            return std::greater()(lhs, rhs);
        case Operands::ge:
            return std::greater_equal()(lhs, rhs);
        case Operands::contain:
            return lhs.find(rhs) != std::string::npos;
        case Operands::in:
            return rhs.find(lhs) != std::string::npos;
        default:
            return false;
        }
    }

    /*
     * Comparison
     */

    bool Output::operator==(const Output &other) const {
        static const std::string undefined = "undefined";
        return consoleColor.value_or(undefined) == other.consoleColor.value_or(undefined) &&
               fileName.value_or(undefined) == other.fileName.value_or(undefined);
    }

    bool Field::operator==(const Field &other) const {
        return key == other.key;
    }

    bool Operation::operator==(const Operation &other) const {
        return field == other.field && operand == other.operand && value == other.value;
    }

    bool Filter::operator==(const Filter &other) const {
        return operation.value_or(Operation{}) == other.operation.value_or(Operation{}) &&
                   and.value_or(Operations{}) == other.and.value_or(Operations{}) &&
               or.value_or(Operations{}) == other.or.value_or(Operations{}) && output == other.output;
    }
} // namespace NS_OSBASE::logviewer
