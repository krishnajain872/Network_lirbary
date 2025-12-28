#ifndef NETWORKLIB_SECURITY_TLS_CONTEXT_H
#define NETWORKLIB_SECURITY_TLS_CONTEXT_H

#include "networklib/utils/result.h"
#include <memory>
#include <mutex>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string>


namespace networklib {
namespace security {

enum class SslMode { kServer, kClient };

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
   * @brief Initializes the SSL context with certificate and key (for server
   * mode).
   *
   * @param cert_path Path to the server certificate chain (PEM).
   * @param key_path Path to the private key (PEM).
   * @return utils::Result<void> Success or error details.
   */
  utils::Result<void> Init(const std::string &cert_path,
                           const std::string &key_path);

  /**
   * @brief Initializes the SSL context for client or server mode.
   *
   * @param mode SSL mode (client or server).
   * @param cert_path Path to the certificate chain (PEM). Optional for client
   * mode.
   * @param key_path Path to the private key (PEM). Optional for client mode.
   * @return utils::Result<void> Success or error details.
   */
  utils::Result<void> Init(SslMode mode, const std::string &cert_path = "",
                           const std::string &key_path = "");

  /**
   * @brief Reloads certificates at runtime (server mode only).
   *
   * @param cert_path Path to the server certificate chain (PEM).
   * @param key_path Path to the private key (PEM).
   * @return utils::Result<void> Success or error.
   */
  utils::Result<void> ReloadCertificates(const std::string &cert_path,
                                         const std::string &key_path);

  /**
   * @brief Creates a new SSL structure for a connection.
   *
   * @return SSL* The raw OpenSSL structure, or nullptr on failure.
   */
  SSL *CreateSsl();

  /**
   * @brief Returns the raw OpenSSL Context.
   */
  SSL_CTX *NativeHandle() const { return ctx_; }

private:
  SSL_CTX *ctx_;
  std::mutex mutex_;
  SslMode mode_;
};

} // namespace security
} // namespace networklib

#endif // NETWORKLIB_SECURITY_TLS_CONTEXT_H
