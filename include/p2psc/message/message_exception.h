#pragma once

#include <string>

namespace p2psc {
namespace message {

class MessageException : public std::exception {
public:
  MessageException(const std::string &what) : _what(what) {}

  virtual const char *what() const noexcept override { return _what.c_str(); }

private:
  std::string _what;
};
}
}