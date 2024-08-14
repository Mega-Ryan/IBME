#include <Hash.hpp>
#include <IB-ME.hpp>
#include <MP12.hpp>
#include <Tree.hpp>
#include <chrono>
#include <iostream>

void normalIBME() {
  IBME ibme;
  // cout << "IB-ME setup successfully!" << endl;

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
  for (int i = 0; i < 5; ++i) {
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
  // testIBME();
  benchmarkIBME();
  
  return 0;
}