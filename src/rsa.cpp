#include <base64/base64.h>
#include <openssl/err.h>
#include <p2psc/crypto/crypto_exception.h>
#include <p2psc/crypto/rsa.h>

namespace p2psc {
namespace crypto {

namespace {

using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
using RSA_ptr = std::unique_ptr<::RSA, decltype(&::RSA_free)>;

::RSA *generate_new_key() {
  ::RSA *rsa(RSA_new());
  BN_ptr bn(BN_new(), ::BN_free);
  BN_set_word(bn.get(), RSA_F4);

  RSA_generate_key_ex(rsa, kDefaultKeySize, bn.get(), NULL);
  return rsa;
}

std::string bio_to_string(BIO *bio) {
  char *bptr;
  const auto length = BIO_get_mem_data(bio, &bptr);
  return std::string(bptr, bptr + length);
}

::RSA *string_to_key(const std::string &key_str) {
  // TODO: does this leak?
  BIO *bio = BIO_new_mem_buf(key_str.c_str(), key_str.length());
  ::RSA *key = PEM_read_bio_RSAPublicKey(bio, 0, 0, 0);
  std::string str = bio_to_string(bio);
  if (!key) {
    throw CryptoException("Could not load RSA public key from string");
  }
  return key;
}

::RSA *file_to_key(const std::string &path) {
  BIO *bio = BIO_new(BIO_s_file());
  BIO_read_filename(bio, path.c_str());
  // the private key will also contain the public key
  ::RSA *key = PEM_read_bio_RSAPrivateKey(bio, 0, 0, 0);
  if (!key) {
    char errbuf[1024];
    ERR_error_string_n(ERR_get_error(), errbuf, 1024);
    throw CryptoException("Could not restore key from file: " + path + ". " +
                          errbuf);
  }
  return key;
}

inline void check_error(const std::string &method, int size) {
  if (size == -1) {
    char errbuf[1024];
    ERR_error_string_n(ERR_get_error(), errbuf, 1024);
    throw CryptoException(method + " failed: " + errbuf);
  }
}

std::string key_to_string_public(::RSA *key) {
  BIO *mem = BIO_new(BIO_s_mem());
  PEM_write_bio_RSAPublicKey(mem, key);
  const auto key_string = bio_to_string(mem);
  BIO_free(mem);
  return key_string;
}
}

std::shared_ptr<RSA> RSA::from_public_key(const std::string &public_key) {
  return std::shared_ptr<RSA>(new RSA(string_to_key(public_key), false));
}

std::shared_ptr<RSA> RSA::from_pem(const std::string &path) {
  return std::shared_ptr<RSA>(new RSA(file_to_key(path), true));
}

std::shared_ptr<RSA> RSA::generate() {
  return std::shared_ptr<RSA>(new RSA(generate_new_key(), true));
}

RSA::RSA(::RSA *key, bool has_private_key)
    : _key(key), _has_private_key(has_private_key) {}

RSA::~RSA() { RSA_free(_key); }

std::string RSA::public_encrypt(const std::string &key_str) const {
  unsigned char buf[RSA_size(_key)];
  int size = RSA_public_encrypt(key_str.length(),
                                (const unsigned char *)key_str.c_str(), buf,
                                _key, RSA_PKCS1_PADDING);
  check_error("RSA_public_encrypt", size);
  return base64_encode(buf, size);
}

std::string RSA::public_decrypt(const std::string &encrypted) const {
  const auto decoded = base64_decode(encrypted);
  unsigned char buf[RSA_size(_key)];
  int size = RSA_public_decrypt(decoded.length(),
                                (const unsigned char *)decoded.c_str(), buf,
                                _key, RSA_PKCS1_PADDING);
  check_error("RSA_public_decrypt", size);
  return std::string(buf, buf + size);
}

std::string RSA::private_decrypt(const std::string &encrypted) const {
  if (!_has_private_key) {
    throw CryptoException("This RSA structure has no private key");
  }
  const auto decoded = base64_decode(encrypted);
  unsigned char buf[RSA_size(_key)];
  int size = RSA_private_decrypt(decoded.length(),
                                 (const unsigned char *)decoded.c_str(), buf,
                                 _key, RSA_PKCS1_PADDING);
  check_error("RSA_private_decrypt", size);
  return std::string(buf, buf + size);
}

std::string RSA::private_encrypt(const std::string &key_str) const {
  if (!_has_private_key) {
    throw CryptoException("This RSA structure has no private key");
  }
  unsigned char buf[RSA_size(_key)];
  int size = RSA_private_encrypt(key_str.length(),
                                 (const unsigned char *)key_str.c_str(), buf,
                                 _key, RSA_PKCS1_PADDING);
  check_error("RSA_private_encrypt", size);
  return base64_encode(buf, size);
}

std::string RSA::get_public_key_string() const {
  return key_to_string_public(_key);
}

void RSA::write_to_file(const std::string &path) const {
  BIO *bio = BIO_new_file(path.c_str(), "w");
  PEM_write_bio_RSAPrivateKey(bio, _key, 0, 0, 0, 0, 0);
  BIO_free_all(bio);
}
}
}