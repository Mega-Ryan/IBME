#include "IB-ME.hpp"
#include <chrono>

IBME::IBME() {
  Matrix::setModulus(MODULUS);
  BigInt q = Matrix::getModulus();
  unsigned int n = ROWS;
  unsigned int k = Matrix::getK();
  unsigned int m = n * k;

  MP12 MP(q, SIGMA);

  pair<Matrix, Matrix> trapPairA = MP12::trapGen(n);
  pair<Matrix, Matrix> trapPairA_prime = MP12::trapGen(n);
  cout << "trapPairA.first size = " << trapPairA.first.getRows() << " x " << trapPairA.first.getCols() << endl;

  Matrix B1 = Matrix::generateUniformRandomMatrix(n, 2 * m);
  Matrix B2 = Matrix::generateUniformRandomMatrix(n, 2 * m);
  Matrix C1 = Matrix::generateUniformRandomMatrix(n, 2 * m);
  Matrix C2 = Matrix::generateUniformRandomMatrix(n, 2 * m);

  vector<Matrix> u;
  for (unsigned int i = 0; i < N; i++) {
    Matrix u_i = Matrix::generateUniformRandomMatrix(n, 1);
    u.push_back(u_i);
  }

  // output trapPair1, trapPair2, B1, B2, C1, C2, u
  this->A = trapPairA.first;
  this->A_prime = trapPairA_prime.first;
  this->trapdoorA = trapPairA.second;
  this->trapdoorA_prime = trapPairA_prime.second;
  this->B1 = B1;
  this->B2 = B2;
  this->C1 = C1;
  this->C2 = C2;
  this->u = u;

  tree = new BinaryTree(USER_NUM);
  RL = {};
}

Matrix IBME::SKGen(int sender_id) {
  if (sender_id < 0 || sender_id >= USER_NUM) {
    throw invalid_argument(
        "SKGen: invalid sender ID, should be between 0 and USER_NUM - 1");
  }

  unsigned int n = A.getRows();
  BigInt q = Matrix::getModulus();
  unsigned int k = Matrix::getK();
  unsigned int m = n * k;

  Hash h1 = Hash(n, m);
  Matrix h_senderid = h1.hash(to_string(sender_id));

  Matrix F_senderid = Matrix::horizontalConcat(A_prime, h_senderid);

  Matrix ek_senderid =
      MP12::delTrap(A_prime, trapdoorA_prime, h_senderid, SIGMA).second;

  return ek_senderid;
}

vector<pair<TreeNode*, vector<Matrix>>> IBME::RKGen(int receiver_id) {
  if (receiver_id < 0 || receiver_id >= USER_NUM) {
    throw invalid_argument(
        "RKGen: invalid receiver ID, should be between 0 and USER_NUM - 1");
  }

  unsigned int n = A.getRows();
  BigInt q = Matrix::getModulus();
  unsigned int k = Matrix::getK();
  unsigned int m = n * k;

  TreeNode* v = tree->findithLeaf(receiver_id);
  set<TreeNode*> pathNodes = tree->path(v);

  vector<pair<TreeNode*, vector<Matrix>>> rk_receiverid;

  Hash h = Hash(n, n);

  // iterate pathNodes
  for (auto node : pathNodes) {
    pair<TreeNode*, vector<Matrix>> rk_node;
    rk_node.first = node;
    
    auto start = chrono::high_resolution_clock::now();
    if (node->u1.empty()) {
      for (unsigned int i = 0; i < N; i++) {
        Matrix temp = Matrix::generateUniformRandomMatrix(n, 1);
        node->u1.push_back(temp);
        node->u2.push_back(u[i] - temp);
      }
    }
    // cout << "node->u1.size() = " << node->u1.size() << endl;
    // cout << "node->u2.size() = " << node->u2.size() << endl;

    for (unsigned int i = 0; i < N; i++) {
      Matrix e_i_1 = MP12::SampleLeft(
          A, B1 + h.hash(to_string(receiver_id)) * C1, trapdoorA, node->u1[i]);
      rk_node.second.push_back(e_i_1);

      Matrix F_receiverid =
          Matrix::horizontalConcat(A, B1 + h.hash(to_string(receiver_id)) * C1);
      if (F_receiverid * e_i_1 != node->u1[i]) {
        throw runtime_error("RKGen: the generated receiver key is not correct");
      }
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time taken for one node in RKGen: " << duration.count() << " microseconds" << endl;
    rk_receiverid.push_back(rk_node);
  }

  return rk_receiverid;
}

vector<pair<TreeNode*, vector<Matrix>>> IBME::KUpdGen(
    const set<pair<TreeNode*, int>>& RL, int t) {
  if (t < 0) {
    throw invalid_argument("KUpdGen: t should be greater than or equal to 0");
  }

  unsigned int n = A.getRows();
  BigInt q = Matrix::getModulus();
  unsigned int k = Matrix::getK();
  unsigned int m = n * k;

  set<TreeNode*> U = tree->KUNodes(RL, t);

  vector<pair<TreeNode*, vector<Matrix>>> ku_t;

  Hash h = Hash(n, n);

  for (auto node : U) {
    pair<TreeNode*, vector<Matrix>> ku_node;
    ku_node.first = node;

    if (node->u2.empty()) {
      for (unsigned int i = 0; i < N; i++) {
        Matrix temp = Matrix::generateUniformRandomMatrix(n, 1);
        node->u2.push_back(temp);
        node->u1.push_back(u[i] - temp);
      }
    }

    for (unsigned int i = 0; i < N; i++) {
      Matrix e_i_2 = MP12::SampleLeft(A, B2 + h.hash(to_string(t)) * C2,
                                      trapdoorA, node->u2[i]);
      ku_node.second.push_back(e_i_2);

      Matrix F_t = Matrix::horizontalConcat(A, B2 + h.hash(to_string(t)) * C2);

      if (F_t * e_i_2 != node->u2[i]) {
        throw runtime_error("KUpdGen: the generated update key is not correct");
      }
    }

    ku_t.push_back(ku_node);
  }

  return ku_t;
}

vector<pair<Matrix, Matrix>> IBME::DKGen(
    const vector<pair<TreeNode*, vector<Matrix>>>& rk_receiverid,
    int receiver_id, const vector<pair<TreeNode*, vector<Matrix>>>& ku_t,
    int t) {
  unsigned int n = A.getRows();
  BigInt q = Matrix::getModulus();
  unsigned int k = Matrix::getK();
  unsigned int m = n * k;

  vector<pair<Matrix, Matrix>> dk_receiverid_t;

  for (auto rk : rk_receiverid) {
    for (auto ku : ku_t) {
      if (rk.first == ku.first) {
        // cout << "found a match" << endl;
        if (rk.second.size() != N || ku.second.size() != N) {
          throw runtime_error("rk.second.size() != N || ku.second.size() != N");
        }
        // make each element of rk.second and ku.second a pair
        for (unsigned int i = 0; i < N; i++) {
          pair<Matrix, Matrix> dk_i;
          dk_i.first = rk.second[i];
          dk_i.second = ku.second[i];
          dk_receiverid_t.push_back(dk_i);
        }
      }
    }
  }

  if (dk_receiverid_t.size() == 0) {
    throw runtime_error(
        "DKGen: cannot generate a proper decryption key, "
        "probably caused by a revoked receiver key");
  }

  Hash h = Hash(n, n);
  Matrix F_receiverid =
      Matrix::horizontalConcat(A, B1 + h.hash(to_string(receiver_id)) * C1);
  Matrix F_t = Matrix::horizontalConcat(A, B2 + h.hash(to_string(t)) * C2);
  for (unsigned i = 0; i < N; i++) {
    if (F_receiverid * dk_receiverid_t[i].first +
            F_t * dk_receiverid_t[i].second !=
        u[i]) {
      throw runtime_error("DKGen: the generated decryption key is not correct");
    }
  }

  return dk_receiverid_t;
}

pair<vector<Matrix>, Matrix> IBME::Enc(const Matrix& ek_senderid, int sender_id,
                                       int receiver_id,
                                       const bitset<MESSAGE_LEN>& message,
                                       int t) {
  if (sender_id == receiver_id) {
    throw invalid_argument("sender_id == receiver_id");
  }

  unsigned int n = A.getRows();
  BigInt q = Matrix::getModulus();
  unsigned int k = Matrix::getK();
  unsigned int m = n * k;

  string message_bitstring = message.to_string();

  Hash h = Hash(n, n);
  Hash h1 = Hash(n, m);
  Hash h2 = Hash(n, 1);

  Matrix h_m = h2.hash(to_string(sender_id) + message.to_string() +
                       to_string(receiver_id));

  Matrix h_senderid = h1.hash(to_string(sender_id));
  Matrix F_senderid = Matrix::horizontalConcat(A_prime, h_senderid);

  // Matrix fake_ek_senderid =
  // Matrix::generateUniformRandomMatrix(ek_senderid.getRows(),
  // ek_senderid.getCols());

  Matrix sigma = MP12::fAInversewithoutVariance(F_senderid, ek_senderid, h_m);

  if (F_senderid * sigma != h_m) {
    throw runtime_error("Enc: signature generation failed");
  }

  // cout << "Enc sigma size = " << sigma.getRows() << " x " << sigma.getCols()
  //      << endl;
  // cout << "Enc sigma values: " << endl;
  // sigma.print();

  // cout << "sigma size = " << sigma.getRows() << " x " << sigma.getCols()
  //      << endl;
  // cout << "sigma(0, 0) = " << sigma.get(0, 0) << endl;

  string sigma_bitstring("");
  for (unsigned int i = 0; i < sigma.getRows(); i++) {
    sigma_bitstring += Utils::valueToBitString(sigma.get(i, 0), q);
  }
  // cout << "sigma_bitstring = " << sigma_bitstring << endl;
  if (sigma_bitstring.size() != SIGNATURE_LEN) {
    throw runtime_error("sigma_bitstring.size() != SIGNATURE_LEN");
  }

  Matrix F_rcv = B1 + h.hash(to_string(receiver_id)) * C1;
  Matrix F_t = B2 + h.hash(to_string(t)) * C2;
  Matrix F_rcv_t =
      Matrix::horizontalConcat(A, Matrix::horizontalConcat(F_rcv, F_t));

  Matrix s = Matrix::generateUniformRandomMatrix(n, 1);
  // cout << "s:" << endl;
  // s.print();
  Matrix R1 = Matrix::generateSignMatrix(2 * m, 2 * m);
  Matrix R2 = Matrix::generateSignMatrix(2 * m, 2 * m);
  vector<Matrix> x(N);
  for (unsigned int i = 0; i < N; i++) {
    x[i] = Matrix::generateDiscreteGaussianMatrix(1, 1, NOISE_SIGMA);
  }

  Matrix y = Matrix::generateDiscreteGaussianMatrix(2 * m, 1, NOISE_SIGMA);

  Matrix z1 = R1.transpose() * y;
  Matrix z2 = R2.transpose() * y;

  vector<Matrix> c1;
  for (unsigned int i = 0; i < MESSAGE_LEN; i++) {
    Matrix message_i(1, 1);
    message_i.set(0, 0, (message_bitstring[i] - '0') * (BigInt)round(q / 2));

    Matrix c1_i = u[i].transpose() * s + x[i] + message_i;
    c1.push_back(c1_i);

    // cout << "message_i" << endl;
    // message_i.print();

    // cout << "u[i]" << endl;
    // u[i].print();

    // cout << "c1_i" << endl;
    // c1_i.print();

    // cout << "x[i]" << endl;
    // x[i].print();
  }

  // cout << "s" << endl;
  // s.print();

  for (unsigned int i = MESSAGE_LEN; i < N; i++) {
    Matrix sigma_i(1, 1);
    sigma_i.set(
        0, 0, (sigma_bitstring[i - MESSAGE_LEN] - '0') * (BigInt)round(q / 2));
    Matrix c1_i = u[i].transpose() * s + x[i] + sigma_i;
    c1.push_back(c1_i);
  }

  Matrix c2 = F_rcv_t.transpose() * s +
              Matrix::verticalConcat(y, Matrix::verticalConcat(z1, z2));

  return make_pair(c1, c2);
}

string IBME::Dec(const vector<pair<Matrix, Matrix>>& dk_receiverid_t,
                 int receiver_id, int sender_id,
                 const pair<vector<Matrix>, Matrix>& ct) {
  if (sender_id == receiver_id) {
    throw invalid_argument("sender_id == receiver_id");
  }

  unsigned int n = A.getRows();
  BigInt q = Matrix::getModulus();
  unsigned int k = Matrix::getK();
  unsigned int m = n * k;

  Matrix c20(2 * m, 1);
  for (unsigned int i = 0; i < 2 * m; i++) {
    c20.set(i, 0, ct.second.get(i, 0));
  }

  Matrix c21(2 * m, 1);
  for (unsigned int i = 0; i < 2 * m; i++) {
    c21.set(i, 0, ct.second.get(2 * m + i, 0));
  }

  Matrix c22(2 * m, 1);
  for (unsigned int i = 0; i < 2 * m; i++) {
    c22.set(i, 0, ct.second.get(4 * m + i, 0));
  }

  vector<Matrix> omega;
  for (unsigned int i = 0; i < N; i++) {
    Matrix omega_i = ct.first[i] -
                     dk_receiverid_t[i].first.transpose() *
                         Matrix::verticalConcat(c20, c21) -
                     dk_receiverid_t[i].second.transpose() *
                         Matrix::verticalConcat(c20, c22);

    if (omega_i.getRows() != 1 || omega_i.getCols() != 1) {
      throw runtime_error("omega_i should be a 1x1 matrix");
    }
    omega.push_back(omega_i);
  }

  string m_prime("");
  for (unsigned int i = 0; i < N; i++) {
    if (abs(omega[i].get(0, 0) - (BigInt)round(q / 2)) < (BigInt)floor(q / 4)) {
      m_prime += "1";
    } else {
      m_prime += "0";
    }
  }

  string message = m_prime.substr(0, MESSAGE_LEN);
  string sigma_bitstring = m_prime.substr(MESSAGE_LEN, SIGNATURE_LEN);

  // convert every k bits of sigma string to a value
  Matrix sigma(ceil(SIGNATURE_LEN / k), 1);
  for (unsigned int i = 0; i < SIGNATURE_LEN; i += k) {
    string sigma_i = sigma_bitstring.substr(i, k);
    BigInt value = Utils::bitStringToValue(sigma_i, q);
    sigma.set(i / k, 0, value);
  }

  // verify the signature
  Hash h1 = Hash(n, m);
  Hash h2 = Hash(n, 1);

  Matrix h_m = h2.hash(to_string(sender_id) + message + to_string(receiver_id));
  Matrix h_senderid = h1.hash(to_string(sender_id));
  Matrix F_senderid = Matrix::horizontalConcat(A_prime, h_senderid);

  if (F_senderid * sigma != h_m) {
    throw runtime_error("Dec: signature verification failed");
  }

  return message;
}

void IBME::KRev(int user_id, int time) {
  TreeNode* leaf = tree->findithLeaf(user_id);
  RL.insert(make_pair(leaf, time));
}