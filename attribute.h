#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

using namespace std;

class Attribute {

  public:
  
  int index; //index into attribute vector of data
  string name;
  double information_gain;
  int set_value; //-1 means unset
  
  int value_count; //number of values for attribute
  vector<string> value_names; //vector has size equal to value_count
  
  
  //information gain info
  //each of the following vectors has size equal to value_count
  vector<int> positive_counts; //one count per attribute value
  vector<int> negative_counts; //one count per attribute value
  
  //methods
  Attribute();
  
  int value_name2index(string name); 
  double compute_information_gain(int positive_count, int negative_count);
  double entropy(double q);
  double remainder(int total_count);
  
};


#endif