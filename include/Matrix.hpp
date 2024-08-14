#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <ctime>
#include <random>
#include <vector>

#include "DiscreteGaussianSampler.hpp"
#include "DiscreteUniformSampler.hpp"

class Matrix {
 private:
  vector<vector<BigInt>> data;
  unsigned int rows, cols;
  static unsigned int k;
  static BigInt modulus;
  static mt19937 rand_generator;

 public:
  // Default Constructor
  Matrix();

  // Constructor
  Matrix(unsigned int r, unsigned int c);

  // get size information
  unsigned int getRows() const;
  unsigned int getCols() const;
  static unsigned int getK();

  // Function to set the global modulus
  static void setModulus(const BigInt& mod);

  // Function to get the global modulus
  static BigInt getModulus();

  // Method to swap two columns
  void swapColumns(unsigned int col1, unsigned int col2);

  // get a column from Matrix
  Matrix getColVector(unsigned int col) const;

  // Function to set a value in the matrix
  void set(unsigned int r, unsigned int c, const BigInt& value);

  // Function to get a value from the matrix
  BigInt get(unsigned int r, unsigned int c) const;

  // Function to add two matrices
  Matrix operator+(const Matrix& other) const;

  // Function to subtract two matrices
  Matrix operator-(const Matrix& other) const;

  // Function to multiply two matrices
  Matrix operator*(const Matrix& other) const;

  // Function to check if two matrices are equal
  bool operator==(const Matrix& other) const;

  // Function to check if two matrices are not equal
  bool operator!=(const Matrix& other) const;

  // Function to multiply matrix by an integer
  static Matrix multiplyByInteger(const Matrix& in, const BigInt& num);

  // Function to print the matrix
  void print() const;

  // Function to generate a discrete Gaussian matrix
  static Matrix generateDiscreteGaussianMatrix(unsigned int r, unsigned int c,
                                               double stddev);

  // Function to generate a random matrix
  static Matrix generateUniformRandomMatrix(unsigned int r, unsigned int c);

  // Function to generate a matrix of 1 and -1
  static Matrix generateSignMatrix(unsigned int r, unsigned int c);

  // Function to transpose the matrix
  Matrix transpose() const;

  // Function to get rank of the matrix
  unsigned int rank() const;

  // Function to generate a gadget matrix, the size is n x nk
  static Matrix generateGadgetMatrix(unsigned int n);

  // Function to generate an identity matrix
  static Matrix generateIdentityMatrix(unsigned int n);

  // Function to vertically concatenate two matrices
  static Matrix verticalConcat(const Matrix& A, const Matrix& B);

  // Function to horizontally concatenate two matrices
  static Matrix horizontalConcat(const Matrix& A, const Matrix& B);

  // Function to check if the matrix is positive definite
  bool isPositiveDefinite() const;

  // Function to perform Cholesky Decomposition
  static Matrix CholeskyDecomposition(const Matrix& in);

  // Function to generate Gaussian with L
  static Matrix generateGaussianwithL(Matrix L, int stddev = 1);

  // Function to convert matrix to a string
  string toString() const;
};

#endif  // MATRIX_HPP
