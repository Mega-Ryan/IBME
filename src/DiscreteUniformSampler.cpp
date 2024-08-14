#include "DiscreteUniformSampler.hpp"

// Initialize the static random number generator
mt19937 DiscreteUniformSampler::gen(random_device{}());

DiscreteUniformSampler::DiscreteUniformSampler(BigInt modulus)
    : modulus(modulus) {
  if (modulus == 0) {
    this->modulus = numeric_limits<BigInt>::max();
  }
}

BigInt DiscreteUniformSampler::GenerateInteger() {
  uniform_int_distribution<BigInt> dist(0, modulus - 1);

  // Generate a random BigInt
  BigInt result = dist(gen);

  return result;
}
