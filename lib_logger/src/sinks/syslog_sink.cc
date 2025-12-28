#include "logger/sinks/syslog_sink.h"
#include "logger/log_formatter.h"
#include <syslog.h>

namespace networklib {
namespace logging {

SyslogSink::SyslogSink(const LoggerConfig& config) : config_(config) {
    openlog(config.app_name.c_str(), LOG_PID | LOG_NDELAY, LOG_USER);
}

SyslogSink::~SyslogSink() {
    closelog();
}

void SyslogSink::Write(const LogEntry& entry) {
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

void SyslogSink::Flush() {
    // Syslog flushes automatically usually
}

} // namespace logging
} // namespace networklib
