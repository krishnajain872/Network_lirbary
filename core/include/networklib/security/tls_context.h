#ifndef NETWORKLIB_SECURITY_TLS_CONTEXT_H
#define NETWORKLIB_SECURITY_TLS_CONTEXT_H

#include <string>
#include <memory>
#include <mutex>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "networklib/utils/result.h"

namespace networklib {
namespace security {

/**
 * @brief Wrapper around OpenSSL SSL_CTX.
 * 
 * Manages the TLS configuration, certificates, and private keys.
 */
class TlsContext {
public:
    using Ptr = std::shared_ptr<TlsContext>;

    TlsContext();
    ~TlsContext();

    /**
     * @brief Initializes the SSL context with certificate and key.
     * 
     * @param cert_path Path to the server certificate chain (PEM).
     * @param key_path Path to the private key (PEM).
     * @return utils::Result<void> Success or error details.
     */
    utils::Result<void> Init(const std::string& cert_path, const std::string& key_path);

    /**
     * @brief Reloads certificates at runtime.
     * 
     * @param cert_path Path to the server certificate chain (PEM).
     * @param key_path Path to the private key (PEM).
     * @return utils::Result<void> Success or error.
     */
    utils::Result<void> ReloadCertificates(const std::string& cert_path, const std::string& key_path);

    /**
     * @brief Creates a new SSL structure for a connection.
     * 
     * @return SSL* The raw OpenSSL structure, or nullptr on failure.
     */
    SSL* CreateSsl();

    /**
     * @brief Returns the raw OpenSSL Context.
     */
    SSL_CTX* NativeHandle() const { return ctx_; }

private:
    SSL_CTX* ctx_;
    std::mutex mutex_;
};

} // namespace security
} // namespace networklib

#endif // NETWORKLIB_SECURITY_TLS_CONTEXT_H
