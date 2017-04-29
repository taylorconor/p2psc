#pragma once

#include <string>

namespace p2psc {

/**
 * This struct contains the data needed to identify a Mediator.
 */
struct Mediator {
  const std::string ip;
  const uint16_t port;

  Mediator(const std::string &ip, const uint16_t port) : ip(ip), port(port) {}

  std::string toString() const { return ip + ":" + std::to_string(port); }
};
}