#pragma once

#include "log_sink.hpp"
#include "../log_formatter.hpp"
#include <syslog.h>

namespace networklib {
namespace logging {

class SyslogSink : public LogSink {
public:
    explicit SyslogSink(const LoggerConfig& config) : config_(config) {
        openlog(config.app_name.c_str(), LOG_PID | LOG_NDELAY, LOG_USER);
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

        std::string formatted = LogFormatter::Format(entry, config_);
        syslog(priority, "%s", formatted.c_str());
    }

    void Flush() override {
        // Syslog flushes automatically usually
    }

private:
    LoggerConfig config_;
};

} // namespace logging
} // namespace networklib
