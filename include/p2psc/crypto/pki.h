#pragma once

#include <string>

namespace p2psc {
namespace crypto {

class PKI {
public:
  virtual std::string public_encrypt(const std::string &) const = 0;
  virtual std::string public_decrypt(const std::string &) const = 0;
  virtual std::string private_encrypt(const std::string &) const = 0;
  virtual std::string private_decrypt(const std::string &) const = 0;

  virtual std::string get_public_key_string() const = 0;
};
}
}