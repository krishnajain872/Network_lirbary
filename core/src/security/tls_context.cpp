#include "networklib/security/tls_context.h"
#include "networklib/constants/errors.h"
#include "networklib/network_lib.h"
#include <iostream>

namespace networklib {
namespace security {

TlsContext::TlsContext() : ctx_(nullptr) {
}

TlsContext::~TlsContext() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (ctx_) {
        SSL_CTX_free(ctx_);
    }
}

utils::Result<void> TlsContext::Init(const std::string& cert_path, const std::string& key_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (ctx_) {
        SSL_CTX_free(ctx_);
        ctx_ = nullptr;
    }

    ctx_ = SSL_CTX_new(TLS_server_method());
    if (!ctx_) {
        return utils::Result<void>::Failure(constants::errors::kInternal, "Failed to create SSL_CTX");
    }

    SSL_CTX_set_min_proto_version(ctx_, TLS1_2_VERSION);

    if (SSL_CTX_use_certificate_chain_file(ctx_, cert_path.c_str()) <= 0) {
        ERR_print_errors_fp(stderr);
        return utils::Result<void>::Failure(constants::errors::kConfigError, "Failed to load certificate file");
    }

    if (SSL_CTX_use_PrivateKey_file(ctx_, key_path.c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return utils::Result<void>::Failure(constants::errors::kConfigError, "Failed to load private key file");
    }

    if (!SSL_CTX_check_private_key(ctx_)) {
        return utils::Result<void>::Failure(constants::errors::kConfigError, "Private key does not match certificate");
    }

    return utils::Result<void>::Success();
}

utils::Result<void> TlsContext::ReloadCertificates(const std::string& cert_path, const std::string& key_path) {
    // We create a NEW context to validate first, then swap
    // This ensures atomic-ish swap and no broken state if file is bad
    SSL_CTX* new_ctx = SSL_CTX_new(TLS_server_method());
    if (!new_ctx) {
        return utils::Result<void>::Failure(constants::errors::kInternal, "Failed to create temp SSL_CTX");
    }
    
    SSL_CTX_set_min_proto_version(new_ctx, TLS1_2_VERSION);

    if (SSL_CTX_use_certificate_chain_file(new_ctx, cert_path.c_str()) <= 0) {
        SSL_CTX_free(new_ctx);
        return utils::Result<void>::Failure(constants::errors::kConfigError, "Failed to load new certificate");
    }

    if (SSL_CTX_use_PrivateKey_file(new_ctx, key_path.c_str(), SSL_FILETYPE_PEM) <= 0) {
        SSL_CTX_free(new_ctx);
        return utils::Result<void>::Failure(constants::errors::kConfigError, "Failed to load new private key");
    }

    if (!SSL_CTX_check_private_key(new_ctx)) {
        SSL_CTX_free(new_ctx);
        return utils::Result<void>::Failure(constants::errors::kConfigError, "New private key mismatch");
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (ctx_) SSL_CTX_free(ctx_);
        ctx_ = new_ctx;
    }
    
    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "[Security] TLS Certificates Reloaded Successfully");
    return utils::Result<void>::Success();
}

SSL* TlsContext::CreateSsl() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!ctx_) return nullptr;
    return SSL_new(ctx_);
}

} // namespace security
} // namespace networklib
