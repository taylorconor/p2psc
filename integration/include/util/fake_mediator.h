#pragma once

#include <include/util/id_to_address_store.h>
#include <include/util/local_listening_socket.h>
#include <p2psc/mediator.h>
#include <p2psc/message.h>
#include <p2psc/message/types.h>
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

  void quit_after(message::MessageType message_type);

  p2psc::Mediator get_mediator_description() const;
  std::vector<std::string> get_received_messages() const;
  std::vector<std::string> get_sent_messages() const;

private:
  LocalListeningSocket _socket;
  p2psc::Mediator _mediator;
  IdToAddressStore _id_to_address_store;
  bool _is_running;
  message::MessageType _quit_after;
  std::thread _worker_thread;
  std::vector<std::thread> _handler_pool;
  std::vector<std::string> _received_messages;
  std::vector<std::string> _sent_messages;

  void _run();
  void _handle_connection(std::shared_ptr<Socket> session_socket);
  template <class T>
  void _send_and_log(std::shared_ptr<Socket> socket, const Message<T> &message);
  template <class T>
  Message<T> _receive_and_log(std::shared_ptr<Socket> socket);
};
}
}
}