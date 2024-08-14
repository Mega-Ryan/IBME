#include "MP12.hpp"
#include "IB-ME.hpp"

mt19937 MP12::rng = mt19937(random_device()());
// unsigned int MP12::mean = 0;
double MP12::stddev = 0;
Matrix* MP12::List = nullptr;

MP12::MP12() {
  if (List == nullptr) {
    throw invalid_argument("Oracle is not initialized");
  }
}

MP12::MP12(unsigned int q, double stddev) {
  if (List != nullptr) {
    return;
  }
  Matrix::setModulus(q);
  // this->mean = mean;
  this->stddev = stddev;
  // cout << "start to generateList(q)" << endl;
  generateList(q);
}

double MP12::getStddev() { return stddev; }

int MP12::partition(int low, int high) {
  BigInt pivot = List->get(0, high);
  int i = low - 1;
  for (int j = low; j < high; ++j) {
    if (List->get(0, j) < pivot) {
      ++i;
      List->swapColumns(i, j);
    }
  }
  List->swapColumns(i + 1, high);
  return i + 1;
}

void MP12::quickSort(int low, int high) {
  if (low < high) {
    int pi = partition(low, high);
    quickSort(low, pi - 1);
    quickSort(pi + 1, high);
  }
}

void MP12::generateList(unsigned int q) {
  BigInt k = ceil(log2(q));
  BigInt m = k;
  Matrix* l = new Matrix(m + 1, q * k);
  List = l;
  // cout << "new l;" << endl;
  //(*List).print();

  Matrix X = Matrix::generateDiscreteGaussianMatrix(m, q * k, stddev);
  // cout << "print X" << endl;
  // X.print();
  Matrix g = Matrix(1, m);

  // cout << "g size is 1 x " << m << endl;
  for (int i = 0; i < m; i++) {
    g.set(0, i, pow(2, i));
  }
  // cout << "print g" << endl;
  // g.print();

  Matrix u = g * X;
  // cout << "print u" << endl;
  // u.print();

  (*l) = Matrix::verticalConcat(u, X);
  // cout << "print Oracle" << endl;
  // (*l).print();

  quickSort(0, (*l).getCols() - 1);

  // cout << "print sorted Oracle" << endl;
  // (*l).print();
}

pair<Matrix, Matrix> MP12::trapGen(unsigned int n) {
  BigInt q = Matrix::getModulus();
  BigInt k = Matrix::getK();
  BigInt m = n * k;
  Matrix B = Matrix::generateUniformRandomMatrix(n, m);

  // cout << "B:" << endl;
  // B.print();
  Matrix G = Matrix::generateGadgetMatrix(n);
  // cout << "G:" << endl;
  // G.print();
  Matrix R =
      Matrix::generateDiscreteGaussianMatrix(m, n * k, MP12().getStddev());
  // cout << "R:" << endl;
  // R.print();
  Matrix G_BR = G - B * R;
  // cout << "G_BR:" << endl;
  // G_BR.print();
  Matrix A = Matrix::horizontalConcat(B, G_BR);
  // cout << "A:" << endl;
  // A.print();

  pair<Matrix, Matrix> result = make_pair(A, R);

  return result;
}

unsigned int MP12::binarySearch(BigInt target) {
  int low = 0;
  int high = (*List).getCols() - 1;

  while (low <= high) {
    int mid = low + (high - low) / 2;

    if ((*List).get(0, mid) == target) {
      // cout << "target is " << target << endl;
      // cout << "it's index is " << mid << endl;
      return mid;
    }

    if ((*List).get(0, mid) < target) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  throw invalid_argument("No such u vectors");
}

Matrix MP12::O(BigInt u) {
  vector<Matrix> targV;
  unsigned int target = binarySearch(u);

  unsigned int i = target;
  // cout<<"target int i="<<i<<endl;
  unsigned int resRows = (*List).getRows();
  // cout << "O cout resRows:" << resRows << endl;
  Matrix res(resRows - 1, 1);

  while (i >= 0 && (*List).get(0, i) == u) {
    // cout<<"i-- i="<<i<<endl;
    targV.push_back((*List).getColVector(i));
    if (i == 0) break;
    --i;
  }
  i = target + 1;
  while (i < (*List).getCols() && (*List).get(0, i) == u) {
    // cout<<"i++ i="<<i<<endl;
    targV.push_back((*List).getColVector(i));
    ++i;
  }
  unsigned int numAns = targV.size();
  // cout<<"end find targV"<<endl;

  unsigned int random_number = 0;
  if (numAns != 1) {
    uniform_int_distribution<int> distribution(0, numAns - 1);
    random_number = distribution(rng);
  }

  for (int i = 0; i < resRows - 1; i++) {
    // cout<<"set "<<"i ="<<i<<"random number is "<<random_number<<endl;
    res.set(i, 0, targV[random_number].get(i + 1, 0));
  }
  // cout<<"O res is :"<<endl;
  // res.print();
  return res;
}

Matrix MP12::fGInverse(Matrix u) {
  unsigned int n = u.getRows();
  unsigned int k = Matrix::getK();

  // cout << "fGInverse n, k=" << n << "," << k << endl;
  Matrix x(n * k, 1);

  for (unsigned int i = 0; i < n; i++) {
    BigInt u_i = u.get(i, 0);
    Matrix x_i = O(u_i);
    // cout<<"u_i="<<u_i<<endl;
    // cout<<"x_i="<<endl;
    // x_i.print();
    for (unsigned int kk = 0; kk < k; kk++) {
      x.set(i * k + kk, 0, x_i.get(kk, 0));
    }
  }
  // cout<<"fGInverse print x"<<endl;
  // x.print();
  return x;
}

Matrix MP12::fAInversewithoutVariance(Matrix A, Matrix T_A, Matrix u) {
  unsigned int nk = T_A.getCols();
  Matrix z = MP12().fGInverse(u);
  Matrix I = Matrix::generateIdentityMatrix(nk);
  Matrix R_I = Matrix::verticalConcat(T_A, I);
  return R_I * z;
}

Matrix MP12::fAInverse(Matrix A, Matrix T_A, Matrix u) {
  throw invalid_argument("This function is to be completed in the future");
  unsigned int n = u.getRows();
  unsigned int k = Matrix::getK();
  Matrix R = T_A;
  Matrix z = MP12().fGInverse(u - A);
  Matrix I = Matrix::generateIdentityMatrix(n * k);
  Matrix RIc = Matrix::verticalConcat(R, I);
  Matrix x = RIc * z;
  BigInt s = stddev;

  Matrix Iss = Matrix::multiplyByInteger(I, s * s);
  cout << "fAInverse Iss:" << endl;
  Iss.print();

  Matrix RRt = R * R.transpose();
  cout << "fAInverse RRt:" << endl;
  RRt.print();

  Matrix v = Iss - RRt;

  cout << "fAInverse v:" << endl;
  v.print();
  Matrix p = Matrix::generateGaussianwithL(v);
  Matrix result = p + RIc * z;
  return result;
}

void MP12::testmp() {
  unsigned int q = 7;
  unsigned int stddev = 2;
  MP12 MP(q, stddev);
  unsigned int tar = MP.binarySearch(1);

  cout << "O" << endl;
  Matrix Ans = MP.O(1);
  Ans.print();

  cout << "fG inverse u:" << endl;
  Matrix u(3, 1);
  u.set(0, 0, 1);
  u.set(1, 0, 1);
  u.set(2, 0, 0);
  u.print();
  Matrix x = MP.fGInverse(u);
  cout << "result x is :" << endl;
  x.print();
}

void MP12::testfA() {
  unsigned int n = 4;
  unsigned int q = 7;
  unsigned int stddev = 2;

  MP12 MP(q, stddev);

  Matrix u(n, 1);
  u.set(0, 0, 1);
  u.set(1, 0, 1);
  u.set(2, 0, 0);
  u.set(3, 0, 1);

  pair<Matrix, Matrix> ATA = trapGen(n);
  Matrix x = fAInverse(ATA.first, ATA.second, u);
  cout << "A:" << endl;
  ATA.first.print();
  cout << "u:" << endl;
  u.print();
  cout << "x:" << endl;
  x.print();
  Matrix res = ATA.first * x;
  cout << "Ax=:" << endl;
  res.print();
}

void MP12::testfAwithoutV() {
  unsigned int n = 4;
  unsigned int q = 71;
  unsigned int stddev = 2;
  MP12 MP(q, stddev);

  Matrix u(n, 1);
  u.set(0, 0, 1);
  u.set(1, 0, 2);
  u.set(2, 0, 0);
  u.set(3, 0, 1);

  pair<Matrix, Matrix> ATA = trapGen(n);
  Matrix x = fAInversewithoutVariance(ATA.first, ATA.second, u);
  cout << "A:" << endl;
  ATA.first.print();
  cout << "u:" << endl;
  u.print();
  cout << "x:" << endl;
  x.print();
  Matrix res = ATA.first * x;
  cout << "Ax=:" << endl;
  res.print();
}

pair<Matrix, Matrix> MP12::delTrap(const Matrix& A, const Matrix& T_A,
                                   const Matrix& A1, double stddev) {
  Matrix A_prime = Matrix::horizontalConcat(A, A1);
  Matrix G = Matrix::generateGadgetMatrix(A.getRows());
  Matrix target = G - A1;

  Matrix T_A_prime =
      fAInversewithoutVariance(A_prime, T_A, target.getColVector(0));
  for (size_t i = 1; i < target.getCols(); i++) {
    Matrix u = target.getColVector(i);
    Matrix T_A_prime_col = fAInversewithoutVariance(A_prime, T_A, u);
    T_A_prime = Matrix::horizontalConcat(T_A_prime, T_A_prime_col);
  }

  return make_pair(A_prime, T_A_prime);
}

void MP12::testDelTrap() {
  unsigned int n = 4;
  unsigned int q = 71;
  unsigned int stddev = 2;
  MP12 MP(q, stddev);

  pair<Matrix, Matrix> trapPairA = trapGen(n);
  Matrix A = trapPairA.first;
  Matrix T_A = trapPairA.second;

  Matrix A1 = Matrix::generateUniformRandomMatrix(n, n * Matrix::getK());

  pair<Matrix, Matrix> trapPairA_prime = delTrap(A, T_A, A1, MP.getStddev());

  cout << "A rows:" << A.getRows() << " cols:" << A.getCols() << endl;
  cout << "T_A rows:" << T_A.getRows() << " cols:" << T_A.getCols() << endl;
  cout << "A1 rows:" << A1.getRows() << " cols:" << A1.getCols() << endl;
  cout << "A' rows:" << trapPairA_prime.first.getRows()
       << " cols:" << trapPairA_prime.first.getCols() << endl;
  cout << "T_A' rows:" << trapPairA_prime.second.getRows()
       << " cols:" << trapPairA_prime.second.getCols() << endl;

  Matrix fakeT_A_prime = Matrix::generateUniformRandomMatrix(
      trapPairA_prime.second.getRows(), trapPairA_prime.second.getCols());

  Matrix u(n, 1);
  u.set(0, 0, 1);
  u.set(1, 0, 2);
  u.set(2, 0, 0);
  u.set(3, 0, 1);
  Matrix x = fAInversewithoutVariance(trapPairA_prime.first,
                                      trapPairA_prime.second, u);

  Matrix res = trapPairA_prime.first * x;
  if (res != u) {
    throw runtime_error("res != u");
  }

  res.print();
}

Matrix MP12::SampleLeft(const Matrix& A, const Matrix& M1,
                        const Matrix& trapdoorA, const Matrix& u) {
  unsigned int n = A.getRows();
  unsigned int m = A.getCols();
  unsigned int m1 = M1.getCols();

  Matrix e2 = Matrix::generateDiscreteGaussianMatrix(m1, 1, SIGMA);
  // cout << "M1 size is " << M1.getRows() << " x " << M1.getCols() << endl;
  // cout << "e2 size is " << e2.getRows() << " x " << e2.getCols() << endl;
  Matrix y = u - M1 * e2;

  Matrix e1 = fAInversewithoutVariance(A, trapdoorA, y);
  // cout << "A size is " << A.getRows() << " x " << A.getCols() << endl;
  // cout << "e1 size is " << e1.getRows() << " x " << e1.getCols() << endl;

  Matrix e = Matrix::verticalConcat(e1, e2);

  // cout << "SampleLeft e size is " << e.getRows() << " x " << e.getCols()
  //      << endl;
  return e;
}
