#ifndef DISCRETE_UNIFORM_SAMPLER_HPP
#define DISCRETE_UNIFORM_SAMPLER_HPP

#include <iostream>
#include <random>
#include <stdexcept>

#include "DataType.hpp"

class DiscreteUniformSampler {
 public:
  DiscreteUniformSampler(BigInt modulus);
  BigInt GenerateInteger();

 private:
  BigInt modulus;           // Modulus
  static mt19937 gen;  // Random number generator
};

#endif  // DISCRETE_UNIFORM_SAMPLER_HPP