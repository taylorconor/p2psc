#pragma once

#include <string>

namespace p2psc {
namespace crypto {

class CryptoException : public std::exception {
public:
  CryptoException(const std::string &what) : _what(what) {}

  virtual const char *what() const noexcept override { return _what.c_str(); }

private:
  std::string _what;
};
}
}