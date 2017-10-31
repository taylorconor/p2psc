#pragma once

#include <string>

namespace p2psc {
namespace error {
enum Kind { kErrorMediatorConnectFailure, kErrorUnknown };
}
class Error {
public:
  Error() : _is_set(false) {}
  Error(const error::Kind kind, const std::string &reason)
      : _is_set(true), _kind(kind), _reason(reason) {}

  operator bool() const { return _is_set; }

  error::Kind kind() const { return _kind; }
  std::string reason() const { return _reason; }

private:
  bool _is_set;
  error::Kind _kind;
  std::string _reason;
};
}