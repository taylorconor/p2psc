#include <boost/test/unit_test.hpp>
#include <crypto/rsa.h>
#include <iostream>
#include <p2psc/crypto/crypto_exception.h>

namespace p2psc {
namespace test {
namespace {
const auto message = "bananas";
}

BOOST_AUTO_TEST_SUITE(rsa_test)

BOOST_AUTO_TEST_CASE(ShouldEncryptPublicAndDecryptPrivate) {
  const auto key = crypto::RSA::generate();
  const auto encrypted_message = key->public_encrypt(message);
  BOOST_ASSERT(message != encrypted_message);
  const auto decrypted_message = key->private_decrypt(encrypted_message);
  BOOST_ASSERT(decrypted_message == message);
}

BOOST_AUTO_TEST_CASE(ShouldEncryptPrivateAndDecryptPublic) {
  const auto key = crypto::RSA::generate();
  const auto encrypted_message = key->private_encrypt(message);
  BOOST_ASSERT(message != encrypted_message);
  const auto decrypted_message = key->public_decrypt(encrypted_message);
  BOOST_ASSERT(decrypted_message == message);
}

BOOST_AUTO_TEST_CASE(ShouldNotEncryptPublicAndDecryptPublic) {
  const auto key = crypto::RSA::generate();
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

BOOST_AUTO_TEST_CASE(ShouldSerialiseAndDeserialiseKey) {
  const auto key = crypto::RSA::generate();
  const auto key_str = key->get_public_key_string();
  const auto restored_key = crypto::RSA::from_public_key(key_str);
  const auto message_encrypted = key->public_encrypt(message);
  const auto message_encrypted_restored = restored_key->public_encrypt(message);
  const auto message_decrypted = key->private_decrypt(message_encrypted);
  const auto message_decrypted_restored =
      key->private_decrypt(message_encrypted_restored);
  // PKCS#1 v1.5 adds 8 bits of random data. it is therefore unlikely (but
  // possible) that two identical messages will be encrypted to the same
  // value. we don't assert `message_encrypted != message_encrypted_restored`
  // because it is possible.
  BOOST_ASSERT(message == message_decrypted);
  BOOST_ASSERT(message_decrypted == message_decrypted_restored);
}

BOOST_AUTO_TEST_CASE(ShouldNotAttemptPrivateKeyActionWithNoPrivateKey) {
  const auto key = crypto::RSA::generate();
  const auto public_key_str = key->get_public_key_string();
  const auto public_key = crypto::RSA::from_public_key(public_key_str);
  // this should work because a public key is present
  const auto message_encrypted = public_key->public_encrypt(message);
  // this should not work because we don't have a private key
  try {
    const auto message_encrypted_private = public_key->private_encrypt(message);
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
}

BOOST_AUTO_TEST_CASE(ShouldRestoreKeyFromFile) {
  const auto generated_key = crypto::RSA::generate();
  const auto filename = "/tmp/p2psc_testfile";
  generated_key->write_to_file(filename);
  const auto key = crypto::RSA::from_pem(filename);
  const auto encrypted = key->public_encrypt(message);
  const auto decrypted = key->private_decrypt(encrypted);
  BOOST_ASSERT(decrypted == message);
  remove(filename);
}

BOOST_AUTO_TEST_CASE(ShouldNotRestoreKeyFromNonExistantFile) {
  try {
    crypto::RSA::from_pem("file_does_not_exist.pem");
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
}

BOOST_AUTO_TEST_CASE(ShouldRestoreKeyFromFileWithPassword) {
  const auto generated_key = crypto::RSA::generate();
  const auto filename = "/tmp/p2psc_testfile";
  const auto min_password = std::string(crypto::RSA::min_password_length, 'z');
  generated_key->write_to_file(filename, min_password, "aes-256-cbc");
  const auto key = crypto::RSA::from_pem(filename, min_password);
  const auto encrypted = key->public_encrypt(message);
  const auto decrypted = key->private_decrypt(encrypted);
  BOOST_ASSERT(decrypted == message);
  remove(filename);
}

BOOST_AUTO_TEST_CASE(ShouldNotRestoreKeyFromFileWithIncorrectPassword) {
  const auto generated_key = crypto::RSA::generate();
  const auto filename = "/tmp/p2psc_testfile";
  generated_key->write_to_file(filename, "my_password", "aes-256-cbc");
  try {
    const auto key = crypto::RSA::from_pem(filename, "wrong_password");
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
  remove(filename);
}

BOOST_AUTO_TEST_CASE(ShouldNotWriteKeyToFileWithInvalidPassword) {
  const auto generated_key = crypto::RSA::generate();
  const auto filename = "/tmp/p2psc_testfile";
  try {
    generated_key->write_to_file(
        filename, std::string(crypto::RSA::min_password_length - 1, 'z'),
        "aes-256-cbc");
    remove(filename);
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
}

BOOST_AUTO_TEST_CASE(ShouldNotWriteKeyToFileWithInvalidCipher) {
  const auto generated_key = crypto::RSA::generate();
  const auto filename = "/tmp/p2psc_testfile";
  try {
    generated_key->write_to_file(filename, "my_password", "invalid_cipher");
    remove(filename);
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
}

BOOST_AUTO_TEST_CASE(ShouldNotDecryptWithoutPassword) {
  const auto generated_key = crypto::RSA::generate();
  const auto filename = "/tmp/p2psc_testfile";
  generated_key->write_to_file(filename, "password", "aes-256-cbc");
  try {
    const auto key = crypto::RSA::from_pem(filename);
    remove(filename);
    BOOST_FAIL("Should have thrown CryptoException");
  } catch (const crypto::CryptoException &e) {
  }
}

BOOST_AUTO_TEST_SUITE_END()
}
}