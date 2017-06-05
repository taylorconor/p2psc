#include <boost/test/unit_test.hpp>
#include <iostream>
#include <p2psc/crypto/crypto_exception.h>
#include <p2psc/crypto/rsa.h>

namespace p2psc {
namespace test {

BOOST_AUTO_TEST_SUITE(rsa_test)

BOOST_AUTO_TEST_CASE(ShouldEncryptPublicAndDecryptPrivate) {
  const auto key = crypto::RSA::generate();
  const auto message = "bananas";
  const auto encrypted_message = key->public_encrypt(message);
  BOOST_ASSERT(message != encrypted_message);
  const auto decrypted_message = key->private_decrypt(encrypted_message);
  BOOST_ASSERT(decrypted_message == message);
}

BOOST_AUTO_TEST_CASE(ShouldEncryptPrivateAndDecryptPublic) {
  const auto key = crypto::RSA::generate();
  const auto message = "bananas";
  const auto encrypted_message = key->private_encrypt(message);
  BOOST_ASSERT(message != encrypted_message);
  const auto decrypted_message = key->public_decrypt(encrypted_message);
  BOOST_ASSERT(decrypted_message == message);
}

BOOST_AUTO_TEST_CASE(ShouldNotEncryptPublicAndDecryptPublic) {
  const auto key = crypto::RSA::generate();
  const auto message = "bananas";
  const auto encrypted_message = key->public_encrypt(message);
  BOOST_ASSERT(message != encrypted_message);
  try {
    const auto decrypted_message = key->public_decrypt(encrypted_message);
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
}

BOOST_AUTO_TEST_CASE(ShouldNotEncryptPrivateAndDecryptPrivate) {
  const auto key = crypto::RSA::generate();
  const auto message = "bananas";
  const auto encrypted_message = key->private_encrypt(message);
  BOOST_ASSERT(message != encrypted_message);
  try {
    const auto decrypted_message = key->private_decrypt(encrypted_message);
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
}

BOOST_AUTO_TEST_CASE(ShouldEncryptMaxBytes) {
  const auto key = crypto::RSA::generate();
  const auto message =
      std::string(crypto::RSA::max_data_size(kDefaultKeySize), 'z');
  const auto encrypted_message = key->public_encrypt(message);
  BOOST_ASSERT(message != encrypted_message);
  const auto decrypted_message = key->private_decrypt(encrypted_message);
  BOOST_ASSERT(decrypted_message == message);
}

BOOST_AUTO_TEST_CASE(ShouldNotEncryptMoreThanMaxBytes) {
  const auto key = crypto::RSA::generate();
  const auto message =
      std::string(crypto::RSA::max_data_size(kDefaultKeySize) + 1, 'z');
  try {
    const auto encrypted_message = key->public_encrypt(message);
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
}

BOOST_AUTO_TEST_SUITE_END()
}
}