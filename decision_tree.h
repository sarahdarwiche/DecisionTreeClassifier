#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

#include "attribute.h"

using namespace std;

class DecisionTree {

  public:
  
  int is_leaf; //whether decision tree node is a leaf node
  
  //in case it is a leaf
  int label_index; //1 or 0
  string label_name; 
  
  //in case it is not a leaf (internal node)
  Attribute *attribute;
  vector<DecisionTree*> children; //ordered according to values of attribute
  int positive_count;
  int negative_count;
  
  public:
  
  //methods
  DecisionTree(Attribute *attribute, int positive_count, int negative_count); //constructs an internal node
  DecisionTree(int label_index, string label_name); //constructs a leaf node
  
  int children_count();
  string name();
  string edge_name(int index);
  DecisionTree *child(int index);
  void add_child(int index, DecisionTree* child);
  int all_leaf_children();
  int size();
  
  DecisionTree *prune();
  
  void save_as_dot(string file_name); //writes out gv file for decision tree
  void save_as_dot(int id, ostream &OutputFile); //supporting function
  
};


#endif