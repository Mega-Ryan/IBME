#ifndef TREE_HPP
#define TREE_HPP

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

#include "Matrix.hpp"

class TreeNode {
 public:
  vector<Matrix> u1;  // Vector to store u_i_N_1
  vector<Matrix> u2;  // Vector to store u_i_N_2
  TreeNode* left;
  TreeNode* right;
  TreeNode* parent;

  // Constructor to initialize the node with default values
  TreeNode();
};

class BinaryTree {
 private:
  // Helper function for findithLeaf
  TreeNode* findLeafHelper(TreeNode* node, int& currentIndex, int targetIndex);

 public:
  TreeNode* root;

  BinaryTree();
  BinaryTree(int leaves);

  TreeNode* initializeTree(int leaves);

  // Find the i-th leaf node
  TreeNode* findithLeaf(int i);

  set<TreeNode*> path(TreeNode* v);

  set<TreeNode*> KUNodes(const set<pair<TreeNode*, int>>& RL, int t);

  void print();
};

#endif  // TREE_HPP