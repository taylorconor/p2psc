#pragma once

#include <cassert>
#include <iostream>
#include <memory>

#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <p2psc/crypto/pki.h>
#include <p2psc/version.h>

namespace p2psc {
namespace crypto {

class RSA : public PKI {
public:
  static constexpr int max_data_size(std::uint16_t key_size) {
    return (key_size / 8) - 11;
  };
  static const int min_password_length = 4;

  static std::shared_ptr<RSA> from_public_key(const std::string &public_key);
  static std::shared_ptr<RSA> from_pem(const std::string &path);
  static std::shared_ptr<RSA> from_pem(const std::string &path,
                                       const std::string &password);
  static std::shared_ptr<RSA> generate();

  std::string public_encrypt(const std::string &message) const override;
  std::string public_decrypt(const std::string &message) const override;
  std::string private_encrypt(const std::string &message) const override;
  std::string private_decrypt(const std::string &message) const override;

  std::string get_public_key_string() const override;

  void write_to_file(const std::string &path) const override;
  void write_to_file(const std::string &path, const std::string &password,
                     const std::string &cipher) const override;

  ~RSA();

private:
  RSA(::RSA *key, bool has_private_key);

  ::RSA *_key;
  bool _has_private_key;
};
}
}