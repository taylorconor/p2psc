#include <boost/test/unit_test.hpp>
#include <p2psc/log.h>
#include <p2psc/socket.h>
#include <p2psc/socket/local_listening_socket.h>

namespace p2psc {
namespace test {
namespace {

void verifySendAndReceive(const std::string &message_1,
                          const std::string &message_2) {
  std::mutex mutex;
  std::condition_variable cv;
  uint16_t port;
  bool has_completed = false;
  std::thread thread([&]() mutable {
    const auto listener = socket::LocalListeningSocket();
    port = listener.get_socket_address().port();
    cv.notify_one();
    const auto socket = listener.accept();
    BOOST_ASSERT(socket != nullptr);
    const auto message = socket->receive();
    BOOST_ASSERT(message == message_1);
    socket->send(message_2);
    has_completed = true;
  });

  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock);
  const auto address = socket::SocketAddress("127.0.0.1", port);
  try {
    const auto socket = std::make_shared<Socket>(address);
    socket->send(message_1);
    const auto message = socket->receive();
    BOOST_ASSERT(message == message_2);
  } catch (const socket::SocketException &e) {
    BOOST_FAIL("Socket connect failed! Reason: " + std::string(e.what()));
  }
  thread.join();
  // this is just a sanity check to prove that this test didn't just pass
  // because of some race condition
  BOOST_ASSERT(has_completed);
}
}

BOOST_AUTO_TEST_SUITE(socket_test);

BOOST_AUTO_TEST_CASE(ShouldReturnCorrectSocketAddress) {
  std::mutex mutex;
  std::condition_variable cv;
  uint16_t port;
  std::thread thread([&cv, &port]() mutable {
    const auto listener = socket::LocalListeningSocket();
    port = listener.get_socket_address().port();
    cv.notify_one();
    const auto socket = listener.accept();
  });

  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock);
  const auto address = socket::SocketAddress("127.0.0.1", port);
  try {
    const auto socket = std::make_shared<Socket>(address);
    BOOST_ASSERT(socket->get_socket_address() == address);
  } catch (const socket::SocketException &e) {
    BOOST_FAIL("Socket connect failed! Reason: " + std::string(e.what()));
  }
  thread.join();
}

BOOST_AUTO_TEST_CASE(ShouldNotConnectToNonListeningPort) {
  const auto address = socket::SocketAddress("127.0.0.1", 1337);
  try {
    const auto socket = std::make_shared<Socket>(address);
    BOOST_FAIL("Should have thrown SocketException");
  } catch (const socket::SocketException &e) {
  }
}

BOOST_AUTO_TEST_CASE(ShouldConnectToListeningPort) {
  std::mutex mutex;
  std::condition_variable cv;
  uint16_t port;
  std::thread thread([&cv, &port]() mutable {
    const auto listener = socket::LocalListeningSocket();
    port = listener.get_socket_address().port();
    cv.notify_one();
    const auto socket = listener.accept();
    BOOST_ASSERT(socket != nullptr);
  });

  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock);
  const auto address = socket::SocketAddress("127.0.0.1", port);
  try {
    const auto socket = std::make_shared<Socket>(address);
  } catch (const socket::SocketException &e) {
    BOOST_FAIL("Socket connect failed! Reason: " + std::string(e.what()));
  }
  thread.join();
}

BOOST_AUTO_TEST_CASE(ShouldSendAndReceive) {
  verifySendAndReceive("banana", "potato");
}

BOOST_AUTO_TEST_CASE(ShouldSendAndReceiveExactlyRecvBufferSize) {
  verifySendAndReceive(std::string(socket::RECV_BUF_SIZE, 'a'),
                       std::string(socket::RECV_BUF_SIZE, 'b'));
}

BOOST_AUTO_TEST_CASE(ShouldSendAndReceiveMoreThanRecvBufferSize) {
  verifySendAndReceive(std::string(socket::RECV_BUF_SIZE + 1, 'a'),
                       std::string(socket::RECV_BUF_SIZE + 1, 'b'));
}

BOOST_AUTO_TEST_SUITE_END()
}
}