#ifndef DATA_H
#define DATA_H

#include <vector>
#include <fstream>
#include <assert.h>

#include "attribute.h"
#include "decision_tree.h"

using namespace std;

class Data {

  public:
  
  int attribute_count; //column count
  int example_count; //row count
  
  vector<vector<int> > attribute_columns;
  vector<int> label_column;
  vector<int> skip_column;
  int set_attribute_count;
  
  //label information 
  //only two values for label (label_value_count==2)
  //positive label is label_value_names[0]
  //negative label is label_value_names[1]
  string label_name; //name of label attribute
  int label_value_count; //must be 2
  vector<string> label_value_names;
  
  //attribute information
  vector<Attribute*> attributes;
  
  //information gain
  int positive_count;
  int negative_count;
  
  //methods
  Data();
  Data(string file_name);

  int label_name2index(string name);
  int positive_index();
  int negative_index();
  string positive_label();
  string negative_label();
  void read(istream &InputFile);
  void print(ostream &OutputFile);
  void read_from_file(string filename);
  void print_to_file(string filename);
  
  //methods for decision trees
  void set_attribute(int c, int value);
  void unset_attribute(int c, int value);
  int is_attribute_set(int c);
  int all_attributes_set();
  int skip_example(int r);
  void count_examples();
  int best_attribute();
  DecisionTree *decision_tree(); //constructs a decision tree from data
  DecisionTree *decision_tree(int default_index, string default_label); //supporting function
  double classify(int print, DecisionTree *dt); //classifies examples in data and return accuracy
  int classify_example(int r, DecisionTree *dt); //supporting function
  
  };

#endif
