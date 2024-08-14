#include <Hash.hpp>
#include <IB-ME.hpp>
#include <MP12.hpp>
#include <Tree.hpp>
#include <chrono>
#include <iostream>

void benchmarkOp() {
  cout << "Parameters:" << endl;
  cout << "N:" << N << endl;
  cout << "n:" << ROWS << endl;
  cout << "m:" << 2 * COLS << endl;
  cout << "G:" << 64 << endl;
  cout << "q:" << K << endl;
  cout << "--------------------------------------------------------------------"
       << endl;
  cout << "Operation:" << endl;

  cout << "test hash" << endl;
  Matrix::setModulus(MODULUS);
  Hash H1(ROWS, COLS);
  Matrix hashresult;
  auto hstart = std::chrono::high_resolution_clock::now();
  cout << "hstart" << endl;
  for (int i = 0; i < 10; ++i) {
    hashresult = H1.hash("hello");
  }
  auto check = H1.hash("hello");
  if (hashresult != check) {
    throw runtime_error("Hash function is not deterministic");
  }
  auto hend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, milli> hduration = hend - hstart;
  std::cout << "Hash time: " << hduration.count() / 10 << " ms" << std::endl;

  cout << "test Gaussian" << endl;
  DiscreteGaussianSampler dgs = DiscreteGaussianSampler(SIGMA);
  BigInt sample;
  auto gstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    sample = dgs.GenerateInteger();
  }
  auto gend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> gduration = gend - gstart;
  std::cout << "Gaussian sampling time: " << gduration.count() / 10 << " ms"
            << std::endl;

  cout << "test Zq multiply" << endl;
  BigInt product;
  auto zstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    product = (MODULUS - 1) * (MODULUS - 1) % MODULUS;
  }
  auto zend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> zduration = zend - zstart;
  std::cout << "Zq multiply time: " << zduration.count() / 10 << " ms"
            << std::endl;

  cout << "---------------------------------------------------------------"
       << endl;
}

int main() {
  // testOp();
  benchmarkOp();

  return 0;
}