#include <vector>
#include <string>
#include <math.h> 

using namespace std;

#include "attribute.h"

/*****************************************************************************************
 * constructors
 ****************************************************************************************/
 
//constructor
Attribute::Attribute() {
}

int Attribute::value_name2index (string name) {
  for(int i=0; i<value_count; i++) {
    if(name.compare(value_names[i])==0) return i;
  }
  assert(0);
  return 0; //to avoid compiler warning
}

/*****************************************************************************************
 * information gain computations
 ****************************************************************************************/
 
double Attribute::entropy(double q){
  assert(q >= 0);
  assert(q <= 1);
  double e;
  if(q==0 || q==1) e = 0;
  else e = -(q * log2(q) + (1-q) * log2(1-q));
  assert(e >= 0);
  return e;
}

double Attribute::remainder(int total_count){
  double remainder = 0;
  for(int v=0; v<value_count; v++){ //iterate over values of attribute
    int pv_count = positive_counts[v];
    int nv_count = negative_counts[v];
    int v_count  = pv_count+nv_count;
    if(v_count>0) { //otherwise skip this value of the attribute
      double probability = ((double) pv_count)/v_count;
      remainder += (((double)v_count)/total_count) * entropy(probability); 
    }
  }
  return remainder;
}

double Attribute::compute_information_gain(int positive_count, int negative_count) {
  int total_count = positive_count + negative_count;
//  double probability = ((double)positive_count)/total_count;
//  assert(probability >=0 && probability <=1);
//  double ent = entropy(probability);
  double rem = remainder(total_count);
//  double gain = ent - rem;
//  assert(ent >= rem);
//  return information_gain = gain;
  return information_gain = rem; //minimize instead of maximize
} 

  
  
/*****************************************************************************************
 * end
 ****************************************************************************************/