#pragma once

#include "log_sink.hpp"
#include <syslog.h>

namespace networklib {
namespace logging {

class SyslogSink : public LogSink {
public:
    explicit SyslogSink(const std::string& ident) {
        openlog(ident.c_str(), LOG_PID | LOG_NDELAY, LOG_USER);
    }

    ~SyslogSink() override {
        closelog();
    }

    void Write(const LogEntry& entry) override {
        int priority = LOG_INFO;
        switch (entry.level) {
            case LogLevel::Trace: priority = LOG_DEBUG; break;
            case LogLevel::Debug: priority = LOG_DEBUG; break;
            case LogLevel::Info: priority = LOG_INFO; break;
            case LogLevel::Warn: priority = LOG_WARNING; break;
            case LogLevel::Error: priority = LOG_ERR; break;
            case LogLevel::Fatal: priority = LOG_CRIT; break;
            case LogLevel::Critical: priority = LOG_EMERG; break;
        }
        syslog(priority, "%s", entry.message.c_str());
    }

    void Flush() override {
        // Syslog flushes automatically usually
    }
};

} // namespace logging
} // namespace networklib
