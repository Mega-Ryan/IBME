#include "Hash.hpp"

const size_t SHA256::InitialValues[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372,
                                         0xa54ff53a, 0x510e527f, 0x9b05688c,
                                         0x1f83d9ab, 0x5be0cd19};

const uint32_t SHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

static inline uint32_t rotr(uint32_t x, uint32_t n) {
  return (x >> n) | (x << (32 - n));
}

static inline uint32_t choose(uint32_t e, uint32_t f, uint32_t g) {
  return (e & f) ^ (~e & g);
}

static inline uint32_t majority(uint32_t a, uint32_t b, uint32_t c) {
  return (a & b) ^ (a & c) ^ (b & c);
}

static inline uint32_t sig0(uint32_t x) {
  return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static inline uint32_t sig1(uint32_t x) {
  return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

static inline uint32_t theta0(uint32_t x) {
  return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

static inline uint32_t theta1(uint32_t x) {
  return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

SHA256::SHA256() {
  datalen = 0;
  bitlen = 0;
  for (size_t i = 0; i < 8; ++i) {
    state[i] = InitialValues[i];
  }
}

void SHA256::update(const uint8_t* data, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    this->data[datalen] = data[i];
    datalen++;
    if (datalen == BlockSize) {
      transform(this->data);
      bitlen += BlockSize * 8;
      datalen = 0;
    }
  }
}

void SHA256::finalize() {
  bitlen += datalen * 8;

  size_t i = datalen;

  // Pad whatever data is left in the buffer.
  if (datalen < 56) {
    data[i++] = 0x80;
    while (i < 56) {
      data[i++] = 0x00;
    }
  } else {
    data[i++] = 0x80;
    while (i < 64) {
      data[i++] = 0x00;
    }
    transform(data);
    memset(data, 0, 56);
  }

  // Append the total message's length in bits and transform.
  for (int j = 7; j >= 0; --j) {
    data[63 - j] = bitlen >> (j * 8);
  }
  transform(data);
}

array<uint8_t, 32> SHA256::digest() const {
  array<uint8_t, 32> hash;
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 8; ++j) {
      hash[i + (j * 4)] = (state[j] >> ((3 - i) * 8)) & 0xff;
    }
  }
  return hash;
}

void SHA256::transform(const uint8_t* chunk) {
  uint32_t w[64];
  uint32_t a, b, c, d, e, f, g, h;

  for (size_t i = 0; i < 16; ++i) {
    w[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) |
           (chunk[i * 4 + 2] << 8) | (chunk[i * 4 + 3]);
  }

  for (size_t i = 16; i < 64; ++i) {
    w[i] = theta1(w[i - 2]) + w[i - 7] + theta0(w[i - 15]) + w[i - 16];
  }

  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  f = state[5];
  g = state[6];
  h = state[7];

  for (size_t i = 0; i < 64; ++i) {
    uint32_t t1 = h + sig1(e) + choose(e, f, g) + K[i] + w[i];
    uint32_t t2 = sig0(a) + majority(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  state[5] += f;
  state[6] += g;
  state[7] += h;
}

void SHA256::revert(uint8_t* hash) {
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 8; ++j) {
      hash[i + (j * 4)] = (state[j] >> ((3 - i) * 8)) & 0xff;
    }
  }
}

array<uint8_t, 32> SHA256::hash(const string& input) {
  SHA256 sha256;
  sha256.update(reinterpret_cast<const uint8_t*>(input.data()), input.size());
  sha256.finalize();
  return sha256.digest();
}

Hash::Hash(int r, int c) : rows(r), cols(c) {}

Matrix Hash::hash(const string& input) {
  BigInt q = Matrix::getModulus();

  Matrix matrix(rows, cols);
  size_t bytes_needed = rows * cols * sizeof(BigInt);
  vector<uint8_t> random_bytes(bytes_needed);

  string hash_input = input;
  size_t hash_output_length = SHA256::HashSize;
  array<uint8_t, 32> hash_output;
  size_t byte_index = 0;

  while (byte_index < bytes_needed) {
    hash_output = SHA256::hash(hash_input);
    size_t copy_size = min(hash_output_length, bytes_needed - byte_index);
    memcpy(random_bytes.data() + byte_index, hash_output.data(),
                copy_size);
    byte_index += copy_size;
    hash_input = string(hash_output.begin(), hash_output.end());
  }

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      BigInt value = 0;
      memcpy(&value, random_bytes.data() + (i * cols + j) * sizeof(BigInt),
                  sizeof(BigInt));
      matrix.set(i, j, value % q);
    }
  }

  return matrix;
}