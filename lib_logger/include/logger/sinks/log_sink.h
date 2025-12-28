#pragma once

#include "logger/log_common.h"

namespace networklib {
namespace logging {

class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void Write(const LogEntry& entry) = 0;
    virtual void Flush() = 0;
};

} // namespace logging
} // namespace networklib
