#ifndef MP12_HPP
#define MP12_HPP

#include <math.h>

#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Matrix.hpp"

class MP12 {
 private:
  static Matrix* List;
  // static unsigned int mean;
  static double stddev;
  static mt19937 rng;

  int partition(int low, int high);
  void quickSort(int low, int high);
  unsigned int binarySearch(BigInt target);
  Matrix O(BigInt u);
  Matrix fGInverse(Matrix u);
  void generateList(unsigned int q);

 public:
  static void testmp();
  static void testfA();
  static void testfAwithoutV();
  static void testDelTrap();

  MP12();
  MP12(unsigned int q, double stddev);

  double getStddev();

  // Function to generate a trapdoor, A size n x 2nk
  static pair<Matrix, Matrix> trapGen(unsigned int n);
  static Matrix fAInverse(Matrix A, Matrix T_A, Matrix u);
  static Matrix fAInversewithoutVariance(Matrix A, Matrix T_A, Matrix u);

  // Function to delegate a trapdoor, A size n x m, A1 size n x nk
  static pair<Matrix, Matrix> delTrap(const Matrix& A, const Matrix& T_A,
                                      const Matrix& A1, double stddev);

  // SampleLeft
  static Matrix SampleLeft(const Matrix& A, const Matrix& M1,
                           const Matrix& trapdoorA, const Matrix& u);
};

#endif  // MP12_HPP
