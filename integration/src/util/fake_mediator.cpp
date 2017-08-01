#include <boost/assert.hpp>
#include <include/util/fake_mediator.h>

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
  const auto message = session_socket->receive();
  std::cout << "FakeMediator received message: " << message << std::endl;
}

p2psc::Mediator FakeMediator::get_mediator_description() const {
  return _mediator;
}
}
}
}