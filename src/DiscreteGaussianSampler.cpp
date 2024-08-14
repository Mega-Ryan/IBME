#include "DiscreteGaussianSampler.hpp"

// Initialize the static random number generator
mt19937 DiscreteGaussianSampler::gen(random_device{}());

DiscreteGaussianSampler::DiscreteGaussianSampler(double stddev, BigInt modulus)
    : m_std(stddev), modulus(modulus) {
  if (modulus == 0) {
    this->modulus = numeric_limits<BigInt>::max();
  }

  m_vals.clear();

  double acc = 5e-32;
  double variance = m_std * m_std;

  BigInt fin = static_cast<BigInt>(ceil(m_std * sqrt(-2 * log(acc))));

  double cusum = 1.0;

  for (BigInt x = 1; x <= fin; x++) {
    cusum = cusum + 2 * exp(-x * x / (variance * 2));
  }

  m_a = 1 / cusum;

  double temp;

  for (BigInt i = 1; i <= fin; i++) {
    temp = m_a * exp(-(static_cast<double>(i * i) / (2 * variance)));
    m_vals.push_back(temp);
  }

  for (size_t i = 1; i < m_vals.size(); i++) {
    m_vals[i] += m_vals[i - 1];
  }
}

BigInt DiscreteGaussianSampler::GenerateInteger() {
  BigInt val = 0;
  double seed;
  BigInt ans;
  uniform_real_distribution<double> distribution(0.0, 1.0);

  seed = distribution(gen) - 0.5;

  if (abs(seed) <= m_a / 2) {
    val = 0;
  } else if (seed > 0) {
    val = FindInVector(m_vals, (abs(seed) - m_a / 2));
  } else {
    val = -static_cast<BigInt>(FindInVector(m_vals, (abs(seed) - m_a / 2)));
  }

  if (val < 0) {
    ans = modulus + val;
  } else {
    ans = val;
  }

  return ans;
}

size_t DiscreteGaussianSampler::FindInVector(const vector<double>& S,
                                             double search) const {
  auto lower = lower_bound(S.begin(), S.end(), search);
  if (lower != S.end()) {
    return lower - S.begin() + 1;
  }
  throw runtime_error("DGG Inversion Sampling. FindInVector value not found: " +
                      to_string(search));
}
