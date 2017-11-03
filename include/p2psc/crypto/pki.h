#pragma once

#include <string>

namespace p2psc {
namespace crypto {

class PKI {
public:
  virtual std::string public_encrypt(const std::string &message) const = 0;
  virtual std::string public_decrypt(const std::string &message) const = 0;
  virtual std::string private_encrypt(const std::string &message) const = 0;
  virtual std::string private_decrypt(const std::string &message) const = 0;

  virtual std::string get_public_key_string() const = 0;

  virtual void write_to_file(const std::string &path) const = 0;
  virtual void write_to_file(const std::string &path,
                             const std::string &password,
                             const std::string &cipher) const = 0;

  virtual ~PKI() {}
};
}
}