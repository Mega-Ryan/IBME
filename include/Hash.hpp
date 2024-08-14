#ifndef HASH_HPP
#define HASH_HPP

#include <array>
#include <cstring>

#include "Matrix.hpp"

class SHA256 {
 public:
  SHA256();
  void update(const uint8_t* data, size_t length);
  void finalize();
  array<uint8_t, 32> digest() const;

  static const size_t BlockSize = 64;
  static const size_t HashSize = 32;

  static array<uint8_t, 32> hash(const string& input);

 private:
  static const size_t InitialValues[8];
  static const uint32_t K[64];

  void transform(const uint8_t* chunk);
  void pad();
  void revert(uint8_t* hash);

  uint8_t data[BlockSize];
  uint32_t datalen;
  uint64_t bitlen;
  uint32_t state[8];
};

class Hash {
 public:
  Hash(int r, int c);
  Matrix hash(const string& input);

 private:
  int rows;
  int cols;
};

#endif  // HASH_HPP