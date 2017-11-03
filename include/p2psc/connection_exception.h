#pragma once

#include <p2psc/error.h>
#include <string>

namespace p2psc {

class ConnectionException : public std::exception {
public:
  ConnectionException(const Error &error) : _error(error) {}

  virtual Error error() const noexcept { return _error; }
  virtual const char *what() const noexcept override {
    return _error.reason().c_str();
  }

private:
  Error _error;
};
}