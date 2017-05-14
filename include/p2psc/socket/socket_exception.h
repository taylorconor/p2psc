#pragma once

#include <string>

namespace p2psc {
namespace socket {

class SocketException : public std::exception {
public:
  SocketException(const std::string &what) : _what(what) {}

  virtual const char *what() const noexcept override { return _what.c_str(); }

private:
  std::string _what;
};
}
}