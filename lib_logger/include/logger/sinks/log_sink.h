#pragma once

#include <memory>
#include <string>
#include <vector>

namespace networklib {
namespace logging {

enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };

struct LogEntry; // Forward declaration

class LogSink {
public:
  virtual ~LogSink() = default;

  // Single entry write
  virtual void Write(const LogEntry &entry) = 0;

  // Batch write (default implementation calls Write repeatedly)
  // Override for better performance in sinks that support batch operations
  virtual void WriteBatch(const std::vector<LogEntry> &entries) {
    for (const auto &entry : entries) {
      Write(entry);
    }
  }

  // Flush buffered data
  virtual void Flush() = 0;

  // Check if sink supports batch operations efficiently
  virtual bool SupportsBatchWrite() const { return false; }
};
} // namespace logging
} // namespace networklib