#ifndef NETWORKLIB_CONSTANTS_ERRORS_H
#define NETWORKLIB_CONSTANTS_ERRORS_H

#include <cstdint>
#include <string_view>

namespace networklib {
namespace constants {
namespace errors {

// Error code type
using ErrorCode = int32_t;

// Error code ranges (following gRPC convention)
inline constexpr ErrorCode kOk = 0;

// Client errors (1-99)
inline constexpr ErrorCode kCancelled = 1;
inline constexpr ErrorCode kInvalidArgument = 2;
inline constexpr ErrorCode kDeadlineExceeded = 3;
inline constexpr ErrorCode kNotFound = 4;
inline constexpr ErrorCode kAlreadyExists = 5;
inline constexpr ErrorCode kPermissionDenied = 6;
inline constexpr ErrorCode kResourceExhausted = 7;
inline constexpr ErrorCode kFailedPrecondition = 8;
inline constexpr ErrorCode kAborted = 9;
inline constexpr ErrorCode kOutOfRange = 10;
inline constexpr ErrorCode kUnimplemented = 11;

// Server errors (100-199)
inline constexpr ErrorCode kInternal = 100;
inline constexpr ErrorCode kUnavailable = 101;
inline constexpr ErrorCode kDataLoss = 102;

// Network errors (200-299)
inline constexpr ErrorCode kConnectionFailed = 200;
inline constexpr ErrorCode kConnectionClosed = 201;
inline constexpr ErrorCode kConnectionTimeout = 202;
inline constexpr ErrorCode kDnsResolutionFailed = 203;
inline constexpr ErrorCode kSslHandshakeFailed = 204;

// Protocol errors (300-399)
inline constexpr ErrorCode kProtocolError = 300;
inline constexpr ErrorCode kInvalidMessage = 301;
inline constexpr ErrorCode kMessageTooLarge = 302;
inline constexpr ErrorCode kUnsupportedProtocol = 303;
inline constexpr ErrorCode kInvalidRequest = 304;

// Configuration errors (400-499)
inline constexpr ErrorCode kConfigError = 400;
inline constexpr ErrorCode kInvalidConfig = 401;
inline constexpr ErrorCode kMissingConfig = 402;

// Runtime errors (500-599)
inline constexpr ErrorCode kNotInitialized = 500;

} // namespace errors
} // namespace constants
} // namespace networklib

#endif // NETWORKLIB_CONSTANTS_ERRORS_H
