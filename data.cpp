#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "data.h"

using namespace std;

/*****************************************************************************************
 * constructors
 ****************************************************************************************/
 
//constructors
Data::Data() {
}

Data::Data(string file_name) {
  read_from_file(file_name);
}


/*****************************************************************************************
 * utilities
 ****************************************************************************************/

int Data::label_name2index(string name) {
  for(int i=0; i<label_value_count; i++) {
    if(name.compare(label_value_names[i])==0) return i;
  }
  assert(0);
  return 0; //to avoid compiler warning
}

//only two labels are allowed: positive and negative
int Data::positive_index() {
  return 0;
}
int Data::negative_index() {
  return 1;
}
string Data::positive_label() {
  return label_value_names[0];
}
string Data::negative_label() {
  return label_value_names[1];
}

/*****************************************************************************************
 * input / output
 ****************************************************************************************/

//reading data from standard input
void Data::read(istream &InputFile) {

  InputFile >> example_count; 
  InputFile >> attribute_count; 
  
  //allocate attributes vector
  attributes.resize(attribute_count);
  
  //allocate data matrix
  attribute_columns.resize(attribute_count); //allocate columns
  for(int i=0; i<attribute_count; i++){
    attribute_columns[i].resize(example_count); //allocate rows for examples
  }
  label_column.resize(example_count); //allocate rows for labels
  skip_column.resize(example_count); //allocate rows for visit bits
  for(int r=0; r<example_count; r++) skip_column[r] = 0; //examples are not skipped initially
  
  InputFile >> label_name; //reading
  InputFile >> label_value_count; //reading
  assert(label_value_count==2); //only two labels are allowed
  
  label_value_names.resize(label_value_count); 
  for(int i=0; i<label_value_count; i++){
    InputFile >> label_value_names[i]; //storing names of labels
  }
  
  //read attribute info
  set_attribute_count = 0; //no attributes are initially set
  for(int i=0; i<attribute_count; i++) {
    Attribute *attribute = new Attribute; 
    attributes[i] = attribute;
    attribute->index = i;
    attribute->set_value = -1; //initially, attribute is not set
    
    //reading attribute name and the number of its values
    InputFile >> attribute->name; //name of attribute
    int count; //number of values for attribute
    InputFile >> count;
    attribute->value_count = count;
    
    //reading names of values for attributes
    attribute->value_names.resize(count);
    for(int j=0; j<count; j++) InputFile >> attribute->value_names[j];
    
    //allocating +ve/-ve count vectors
    attribute->positive_counts.resize(count);
    attribute->negative_counts.resize(count);
  }
  
  //fill in data
  positive_count = negative_count = 0; //initialize counts
  for(int r=0; r<example_count; r++) {
    //example
    for(int c=0; c<attribute_count; c++) {
      string name;
      InputFile >> name; //attribute's value name
      attribute_columns[c][r] = attributes[c]->value_name2index(name);
    }
    //label
    string name;
    InputFile >> name; //label name
    int index = label_name2index(name);
    label_column[r] = index;
    if(index==0) ++positive_count;
    else ++negative_count;
  }
  assert(example_count==positive_count+negative_count);
  
}

void Data::print(ostream &OutputFile){

  OutputFile << "Data" << endl << "----" << endl;
  
  for(int c=0; c<attribute_count; c++) OutputFile << attributes[c]->name << " ";
  OutputFile << label_name << endl;
  OutputFile << "----" << endl;
  
  for(int r=0; r<example_count; r++) {
    OutputFile << r << ": ";
    for(int c=0; c<attribute_count; c++) {
      int value = attribute_columns[c][r];
      string name = attributes[c]->value_names[value];
      OutputFile << name << " ";
    }
    int value = label_column[r];
    string name = label_value_names[value];
    OutputFile <<  name << endl;
  }
  
  OutputFile << "----" << endl;
}

void Data::read_from_file(string filename){
  ifstream is;
  is.open(filename);
  read(is);
  is.close();
}

void Data::print_to_file(string filename){
  ofstream os;
  os.open(filename);
  print(os);
  os.close();
}

/*****************************************************************************************
 * setting attribute values
 ****************************************************************************************/
 
void Data::set_attribute(int c, int value) {
  assert(set_attribute_count>=0);
  ++set_attribute_count;
  Attribute *attribute = attributes[c];
  assert(attribute->set_value==-1); //attribute must be unset
  attribute->set_value = value;
  
  for(int r=0; r<example_count; r++) {
    assert(skip_column[r] >= 0);
    if(attribute_columns[c][r]!=value) ++skip_column[r];
  }
}

void Data::unset_attribute(int c, int value) {
  assert(set_attribute_count>0);
  --set_attribute_count;
  Attribute *attribute = attributes[c];
  assert(attribute->set_value!=-1); //attribute must already be set
  attribute->set_value = -1; //no set value
  
  for(int r=0; r<example_count; r++) {
    if(attribute_columns[c][r]!=value) --skip_column[r];
    assert(skip_column[r] >= 0);
  }
}

//returns 1 iff example r should be skipped
int Data::skip_example(int r) {
  assert(skip_column[r] >= 0);
  return skip_column[r]>0;
}

//checks whether attribute c is currently set
int Data::is_attribute_set(int c) {
  return attributes[c]->set_value != -1; //-1 is reserved for unset attributes
}
  
//checks whether all attributes have been set
int Data::all_attributes_set() {
  return set_attribute_count==attribute_count;
}

/*****************************************************************************************
 * information gain computations
 ****************************************************************************************/
 
//fills in the positive and negative counts of each attribute in the data
void Data::count_examples() {

  //zero out +ve/-ve counts for each attribute, and for the data
  positive_count = negative_count = 0;
  for(int c=0; c<attribute_count; c++) {
    Attribute *attribute = attributes[c]; 
    for(int v=0; v<attribute->value_count; v++) {
      attribute->positive_counts[v] = 0;
      attribute->negative_counts[v] = 0;
     }
  } 
  
  //pass over data, incrementing +ve/-ve counts for each attribute, and for the data
  for(int r=0; r<example_count; r++) {
    if(!skip_example(r)) {
      if(label_column[r] == 0) ++positive_count;
      else ++negative_count;
    }
  }
  
  //validation
  int total_count = 0;
  for(int r=0; r<example_count; r++) if(!skip_example(r)) ++total_count; 
  assert(positive_count+negative_count==total_count);
  
  for(int r=0; r<example_count; r++) {
    if(!skip_example(r)) {  
      for(int c=0; c<attribute_count; c++) {
        Attribute *attribute = attributes[c];
        int value = attribute_columns[c][r];
        if(label_column[r]==0) ++attribute->positive_counts[value];
        else ++attribute->negative_counts[value];
      }
    }
  } 
  
  //validation
  for(int c=0; c<attribute_count; c++) {
    Attribute *attribute = attributes[c];
    int pc = 0;
    for(int v=0; v<attribute->value_count; v++) pc += attribute->positive_counts[v];
    assert(pc==positive_count);
    int nc = 0;
    for(int v=0; v<attribute->value_count; v++) nc += attribute->negative_counts[v];
    assert(nc==negative_count);
  }
  
}

//returns the attribute with the highest information gain
int Data::best_attribute() {
  //count examples
  count_examples();
  
  //compute information gain for each attribute
//  double best_ig = -1;
  double best_ig = 2;
  int best_attribute = -1;
  
  for(int c=0; c<attribute_count; c++) {
    Attribute *attribute = attributes[c];
    if(!is_attribute_set(c)) {
      double ig = attribute->compute_information_gain(positive_count,negative_count);
//      if(ig > best_ig) {
      if(ig < best_ig) {
        best_ig = ig;
        best_attribute = c;
      }
    }
  }

  assert(best_attribute!=-1);
  return best_attribute;
}

/*****************************************************************************************
 * constructing a decision tree
 ****************************************************************************************/

//constructs a decision tree from data
DecisionTree *Data::decision_tree(int default_index, string default_label) {
  //count current positive and negative examples
  int p_count = 0;
  int n_count = 0;
  for(int r=0; r<example_count; r++) {
    if(!skip_example(r)) {
      if(label_column[r] == 0) ++p_count;
      else ++n_count;
    }
  }
  int total_count = p_count + n_count;
  assert(total_count <= example_count);

  int pi    = positive_index();
  int ni    = negative_index();
  string pl = positive_label();
  string nl = negative_label();
  
  if(total_count==0) { //no more examples
    return new DecisionTree(default_index,default_label);
  }
  else if(p_count==total_count) { //only positive examples
    return new DecisionTree(pi,pl); //leaf decision node
  }
  else if(n_count==total_count) { //only negative examples
    return new DecisionTree(ni,nl); //leaf decision node
  }
  else if(all_attributes_set()) { //no more attributes
    if(p_count >= n_count) return new DecisionTree(pi,pl); //leaf decision node
    else return new DecisionTree(ni,nl); //leaf decision node
  }
  else {
    int c = best_attribute();
    assert(c>=0 && c<attribute_count);
    Attribute *attribute = attributes[c];
    DecisionTree* tree = new DecisionTree(attribute,p_count,n_count); //internal decision node
    if(p_count >= n_count) {
      default_index = pi;
      default_label = pl;
    }
    else {
      default_index = ni;
      default_label = nl;
    }
    for(int v=0; v<attribute->value_count; v++) { //iterate over values of attribute
      set_attribute(c,v);
      DecisionTree *subtree = decision_tree(default_index,default_label); 
      unset_attribute(c,v);
      tree->add_child(v,subtree);
    }
    return tree;
  }
}
  
DecisionTree *Data::decision_tree() {
  return decision_tree(positive_index(),positive_label());
}

/*****************************************************************************************
 * classifying a dataset
 ****************************************************************************************/

//classify example r into 0 (positive) or 1 (negative)
int Data::classify_example(int r, DecisionTree *dt) {
  if(dt->is_leaf) return dt->label_index;
  else {
    int c = dt->attribute->index;
    int v = attribute_columns[c][r];
    return classify_example(r,dt->child(v));      
  }
}

//classifies examples in data and returns accuracy (percentage of correct classifications)
double Data::classify(int print, DecisionTree *dt) {
 int total   = example_count;
 int correct = 0;
 for(int r=0; r<example_count; r++) {
   int label_index = classify_example(r,dt);
   if(print) cout << label_value_names[label_index] << " " << endl;
   int correct_label_index = label_column[r]; 
   if(label_index==correct_label_index) ++correct;
 }
 return ((double)correct)/total;
}
/*****************************************************************************************
 * end
 ****************************************************************************************/
  
