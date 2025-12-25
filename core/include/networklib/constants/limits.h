#ifndef NETWORKLIB_CONSTANTS_LIMITS_H
#define NETWORKLIB_CONSTANTS_LIMITS_H

#include <cstddef>

namespace networklib {
namespace constants {
namespace limits {

// Connection limits
inline constexpr std::size_t kMaxConnections = 10'000'000'000ULL;  // 10 billion
inline constexpr std::size_t kMaxConnectionsPerHost = 1'000'000ULL;
inline constexpr std::size_t kMinConnectionPoolSize = 10ULL;
inline constexpr std::size_t kMaxConnectionPoolSize = 100'000ULL;

// Memory limits
inline constexpr std::size_t kMaxPayloadSize = 10ULL * 1024 * 1024;  // 10MB
inline constexpr std::size_t kMaxHeaderSize = 64ULL * 1024;           // 64KB
inline constexpr std::size_t kBufferSize = 8192ULL;                   // 8KB
inline constexpr std::size_t kMaxBufferSize = 1ULL * 1024 * 1024;    // 1MB

// Thread limits
inline constexpr std::size_t kMinThreads = 1ULL;
inline constexpr std::size_t kMaxThreads = 256ULL;
inline constexpr std::size_t kDefaultThreads = 0ULL;  // 0 = auto-detect

// Queue limits
inline constexpr std::size_t kDefaultQueueSize = 10'000ULL;
inline constexpr std::size_t kMaxQueueSize = 1'000'000ULL;

} // namespace limits
} // namespace constants
} // namespace networklib

#endif // NETWORKLIB_CONSTANTS_LIMITS_H
