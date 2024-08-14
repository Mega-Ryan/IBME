#include "Tree.hpp"

// TreeNode constructor
TreeNode::TreeNode()
    : u1(), u2(), left(nullptr), right(nullptr), parent(nullptr) {}

// BinaryTree private helper function for findithLeaf
TreeNode* BinaryTree::findLeafHelper(TreeNode* node, int& currentIndex,
                                     int targetIndex) {
  if (!node) return nullptr;  // Base case: node is null

  // Check if the node is a leaf
  if (!node->left && !node->right) {
    if (currentIndex == targetIndex) {
      return node;  // Found the i-th leaf
    }
    currentIndex++;  // Update leaf count
  }

  // Recursively search in the left subtree
  TreeNode* leftResult = findLeafHelper(node->left, currentIndex, targetIndex);
  if (leftResult) return leftResult;  // If found in the left subtree, return it

  // Recursively search in the right subtree
  return findLeafHelper(node->right, currentIndex, targetIndex);
}

// BinaryTree default constructor
BinaryTree::BinaryTree() : root(nullptr) {}

// BinaryTree constructor with leaves parameter
BinaryTree::BinaryTree(int leaves) : root(initializeTree(leaves)) {}

// BinaryTree method to initialize the tree
TreeNode* BinaryTree::initializeTree(int leaves) {
  if (leaves == 1) {
    return new TreeNode();
  }
  int h = ceil(log2(leaves + 1));
  int maxLeaves = pow(2, h - 1);
  int leftLeaves = max(maxLeaves / 2, leaves - maxLeaves / 2);
  int rightLeaves = leaves - leftLeaves;

  TreeNode* node = new TreeNode();
  node->left = initializeTree(leftLeaves);
  node->right = initializeTree(rightLeaves);
  node->left->parent = node;
  node->right->parent = node;
  return node;
}

// BinaryTree method to find the i-th leaf node
TreeNode* BinaryTree::findithLeaf(int i) {
  int currentIndex = 0;
  // cout << "Finding the " << i << "-th leaf node..." << endl;
  return findLeafHelper(root, currentIndex, i);
}

// BinaryTree method to get the path from the root to a given node
set<TreeNode*> BinaryTree::path(TreeNode* v) {
  std::set<TreeNode*> pathNodes;

  // Traverse from the target node back to the root using parent pointers
  TreeNode* current = v;
  while (current != nullptr) {
    pathNodes.insert(current);
    current = current->parent;
  }
  return pathNodes;
}

// BinaryTree method to get the KUNodes
set<TreeNode*> BinaryTree::KUNodes(const set<pair<TreeNode*, int>>& RL, int t) {
  set<TreeNode*> U;
  set<TreeNode*> L;
  for (auto item : RL) {
    if (item.second <= t) {
      set<TreeNode*> pathNodes = path(item.first);

      L.insert(pathNodes.begin(), pathNodes.end());
    }
  }

  for (auto node : L) {
    if (node->left && L.find(node->left) == L.end()) {
      U.insert(node->left);
    }
    if (node->right && L.find(node->right) == L.end()) {
      U.insert(node->right);
    }
  }

  if (U.empty()) {
    U.insert(root);
  }

  // cout << "U size: " << U.size() << endl;

  return U;
}

// BinaryTree method to print the tree
void BinaryTree::print() {
  if (root == nullptr) {
    cout << "The tree is empty." << endl;
    return;
  }

  queue<TreeNode*> q;
  q.push(root);

  while (!q.empty()) {
    int levelSize = q.size();

    for (int i = 0; i < levelSize; ++i) {
      TreeNode* currentNode = q.front();
      q.pop();

      // Print current node's information
      cout << "(";
      if (!currentNode->u1.empty()) {
        cout << "u1: " << currentNode->u1.size() << " ";
      }
      if (!currentNode->u2.empty()) {
        cout << "u2: " << currentNode->u2.size();
      }
      cout << ") ";

      if (currentNode->left != nullptr) {
        q.push(currentNode->left);
      }
      if (currentNode->right != nullptr) {
        q.push(currentNode->right);
      }
    }
    cout << endl;
  }
}
