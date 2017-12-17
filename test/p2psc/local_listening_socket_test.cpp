#include <boost/test/unit_test.hpp>
#include <p2psc/log.h>
#include <socket/local_listening_socket.h>

namespace p2psc {
namespace test {
namespace {
const auto socket_creator =
    [](const SocketAddressOrFileDescriptor &address_or_file_descriptor) {
      if (address_or_file_descriptor.has_socket_address()) {
        return std::make_shared<Socket>(
            address_or_file_descriptor.socket_address());
      } else {
        return std::make_shared<Socket>(address_or_file_descriptor.sock_fd());
      }
    };
}

BOOST_AUTO_TEST_SUITE(local_listening_socket_test);

/*
 * Some of the tests in this suite are duplicated in the socket_test suite.
 */

BOOST_AUTO_TEST_CASE(ShouldRepeatedlyBindToFreePort) {
  for (int i = 0; i < 1000; i++) {
    const auto socket =
        std::make_unique<socket::LocalListeningSocket>(socket_creator);
    BOOST_ASSERT(socket->get_socket_address().port() > 1024);
  }
}

BOOST_AUTO_TEST_CASE(ShouldBindToSpecifiedPort) {
  /*
   * NOTE: this test is potentially flaky depending on the environment in
   * which it is run.
   */
  const auto socket =
      std::make_unique<socket::LocalListeningSocket>(socket_creator, 1337);
  BOOST_ASSERT(socket->get_socket_address().port() == 1337);
}

BOOST_AUTO_TEST_CASE(ShouldAcceptAndCreateSocket) {
  std::mutex mutex;
  std::condition_variable cv;
  uint16_t port;
  bool has_received_message = false;
  std::thread thread([&]() mutable {
    const auto listener =
        std::make_unique<socket::LocalListeningSocket>(socket_creator);
    port = listener->get_socket_address().port();
    cv.notify_one();
    const auto socket = listener->accept();
    BOOST_ASSERT(socket != nullptr);
    const auto message = socket->receive();
    BOOST_ASSERT(message == "bananas");
    has_received_message = true;
  });

  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock);
  const auto address = socket::SocketAddress("127.0.0.1", port);
  try {
    const auto socket = std::make_shared<Socket>(address);
    socket->send("bananas");
  } catch (const socket::SocketException &e) {
    BOOST_FAIL("Socket connect failed! Reason: " + std::string(e.what()));
  }
  thread.join();
  BOOST_ASSERT(has_received_message);
}

BOOST_AUTO_TEST_SUITE_END();
}
}