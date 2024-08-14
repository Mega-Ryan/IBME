#include <Hash.hpp>
#include <IB-ME.hpp>
#include <MP12.hpp>
#include <Tree.hpp>
#include <chrono>
#include <iostream>

void benchmarkIBMEfunc() {
  cout << "test IBME function" << endl;
  cout << "test Setup" << endl;
  auto sstart = std::chrono::high_resolution_clock::now();
  IBME ibme;
  auto send = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> sduration = send - sstart;
  std::cout << "Setup time: " << sduration.count() << " ms" << std::endl;

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

  cout << "test SKGen" << endl;
  auto skstart = std::chrono::high_resolution_clock::now();
  // for (int i = 0; i < 10; ++i) {
  //   sender_key[i % USER_NUM] = ibme.SKGen(i % USER_NUM);

  //   cout << "SK[" << i % USER_NUM << "] generated" << endl;
  // }

  sender_key[sender_id] = ibme.SKGen(sender_id);

  auto skend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> skduration = skend - skstart;
  std::cout << "SKGen time: " << skduration.count() << " ms" << std::endl;

  cout << "test RKGen" << endl;
  auto rkstart = std::chrono::high_resolution_clock::now();
  // for (int i = 0; i < 10; ++i) {
  //   receiver_key[i % USER_NUM] = ibme.RKGen(i % USER_NUM);

  //   cout << "RK[" << i % USER_NUM << "] generated" << endl;
  // }

  receiver_key[receiver_id] = ibme.RKGen(receiver_id);
  auto rkend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> rkduration = rkend - rkstart;
  std::cout << "RKGen time: " << rkduration.count() << " ms" << std::endl;

  cout << "test KUpdGen" << endl;
  auto kupdstart = std::chrono::high_resolution_clock::now();
  // for (int i = 0; i < 10; ++i) {
  //   key_update = ibme.KUpdGen(ibme.RL, time0);

  //   cout << "KUpd[" << i << "] generated" << endl;
  // }
  key_update = ibme.KUpdGen(ibme.RL, time0);
  auto kupdend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> kupdduration = kupdend - kupdstart;
  std::cout << "KUpdGen time: " << kupdduration.count() << " ms"
            << std::endl;

  cout << "test Enc" << endl;
  auto encstart = std::chrono::high_resolution_clock::now();
  // for (int i = 0; i < 10; ++i) {
  //   ct =
  //       ibme.Enc(sender_key[sender_id], sender_id, receiver_id, message, time0);

  //   cout << "Enc[" << i << "] generated" << endl;
  // }
  ct = ibme.Enc(sender_key[sender_id], sender_id, receiver_id, message, time0);
  auto encend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> encduration = encend - encstart;
  std::cout << "Enc time: " << encduration.count() << " ms" << std::endl;

  cout << "test DKGen" << endl;
  auto dkstart = std::chrono::high_resolution_clock::now();
  // for (int i = 0; i < 10; ++i) {
  //   decrytion_key =
  //       ibme.DKGen(receiver_key[receiver_id], receiver_id, key_update, time0);

  //   cout << "DK[" << i << "] generated" << endl;
  // }
  decrytion_key =
      ibme.DKGen(receiver_key[receiver_id], receiver_id, key_update, time0);
  auto dkend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> dkduration = dkend - dkstart;
  std::cout << "DKGen time: " << dkduration.count() << " ms" << std::endl;

  cout << "test Dec" << endl;
  auto decstart = std::chrono::high_resolution_clock::now();
  // for (int i = 0; i < 10; ++i) {
  //   string decrypted_message =
  //       ibme.Dec(decrytion_key, receiver_id, sender_id, ct);

  //   cout << "Dec[" << i << "] generated" << endl;
  // }
  string decrypted_message =
      ibme.Dec(decrytion_key, receiver_id, sender_id, ct);
  auto decend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> decduration = decend - decstart;
  std::cout << "Dec time: " << decduration.count() << " ms" << std::endl;

  cout << "test KRev" << endl;
  auto krstart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10; ++i) {
    ibme.KRev(receiver_id, time1);
  }
  auto krend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> krduration = krend - krstart;
  std::cout << "KRev time: " << krduration.count() << " ms" << std::endl;
  cout << "--------------------------------------------------------------"
       << endl;
}

int main() {
  // testIBMEfunc();
  benchmarkIBMEfunc();

  return 0;
}