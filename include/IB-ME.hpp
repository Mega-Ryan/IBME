#ifndef IBME_HPP
#define IBME_HPP

#include <bitset>
#include <cmath>
#include <set>
#include <utility>
#include <vector>

#include "Hash.hpp"
#include "MP12.hpp"
#include "Tree.hpp"
#include "Utils.hpp"

#define USER_NUM 4
#define MESSAGE_LEN 12
#define MODULUS 3329
#define K ceil(log2(MODULUS))
#define ROWS 96
#define COLS ROWS* K
#define SIGNATURE_LEN 3 * COLS* K
#define N MESSAGE_LEN + SIGNATURE_LEN
#define SIGMA MODULUS / COLS
#define ALPHA 1 / (COLS * COLS)  // between 0 and 1
#define NOISE_SIGMA ALPHA / sqrt(2 * M_PI)

class IBME {
 private:
  Matrix trapdoorA;
  Matrix trapdoorA_prime;

 public:
  Matrix A;
  Matrix A_prime;
  Matrix B1;
  Matrix B2;
  Matrix C1;
  Matrix C2;
  vector<Matrix> u;
  BinaryTree* tree;
  set<pair<TreeNode*, int>> RL;

  IBME();
  Matrix SKGen(int sender_id);
  vector<pair<TreeNode*, vector<Matrix>>> RKGen(int rcvr_id);
  vector<pair<TreeNode*, vector<Matrix>>> KUpdGen(
      const set<pair<TreeNode*, int>>& RL, int time);
  vector<pair<Matrix, Matrix>> DKGen(
      const vector<pair<TreeNode*, vector<Matrix>>>& rk_receiverid,
      int receiver_id, const vector<pair<TreeNode*, vector<Matrix>>>& ku_t,
      int t);
  pair<vector<Matrix>, Matrix> Enc(const Matrix& ek_senderid, int sender_id,
                                   int receiver_id,
                                   const bitset<MESSAGE_LEN>& message,
                                   int time);
  string Dec(const vector<pair<Matrix, Matrix>>& dk_receiverid_t,
             int receiver_id, int sender_id,
             const pair<vector<Matrix>, Matrix>& ct);
  void KRev(int user_id, int time);
};

#endif  // IBME_HPP
