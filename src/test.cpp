#include <Hash.hpp>
#include <IB-ME.hpp>
#include <MP12.hpp>
#include <Tree.hpp>
#include <chrono>
#include <iostream>

enum TEST_SITUATION { NORMAL, ID_MISMATCH, REVOKED };

void testIBME(enum TEST_SITUATION situation) {
  IBME ibme;
  cout << "IB-ME setup successfully!" << endl;

  int sender_id = 2;
  int receiver_id = 3;

  int time0 = 0;
  int time1 = 1;

  vector<Matrix> sender_key;
  vector<vector<pair<TreeNode*, vector<Matrix>>>> receiver_key;
  vector<pair<TreeNode*, vector<Matrix>>> key_update;
  vector<pair<Matrix, Matrix>> decrytion_key;

  bitset<MESSAGE_LEN> message("10100111");

  for (unsigned int i = 0; i < USER_NUM; i++) {
    sender_key.push_back(ibme.SKGen(i));
  }
  cout << "SKGen function executed successfully!" << endl;

  for (unsigned int i = 0; i < USER_NUM; i++) {
    receiver_key.push_back(ibme.RKGen(i));
  }
  cout << "RKGen function executed successfully!" << endl;

  // test when the scheme works properly
  if (situation == NORMAL) {
    cout << "Test when the scheme works properly" << endl;
    // revocalition list = empty, update time = 0, sender_id = 2, receiver_id =
    // 3
    key_update = ibme.KUpdGen(ibme.RL, time0);
    cout << "KUpdGen function executed successfully!" << endl;

    // test DKGen
    decrytion_key =
        ibme.DKGen(receiver_key[receiver_id], receiver_id, key_update, time0);
    cout << "DKGen function executed successfully!" << endl;

    // test Enc
    pair<vector<Matrix>, Matrix> ct =
        ibme.Enc(sender_key[sender_id], sender_id, receiver_id, message, time0);
    for (unsigned int i = 0; i < MESSAGE_LEN; i++) {
      cout << "ct.first[" << i << "]: " << endl;
      ct.first[i].print();
    }
    cout << "Enc function executed successfully!" << endl;

    // test Dec
    string decrypted_message =
        ibme.Dec(decrytion_key, receiver_id, sender_id, ct);
    cout << "decrypted_message: " << decrypted_message << endl;
    cout << "Dec function executed successfully!" << endl;
  }

  // test when the receiver is not the intended receiver
  if (situation == ID_MISMATCH) {
    // revocalition list = empty, update time = 0, sender_id = 2, receiver_id =
    // 3
    key_update = ibme.KUpdGen(ibme.RL, time0);
    cout << "KUpdGen function executed successfully!" << endl;

    // test DKGen
    // note that we here create a decryption key for receiver_id + 1
    decrytion_key = ibme.DKGen(receiver_key[receiver_id + 1], receiver_id + 1,
                               key_update, time0);
    cout << "DKGen function executed successfully!" << endl;

    // test Enc
    // note that the intended receiver of this ciphertext is receiver_id
    pair<vector<Matrix>, Matrix> ct =
        ibme.Enc(sender_key[sender_id], sender_id, receiver_id, message, time0);
    for (unsigned int i = 0; i < MESSAGE_LEN; i++) {
      cout << "ct.first[" << i << "]: " << endl;
      ct.first[i].print();
    }
    cout << "Enc function executed successfully!" << endl;

    // test Dec
    // it should throws an exception because the decryption key here is not the
    // intended receiver's decryption key, so the verification of the signature
    // will fail
    string decrypted_message =
        ibme.Dec(decrytion_key, receiver_id + 1, sender_id, ct);
    cout << "decrypted_message: " << decrypted_message << endl;
    cout << "Dec function executed successfully!" << endl;
  }

  // test when the receiver key is revoked
  if (situation == REVOKED) {
    // revocation list = {(1, time1)}, update time = 1, sender_id = 2,
    // receiver_id = 3
    ibme.KRev(receiver_id, time1);

    key_update = ibme.KUpdGen(ibme.RL, time1);
    cout << "KUpdGen function executed successfully!" << endl;

    // test DKGen
    // it should throws an exception because the receiver key is revoked, so no
    // valid decryption key can be generated
    decrytion_key =
        ibme.DKGen(receiver_key[receiver_id], receiver_id, key_update, time1);
    // ideally, code below should not be executed
    cout << "DKGen function executed successfully!" << endl;

    // test Enc
    pair<vector<Matrix>, Matrix> ct =
        ibme.Enc(sender_key[sender_id], sender_id, receiver_id, message, time1);
    for (unsigned int i = 0; i < MESSAGE_LEN; i++) {
      cout << "ct.first[" << i << "]: " << endl;
      ct.first[i].print();
    }

    cout << "Enc function executed successfully!" << endl;

    // test Dec
    string decrypted_message =
        ibme.Dec(decrytion_key, receiver_id, sender_id, ct);
    cout << "decrypted_message: " << decrypted_message << endl;
    cout << "Dec function executed successfully!" << endl;
  }
}

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

void benchmarkIBMEfunc() {
  IBME ibme;
  vector<Matrix> sender_key(USER_NUM);
  vector<vector<pair<TreeNode*, vector<Matrix>>>> receiver_key(USER_NUM);
  vector<pair<TreeNode*, vector<Matrix>>> key_update;
  vector<pair<Matrix, Matrix>> decrytion_key;
  pair<vector<Matrix>, Matrix> ct;
  bitset<MESSAGE_LEN> message("01011111");

  int sender_id = 2;
  int receiver_id = 3;

  int time0 = 0;
  int time1 = 1;

  cout << "test IBME" << endl;
  cout << "test Setup" << endl;
  auto sstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    IBME setup;
  }
  auto send = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> sduration = send - sstart;
  std::cout << "Setup time: " << sduration.count() / 10 << " ms" << std::endl;

  cout << "test SKGen" << endl;
  auto skstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    sender_key[i % USER_NUM] = ibme.SKGen(i % USER_NUM);
  }
  auto skend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> skduration = skend - skstart;
  std::cout << "SKGen time: " << skduration.count() / 10 << " ms" << std::endl;

  cout << "test RKGen" << endl;
  auto rkstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    receiver_key[i % USER_NUM] = ibme.RKGen(i % USER_NUM);
  }
  auto rkend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> rkduration = rkend - rkstart;
  std::cout << "RKGen time: " << rkduration.count() / 10 << " ms" << std::endl;

  cout << "test KUpdGen" << endl;
  auto kupdstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    key_update = ibme.KUpdGen(ibme.RL, time0);
  }
  auto kupdend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> kupdduration = kupdend - kupdstart;
  std::cout << "KUpdGen time: " << kupdduration.count() / 10 << " ms"
            << std::endl;

  cout << "test Enc" << endl;
  auto encstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    ct =
        ibme.Enc(sender_key[sender_id], sender_id, receiver_id, message, time0);
  }
  auto encend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> encduration = encend - encstart;
  std::cout << "Enc time: " << encduration.count() / 10 << " ms" << std::endl;

  cout << "test DKGen" << endl;
  auto dkstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    decrytion_key =
        ibme.DKGen(receiver_key[receiver_id], receiver_id, key_update, time0);
  }
  auto dkend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> dkduration = dkend - dkstart;
  std::cout << "DKGen time: " << dkduration.count() / 10 << " ms" << std::endl;

  cout << "test Dec" << endl;
  auto decstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    string decrypted_message =
        ibme.Dec(decrytion_key, receiver_id, sender_id, ct);
  }
  auto decend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> decduration = decend - decstart;
  std::cout << "Dec time: " << decduration.count() / 10 << " ms" << std::endl;

  cout << "test KRev" << endl;
  auto krstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    ibme.KRev(receiver_id, time1);
  }
  auto krend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> krduration = krend - krstart;
  std::cout << "KRev time: " << krduration.count() / 10 << " ms" << std::endl;
  cout << "--------------------------------------------------------------"
       << endl;
}

void normalIBME() {
  IBME ibme;
  cout << "IB-ME setup successfully!" << endl;

  int sender_id = 2;
  int receiver_id = 3;

  int time0 = 0;
  int time1 = 1;

  vector<Matrix> sender_key;
  vector<vector<pair<TreeNode*, vector<Matrix>>>> receiver_key;
  vector<pair<TreeNode*, vector<Matrix>>> key_update;
  vector<pair<Matrix, Matrix>> decrytion_key;

  bitset<MESSAGE_LEN> message("10100111");

  for (unsigned int i = 0; i < USER_NUM; i++) {
    sender_key.push_back(ibme.SKGen(i));
  }
  // cout << "SKGen function executed successfully!" << endl;

  for (unsigned int i = 0; i < USER_NUM; i++) {
    receiver_key.push_back(ibme.RKGen(i));
  }
  // cout << "RKGen function executed successfully!" << endl;

  // test when the scheme works properly
  // cout << "Test when the scheme works properly" << endl;
  // revocalition list = empty, update time = 0, sender_id = 2, receiver_id =
  // 3
  key_update = ibme.KUpdGen(ibme.RL, time0);
  // cout << "KUpdGen function executed successfully!" << endl;

  // test DKGen
  decrytion_key =
      ibme.DKGen(receiver_key[receiver_id], receiver_id, key_update, time0);
  // cout << "DKGen function executed successfully!" << endl;

  // test Enc
  pair<vector<Matrix>, Matrix> ct =
      ibme.Enc(sender_key[sender_id], sender_id, receiver_id, message, time0);
  // for (unsigned int i = 0; i < MESSAGE_LEN; i++) {
  //   cout << "ct.first[" << i << "]: " << endl;
  //   ct.first[i].print();
  // }
  // cout << "Enc function executed successfully!" << endl;

  // test Dec
  string decrypted_message =
      ibme.Dec(decrytion_key, receiver_id, sender_id, ct);
  // cout << "decrypted_message: " << decrypted_message << endl;
  // cout << "Dec function executed successfully!" << endl;
}

void benchmarkIBME() {
  cout << "test Whole system" << endl;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    normalIBME();
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;
  std::cout << "Whole system time: " << duration.count() / 10 << " ms"
            << std::endl;

  cout << "------------------------------------------------------------------"
       << endl;
}

int main() {
  // testIBME(NORMAL);
  benchmarkOp();

  // testIBMEfunc();
  benchmarkIBMEfunc();

  // testIBME();
  benchmarkIBME();
  return 0;
}
