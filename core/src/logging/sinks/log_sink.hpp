#pragma once

#include "../log_common.hpp"

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
