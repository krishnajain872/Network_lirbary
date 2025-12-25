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
            case LogLevel::TRACE: priority = LOG_DEBUG; break;
            case LogLevel::DEBUG: priority = LOG_DEBUG; break;
            case LogLevel::INFO: priority = LOG_INFO; break;
            case LogLevel::WARN: priority = LOG_WARNING; break;
            case LogLevel::ERROR: priority = LOG_ERR; break;
            case LogLevel::FATAL: priority = LOG_CRIT; break;
            case LogLevel::CRITICAL: priority = LOG_EMERG; break;
        }
        syslog(priority, "%s", entry.message.c_str());
    }

    void Flush() override {
        // Syslog flushes automatically usually
    }
};

} // namespace logging
} // namespace networklib
