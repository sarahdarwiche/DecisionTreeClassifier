#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

#include "data.h"

using namespace std;

void parse_text(string filename);
void movie_data_to_standard_data(string movie_file, string data_file, string words_file);

//copies standard out to file_name
void cin_to_file(string file_name) {
  ofstream os;
  os.open(file_name);
  os << cin.rdbuf();
  os.close();
}

int main (int argc, char** argv) {

  string test_file, train_file, word_list_file, my_train_file, my_test_file;
  
  cin_to_file("sad_in_train.txt"); //write standard input to file
  
  train_file = "sad_in_train.txt";
  test_file  = argv[1];
    
  word_list_file = "sad_word_list.txt";
  my_train_file = "sad_out_train.txt";
  my_test_file  = "sad_out_test.txt";
  
  //generate data in my form
  movie_data_to_standard_data(train_file,my_train_file,word_list_file);
  movie_data_to_standard_data(test_file,my_test_file,word_list_file);
  
  Data train_data = Data(my_train_file);
  Data test_data  = Data(my_test_file);
  
  //start time
  clock_t start;
  double tree_build_duration;
  start = clock();
  
  //learn
  DecisionTree *dt = train_data.decision_tree();
//  cout << "Decision tree has " << dt->size() << " nodes" << endl;
  
  //prune
  dt = dt->prune();
  
  //end time
  tree_build_duration = ((double)(clock()-start))/CLOCKS_PER_SEC;
  cout << tree_build_duration << " seconds" << endl;
  
//  dt->save_as_dot("dt.gv");
  
  //check accuracy on train data
  double train_accuracy = train_data.classify(0,dt); //do not print classifications
  
  //start time
  double classify_test_duration;
  start = clock();
  
  //check accuracy on test data
  double test_accuracy = test_data.classify(1,dt); //print classifications
  
  //end time
  classify_test_duration = ((double)(clock()-start))/CLOCKS_PER_SEC;
  cout << classify_test_duration << " seconds" << endl;
  
  cout  <<  train_accuracy << " training accuracy" << endl;
  cout  <<  test_accuracy << " test accuracy" << endl;


  return 0;
}