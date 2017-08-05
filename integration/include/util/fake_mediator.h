#pragma once

#include <include/util/local_listening_socket.h>
#include <p2psc/mediator.h>
#include <thread>
#include <vector>

namespace p2psc {
namespace integration {
namespace util {

class FakeMediator {
public:
  FakeMediator();
  FakeMediator(const p2psc::Mediator &mediator);
  ~FakeMediator();

  void run();
  void stop();

  p2psc::Mediator get_mediator_description() const;
  std::vector<std::string> get_received_messages() const;

private:
  LocalListeningSocket _socket;
  p2psc::Mediator _mediator;
  bool _is_running;
  std::thread _thread;
  std::vector<std::string> _received_messages;

  void _run();
  void _handle_connection(std::shared_ptr<Socket> session_socket);
};
}
}
}