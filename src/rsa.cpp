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

::RSA *string_to_key(const std::string &key_str) {
  const auto bio = BIO_new_mem_buf(key_str.c_str(), key_str.length());
  return PEM_read_bio_RSA_PUBKEY(bio, 0, 0, 0);
}

inline void check_error(const std::string &method, int size) {
  if (size == -1) {
    char errbuf[1024];
    ERR_error_string_n(ERR_get_error(), errbuf, 1024);
    throw CryptoException(method + " failed: " + errbuf);
  }
}

std::string bio_to_string(BIO *bio) {
  char *bptr;
  const auto length = BIO_get_mem_data(bio, &bptr);
  return std::string(bptr, bptr + length);
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
  return std::shared_ptr<RSA>(new RSA(string_to_key(public_key)));
}

std::shared_ptr<RSA> RSA::generate() {
  return std::shared_ptr<RSA>(new RSA(generate_new_key()));
}

RSA::RSA(::RSA *key) : _key(key) {}

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
  const auto decoded = base64_decode(encrypted);
  unsigned char buf[RSA_size(_key)];
  int size = RSA_private_decrypt(decoded.length(),
                                 (const unsigned char *)decoded.c_str(), buf,
                                 _key, RSA_PKCS1_PADDING);
  check_error("RSA_private_decrypt", size);
  return std::string(buf, buf + size);
}

std::string RSA::private_encrypt(const std::string &key_str) const {
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
}
}