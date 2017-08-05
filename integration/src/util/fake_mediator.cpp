#include <include/util/fake_mediator.h>
#include <p2psc/log.h>
#include <p2psc/message/message_decoder.h>

namespace p2psc {
namespace integration {
namespace util {

FakeMediator::FakeMediator() : _mediator(_socket.get_socket_address()) {}

FakeMediator::FakeMediator(const p2psc::Mediator &mediator)
    : _mediator(mediator) {}

FakeMediator::~FakeMediator() {
  if (_is_running) {
    stop();
  }
}

void FakeMediator::run() {
  BOOST_ASSERT(!_is_running);
  _is_running = true;
  _thread = std::thread(&FakeMediator::_run, this);
}

void FakeMediator::stop() {
  BOOST_ASSERT(_is_running);
  _is_running = false;
  _socket.close();
  _thread.join();
}

void FakeMediator::quit_after(message::MessageType message_type) {
  _quit_after = message_type;
}

void FakeMediator::_run() {
  while (_is_running) {
    auto socket = _socket.accept();
    if (!socket) {
      continue;
    }
    _handle_connection(socket);
  }
}

void FakeMediator::_handle_connection(std::shared_ptr<Socket> session_socket) {
  const auto raw_message = session_socket->receive();
  const auto message_type = message::decode_message_type(raw_message);
  _received_messages.push_back(raw_message);
  LOG(level::Debug) << "FakeMediator received "
                    << message::message_type_string(message_type)
                    << " message: " << raw_message << std::endl;
  if (message_type == _quit_after) {
    LOG(level::Debug) << "Finishing FakeMediator connection handling (after "
                      << message::message_type_string(_quit_after) << ")";
    return;
  }
  // TODO: continue connection handling
}

p2psc::Mediator FakeMediator::get_mediator_description() const {
  return _mediator;
}

std::vector<std::string> FakeMediator::get_received_messages() const {
  return _received_messages;
}
}
}
}