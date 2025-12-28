#ifndef NETWORKLIB_CONSTANTS_DEFAULTS_H
#define NETWORKLIB_CONSTANTS_DEFAULTS_H

#include <chrono>
#include <string_view>

namespace networklib {
namespace constants {
namespace defaults {

// Timeout defaults
inline constexpr auto kConnectionTimeout = std::chrono::milliseconds(5000);
inline constexpr auto kRequestTimeout = std::chrono::milliseconds(30000);
inline constexpr auto kIdleTimeout = std::chrono::minutes(5);
inline constexpr auto kKeepAliveInterval = std::chrono::seconds(30);

// Network defaults
inline constexpr std::string_view kDefaultHost = "0.0.0.0";
inline constexpr uint16_t kDefaultHttpPort = 8080;
inline constexpr uint16_t kDefaultHttpsPort = 8443;
inline constexpr uint16_t kDefaultGrpcPort = 50051;

// Protocol defaults
inline constexpr std::string_view kDefaultProtocol = "tcp";
inline constexpr std::string_view kDefaultCompression = "gzip";
inline constexpr std::string_view kDefaultEncoding = "utf-8";

// Performance defaults
inline constexpr bool kDefaultUseIOUring = true;
inline constexpr bool kDefaultUseZeroCopy = true;
inline constexpr bool kDefaultEnableNuma = true;

// Retry defaults
inline constexpr std::size_t kDefaultMaxRetries = 3;
inline constexpr auto kDefaultInitialBackoff = std::chrono::milliseconds(100);
inline constexpr auto kDefaultMaxBackoff = std::chrono::seconds(10);
inline constexpr double kDefaultBackoffMultiplier = 2.0;

} // namespace defaults
} // namespace constants
} // namespace networklib

#endif // NETWORKLIB_CONSTANTS_DEFAULTS_H
