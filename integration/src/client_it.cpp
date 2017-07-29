#include <boost/test/unit_test.hpp>
#include <iostream>

namespace p2psc {
namespace integration {

BOOST_AUTO_TEST_SUITE(client_integration_test)

BOOST_AUTO_TEST_CASE(ShouldDoAThing) {
  std::cout << "Running integration test suite" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
}
}