#ifndef NETWORKLIB_UTILS_RESULT_H
#define NETWORKLIB_UTILS_RESULT_H

#include <string>
#include <variant>
#include <optional>
#include <stdexcept>
#include "networklib/constants/errors.h"

namespace networklib {
namespace utils {

class Error {
public:
    Error(constants::errors::ErrorCode code, std::string message)
        : code_(code), message_(std::move(message)) {}
    
    constants::errors::ErrorCode Code() const { return code_; }
    const std::string& Message() const { return message_; }
    
private:
    constants::errors::ErrorCode code_;
    std::string message_;
};

// Polyfill for std::expected (C++23)
template<typename T>
class Result {
public:
    // Constructors
    Result(T value) : value_(std::move(value)), has_value_(true) {}
    Result(Error error) : error_(std::move(error)), has_value_(false) {}

    // Observers
    bool HasValue() const { return has_value_; }
    operator bool() const { return has_value_; }

    const T& Value() const {
        if (!has_value_) throw std::runtime_error("Attempted to access value of error result");
        return *value_;
    }

    T& Value() {
        if (!has_value_) throw std::runtime_error("Attempted to access value of error result");
        return *value_;
    }

    const networklib::utils::Error& GetError() const {
        if (has_value_) throw std::runtime_error("Attempted to access error of success result");
        return *error_;
    }

    // Accessors
    T* operator->() { return &Value(); }
    const T* operator->() const { return &Value(); }
    T& operator*() { return Value(); }
    const T& operator*() const { return Value(); }

    static Result<T> Success(T value) { return Result<T>(std::move(value)); }
    static Result<T> Failure(constants::errors::ErrorCode code, std::string msg) {
        return Result<T>(networklib::utils::Error(code, std::move(msg)));
    }

private:
    std::optional<T> value_;
    std::optional<networklib::utils::Error> error_;
    bool has_value_;
};

// Void specialization
template<>
class Result<void> {
public:
    Result() : has_value_(true) {}
    Result(Error error) : error_(std::move(error)), has_value_(false) {}

    bool HasValue() const { return has_value_; }
    operator bool() const { return has_value_; }

    const networklib::utils::Error& GetError() const {
        if (has_value_) throw std::runtime_error("Attempted to access error of success result");
        return *error_;
    }

    static Result<void> Success() { return Result<void>(); }
    static Result<void> Failure(constants::errors::ErrorCode code, std::string msg) {
        return Result<void>(networklib::utils::Error(code, std::move(msg)));
    }

private:
    std::optional<networklib::utils::Error> error_;
    bool has_value_;
};

} // namespace utils
} // namespace networklib

#endif // NETWORKLIB_UTILS_RESULT_H
