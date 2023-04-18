// \brief Declaration of the class LogViewer

#pragma once
#include "osLog.h"
#include "osData/Uri.h"

namespace cho::osbase::logviewer {
    struct LogServiceSetting {
        data::Uri address;
        std::string realm;
    };

    struct Input {
        std::optional<std::string> fileName;
        std::optional<LogServiceSetting> logServiceSettings;
    };

    struct Output {
        std::optional<std::string> consoleColor;
        std::optional<std::string> fileName;

        bool operator==(const Output &other) const;
    };

    struct Field {
        std::string key;
        std::optional<std::string> type;

        bool operator==(const Field &other) const;
    };
    using Fields = std::vector<Field>;

    struct ColumnHeader {
        std::string name;
        Field field;
    };
    using ColumnHeaders = std::vector<ColumnHeader>;

    struct Columns {
        ColumnHeaders headers;
        std::string separator;
    };

    struct Operation {
        Field field;
        std::string operand;
        std::string value;
        std::optional<std::string> type;

        bool operator==(const Operation &other) const;
    };
    using Operations = std::vector<Operation>;

    struct Filter {
        std::optional<Operation> operation;
        std::optional<Operations> and;
        std::optional<Operations> or ;
        Output output;

        bool operator==(const Filter &other) const;
    };
    using Filters = std::vector<Filter>;

    struct Settings {
        Input input;
        Columns columns;
        std::optional<Filters> filters;
    };
} // namespace cho::osbase::logviewer
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::LogServiceSetting, address, realm)
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::Field, key, type)
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::ColumnHeader, name, field)
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::Columns, headers, separator)
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::Operation, field, operand, value)
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::Input, fileName, logServiceSettings)
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::Output, consoleColor, fileName)
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::Filter, operation, and, or, output)
OS_KEY_SERIALIZE_STRUCT(cho::osbase::logviewer::Settings, input, columns, filters)

template <>
struct std::less<cho::osbase::logviewer::Filter> {
    bool operator()(const cho::osbase::logviewer::Filter &lhs, const cho::osbase::logviewer::Filter &rhs) const;
};

namespace cho::osbase::logviewer {

    /**
     * \brief base class for stream writers
     */
    class StreamWriter {
    public:
        explicit StreamWriter(const Columns &columns);

        virtual ~StreamWriter() = default;
        virtual void write(const std::string &log);

    protected:
        virtual void doWrite(const std::string &log) = 0;

    private:
        Columns m_columns;
        bool m_bColumsWritten = false;
    };
    using StreamWriterPtr = std::shared_ptr<StreamWriter>;

    /**
     * \brief Output the log in a human readable format
     */
    class LogViewer {
    public:
        LogViewer();
        ~LogViewer();

        void run() const;

    private:
        class LogServiceObserver;
        using LogServiceObserverPtr = std::shared_ptr<LogServiceObserver>;

        void runFile(const std::filesystem::path &path) const;
        void runStream(const LogServiceSetting &logServiceSettings) const;

        void onLogServiceConnection(const bool bConnected);
        void onLog(std::string &&log) const;
        std::string writeLog(core::StringKeyStream &stream) const;

        static std::optional<std::string> getFieldValue(core::StringKeyStream &stream, const Field &field);
        static void setConsoleOutpuColor(const std::string &color);
        static bool isFilteredOK(core::StringKeyStream &stream, const Filter &filter);
        static bool performOperation(osbase::core::StringKeyStream &stream, const Operation &operation);

        Settings m_settings;
        log::ILogServicePtr m_pLogService;
        LogServiceObserverPtr m_pLogServiceObserver;
        data::AsyncData<std::string> m_asyncLog;
        mutable std::mutex m_mutLog;
        std::vector<std::pair<Filter, StreamWriterPtr>> m_streamWriters;
    };
} // namespace cho::osbase::logviewer
