#ifndef DISCRETEGAUSSIANSAMPLER_H
#define DISCRETEGAUSSIANSAMPLER_H

#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "DataType.hpp"


class DiscreteGaussianSampler {
 private:
  double m_std;  // Standard deviation
  double m_a;
  vector<double> m_vals;
  BigInt modulus;           // Modulus
  static mt19937 gen;  // Random number generator

  size_t FindInVector(const vector<double>& S, double search) const;

 public:
  DiscreteGaussianSampler(double stddev = 1, BigInt modulus = 0);
  BigInt GenerateInteger();
};

#endif  // DISCRETEGAUSSIANSAMPLER_H
