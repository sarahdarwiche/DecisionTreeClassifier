#include <vector>
#include <string>
#include <math.h> 

using namespace std;

#include "decision_tree.h"

/*****************************************************************************************
 * constructors
 ****************************************************************************************/

//constructs an internal node
DecisionTree::DecisionTree(Attribute *a, int pc, int nc) {
  is_leaf = 0;
  attribute = a;
  positive_count = pc;
  negative_count = nc;
  children.resize(a->value_count);
} 

//constructs a leaf node
DecisionTree::DecisionTree(int index, string name) {
  is_leaf = 1;
  label_index = index;
  label_name  = name;
}

/*****************************************************************************************
 * utilities
 ****************************************************************************************/

//returns the number of children for decision tree node
int DecisionTree::children_count() {
  if(is_leaf) return 0;
  else return attribute->value_count;
}

//returns the name of the attribute for decision tree node
string DecisionTree::name() {
  if(is_leaf) return label_name; 
  else return attribute->name;
}

//returns the name of an edge outgoing from decision tree node
string DecisionTree::edge_name(int index) {
  assert(!is_leaf);
  assert(index < attribute->value_count);
  return attribute->value_names[index];
}  

//returns a child of a decision tree node
DecisionTree *DecisionTree::child(int index) {  
  assert(!is_leaf);
  assert(index < attribute->value_count);
  return children[index];
}

//adds a child for decision node
void DecisionTree::add_child(int index, DecisionTree* child) {
  assert(!is_leaf);
  assert(index < attribute->value_count);
  children[index] = child;
}

//check if all children of node are leaves
int DecisionTree::all_leaf_children() {
  assert(!is_leaf);
  for(int i=0; i<children_count(); i++) {
    if(child(i)->is_leaf==0) return 0;
  }
  return 1;
}

int DecisionTree::size() {
  if(is_leaf) return 1;
  else {
    int s = 1;
    for(int i=0; i<children_count(); i++) s += child(i)->size();
    return s;
  }
}

/*****************************************************************************************
 * decision tree prunning
 ****************************************************************************************/

DecisionTree* DecisionTree::prune() {
  if(is_leaf) return this;
  else {
    for(int i=0; i<children_count(); i++) {
      DecisionTree *child     = this->child(i);
      DecisionTree *new_child = child->prune();
      if(new_child!=child) {
        assert(child->is_leaf==0);
        for(int j=0; j<child->children_count(); j++) delete(child->child(j));
        delete(child);
        add_child(i,new_child);
      }
    }
    if(all_leaf_children()) { //try pruning
      int p = positive_count;
      int n = negative_count;
      int c = p+n;
      double deviation = 0;
      for(int i=0; i<children_count(); i++) {
        DecisionTree *child = this->child(i);
        int pi = child->positive_count;
        int ni = child->negative_count;
        int ci = pi+ni;
        double r = ((double)ci)/c;
        double epi = p*r;
        double eni = n*r;
        deviation += pow(pi-epi,2)/epi;
        deviation += pow(ni-eni,2)/eni;
      }
      if(deviation >= 7.82) {
//        cout << "pruning node" << endl;
        if(p>=n) return new DecisionTree(0,"yes");
        else return new DecisionTree(1,"no");
      }
      else return this;
    }
    else return this;
  }
}

/*****************************************************************************************
 * visualization
 ****************************************************************************************/

int ID = 0;

void DecisionTree::save_as_dot(int id, ostream &OutputFile) {
  OutputFile << "n" << id << " [label=" << name() << ",shape=box];" << endl;
  if(!is_leaf) {
    for(int i=0; i<children_count(); i++) {
      int child_id = ID++;
      OutputFile << "n" << id << "-> n" << child_id << "[label=" << edge_name(i) << ",headclip=true,arrowhead=none];" << endl;
      child(i)->save_as_dot(child_id,OutputFile);
    }
  }
}
 
void DecisionTree::save_as_dot(string file_name) {
  ofstream os;
  os.open(file_name);
  
  os << "digraph decision_tree {" << endl;
  os << "\noverlap=false" << endl << endl;
  save_as_dot(ID++,os);
  os << endl << endl << endl << "}";
  
  os.close();
}
  
  
/*****************************************************************************************
 * end
 ****************************************************************************************/