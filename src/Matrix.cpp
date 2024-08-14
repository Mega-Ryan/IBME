#include "Matrix.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

// Initialize static members
mt19937 Matrix::rand_generator(static_cast<unsigned>(time(nullptr)));
unsigned int Matrix::k = 0;
BigInt Matrix::modulus = 0;

// Default Constructor
Matrix::Matrix() {}

// Constructor
Matrix::Matrix(unsigned int r, unsigned int c)
    : rows(r), cols(c), data(r, vector<BigInt>(c)) {}

// Get size information
unsigned int Matrix::getRows() const { return rows; }

unsigned int Matrix::getCols() const { return cols; }

unsigned int Matrix::getK() { return k; }

// Function to set the global modulus
void Matrix::setModulus(const BigInt& mod) {
  modulus = mod;
  k = ceil(log2(modulus));
}

BigInt Matrix::getModulus() { return modulus; }

// Method to swap two columns
void Matrix::swapColumns(const unsigned int col1, const unsigned int col2) {
  if (col1 >= cols || col2 >= cols) {
    throw out_of_range("Column index out of range");
  }
  for (unsigned int i = 0; i < rows; ++i) {
    swap(data[i][col1], data[i][col2]);
  }
}

// Get a column from Matrix
Matrix Matrix::getColVector(const unsigned int col) const {
  Matrix res(rows, 1);
  for (unsigned int i = 0; i < rows; ++i) {
    res.set(i, 0, data[i][col]);
  }
  return res;
}

// Function to set a value in the matrix
void Matrix::set(const unsigned int r, const unsigned int c,
                 const BigInt& value) {
  if (r < rows && c < cols) {
    data[r][c] = (value % modulus + modulus) % modulus;
  } else {
    throw out_of_range("set Index out of range");
  }
}

// Function to get a value from the matrix
BigInt Matrix::get(unsigned int r, unsigned int c) const {
  if (r < rows && c < cols) {
    return data[r][c];
  } else {
    // cout<<"int put r,c="<<r<<" "<<c<<endl;
    throw out_of_range("get Index out of range");
  }
}

// Function to add two matrices
Matrix Matrix::operator+(const Matrix& other) const {
  if (rows != other.rows || cols != other.cols) {
    throw invalid_argument(
        "Matrices must have the same dimensions for addition");
  }
  Matrix result(rows, cols);
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      result.set(i, j, this->get(i, j) + other.get(i, j));
    }
  }
  return result;
}

// Function to subtract two matrices
Matrix Matrix::operator-(const Matrix& other) const {
  if (rows != other.rows || cols != other.cols) {
    throw invalid_argument(
        "Matrices must have the same dimensions for subtraction");
  }
  Matrix result(rows, cols);
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      result.set(i, j, this->get(i, j) - other.get(i, j));
    }
  }
  return result;
}

// Function to multiply two matrices
Matrix Matrix::operator*(const Matrix& other) const {
  if (cols != other.rows) {
    throw invalid_argument("Matrices cannot be multiplied");
  }
  Matrix result(rows, other.cols);
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < other.cols; ++j) {
      BigInt sum = 0;
      for (unsigned int k = 0; k < cols; ++k) {
        sum = sum + (this->get(i, k) * other.get(k, j));
      }
      result.set(i, j, sum);
    }
  }
  return result;
}

// Function to check if two matrices are equal
bool Matrix::operator==(const Matrix& other) const {
  if (rows != other.rows || cols != other.cols) {
    return false;
  }
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      if (data[i][j] != other.data[i][j]) {
        return false;
      }
    }
  }
  return true;
}

// Function to check if two matrices are not equal
bool Matrix::operator!=(const Matrix& other) const { return !(*this == other); }

// Function to multiply matrix by an integer
Matrix Matrix::multiplyByInteger(const Matrix& in, const BigInt& num) {
  unsigned int rows = in.getRows();
  unsigned int cols = in.getCols();
  Matrix result(rows, cols);
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      result.data[i][j] = in.get(i, j) * num;
    }
  }
  return result;
}

// Function to print the matrix
void Matrix::print() const {
  string line(this->cols * 11, '-');
  cout << line << endl;
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      cout << setw(10) << data[i][j] << " ";
    }
    cout << endl;
  }
  cout << line << endl;
}

// Function to generate a discrete Gaussian matrix
Matrix Matrix::generateDiscreteGaussianMatrix(unsigned int r, unsigned int c,
                                              double stddev) {
  DiscreteGaussianSampler sampler(stddev, modulus);
  Matrix result(r, c);
  for (unsigned int i = 0; i < r; ++i) {
    for (unsigned int j = 0; j < c; ++j) {
      BigInt value = sampler.GenerateInteger();
      result.set(i, j, value);
    }
  }
  return result;
}

// Function to generate a matrix of 1 and -1
Matrix Matrix::generateSignMatrix(unsigned int r, unsigned int c) {
  // Initialize the random number generator
  uniform_int_distribution<int> dist(0, 1);  // distribution to generate 0 or 1

  // Create the m x m matrix
  Matrix matrix(r, c);

  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      // Generate either -1 or 1
      matrix.set(i, j, dist(rand_generator) == 0 ? -1 : 1);
    }
  }

  return matrix;
}

// Function to generate a random matrix
Matrix Matrix::generateUniformRandomMatrix(unsigned int r, unsigned int c) {
  DiscreteUniformSampler sampler(modulus);
  Matrix result(r, c);
  for (unsigned int i = 0; i < r; ++i) {
    for (unsigned int j = 0; j < c; ++j) {
      BigInt value = sampler.GenerateInteger();
      result.set(i, j, value);
    }
  }
  return result;
}

// Function to transpose the matrix
Matrix Matrix::transpose() const {
  Matrix result(cols, rows);
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      result.set(j, i, data[i][j]);
    }
  }
  return result;
}

// Function to get rank of the matrix
unsigned int Matrix::rank() const {
  Matrix temp(*this);  // Make a copy of the matrix
  unsigned int rank = 0;
  for (unsigned int row = 0; row < rows; ++row) {
    if (temp.data[row][row] != 0) {
      for (unsigned int col = 0; col < cols; ++col) {
        if (col != row) {
          BigInt ratio = temp.data[col][row] / temp.data[row][row];
          for (unsigned int k = 0; k < cols; ++k) {
            temp.data[col][k] = temp.data[col][k] - ratio * temp.data[row][k];
          }
        }
      }
    } else {
      bool reduce = true;
      for (unsigned int i = row + 1; i < rows; ++i) {
        if (temp.data[i][row] != 0) {
          swap(temp.data[row], temp.data[i]);
          reduce = false;
          break;
        }
      }
      if (reduce) {
        --rank;
        for (unsigned int i = 0; i < rows; ++i) {
          temp.data[i][row] = temp.data[i][cols - 1];
        }
      }
    }
    ++rank;
  }
  return rank;
}

// Function to generate a gadget matrix
Matrix Matrix::generateGadgetMatrix(unsigned int n) {
  Matrix result(n, n * k);
  for (unsigned int i = 0; i < n; ++i) {
    for (unsigned int j = 0; j < n * k; j += k) {
      if (i == j / k) {
        for (unsigned int kk = 0; kk < k; ++kk) {
          result.set(i, j + kk, (pow(2., kk)));
        }
      } else {
        for (unsigned int kk = 0; kk < k; ++kk) {
          result.set(i, j + kk, 0);
        }
      }
    }
  }
  return result;
}

// Function to generate an identity matrix
Matrix Matrix::generateIdentityMatrix(unsigned int n) {
  Matrix result(n, n);
  for (unsigned int i = 0; i < n; ++i) {
    for (unsigned int j = 0; j < n; ++j) {
      if (i == j) {
        result.set(i, j, 1);
      } else {
        result.set(i, j, 0);
      }
    }
  }
  return result;
}

// Function to vertically concatenate two matrices
Matrix Matrix::verticalConcat(const Matrix& A, const Matrix& B) {
  if (A.cols != B.cols) {
    throw invalid_argument(
        "Matrix column counts do not match for vertical concatenation.");
  }
  Matrix result(A.rows + B.rows, A.cols);
  for (unsigned int i = 0; i < A.rows; ++i) {
    for (unsigned int j = 0; j < A.cols; ++j) {
      result.data[i][j] = A.data[i][j];
    }
  }
  for (unsigned int i = 0; i < B.rows; ++i) {
    for (unsigned int j = 0; j < B.cols; ++j) {
      result.data[i + A.rows][j] = B.data[i][j];
    }
  }
  return result;
}

// Function to horizontally concatenate two matrices
Matrix Matrix::horizontalConcat(const Matrix& A, const Matrix& B) {
  if (A.rows != B.rows) {
    throw invalid_argument(
        "Matrix row counts do not match for horizontal concatenation.");
  }
  Matrix result(A.rows, A.cols + B.cols);
  for (unsigned int i = 0; i < A.rows; ++i) {
    for (unsigned int j = 0; j < A.cols; ++j) {
      result.data[i][j] = A.data[i][j];
    }
  }
  for (unsigned int i = 0; i < B.rows; ++i) {
    for (unsigned int j = 0; j < B.cols; ++j) {
      result.data[i][j + A.cols] = B.data[i][j];
    }
  }
  return result;
}

// Function to check if the matrix is positive definite
bool Matrix::isPositiveDefinite() const {
  if (rows != cols) {
    throw invalid_argument(
        "Matrix must be square to check positive definiteness");
  }

  for (unsigned int i = 1; i <= rows; ++i) {
    if (data[i][i] == 0) {
      throw invalid_argument(
          "The principal diagonal elements of the matrix cannot be 0.");
    }
  }
  return true;
}

// Function to perform Cholesky Decomposition
Matrix Matrix::CholeskyDecomposition(const Matrix& in) {
  Matrix result(in.rows, in.cols);
  if (in.isPositiveDefinite()) {
    for (unsigned int i = 0; i < in.rows; ++i) {
      for (unsigned int j = 0; j <= i; ++j) {
        BigInt sum = 0;
        for (unsigned int k = 0; k < j; ++k) {
          sum = (sum + in.get(i, k) * in.get(j, k)) % modulus;
        }
        if (i == j) {
          result.set(i, j, sqrt(in.get(i, i) - sum));
        } else {
          result.set(i, j, (in.get(i, j) - sum) / in.get(j, j));
        }
      }
    }
  }
  return result;
}

// Function to generate Gaussian with L
Matrix Matrix::generateGaussianwithL(Matrix L, int stddev) {
  unsigned int n = L.getCols();
  Matrix variance = CholeskyDecomposition(L);
  Matrix Gauss = generateDiscreteGaussianMatrix(n, n, stddev);
  Matrix result = Gauss * variance;
  return result;
}

// Function to convert matrix to a string
string Matrix::toString() const {
  string result;
  for (unsigned int i = 0; i < rows; ++i) {
    for (unsigned int j = 0; j < cols; ++j) {
      result += to_string(data[i][j]) + " ";
    }
    result += "\n";
  }
  return result;
}
