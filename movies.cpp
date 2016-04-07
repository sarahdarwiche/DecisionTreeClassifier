#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>

#include <sstream>
#include <algorithm>

#include "data.h"

using namespace std;

/*****************************************************************************************
 * utilities
 ****************************************************************************************/
 
void clean_word(string &word) {
  //remove the following characters
  char chars[] = "?.,;:\"(){}~!@#$%^&'<>*+-/=";
  for (int i = 0; i<strlen(chars); ++i) {
    word.erase(remove(word.begin(),word.end(),chars[i]),word.end());
  }
  //make it lowercase
//  transform(word.begin(),word.end(),word.begin(),::tolower);
}

//returns the number of examples in file
int count_examples(string movie_file) {
  ifstream is;
  is.open(movie_file);
  
  int count = 0;
  int label;
  string document;
  while (is >> label) {
    getline(is,document);
    ++count;
  }
  
  is.close();
  return count;
}

//returns a vector of words in a file
vector<string> read_word_attributes(string words_file) {
  ifstream is;
  is.open(words_file);
  
  //read words from file
  int number_of_words;
  is >> number_of_words;
  
  vector<string> word_list;
  word_list.resize(number_of_words);

  string word;
  while(number_of_words--) {
    is >> word;
    word_list[number_of_words] = word;
  }
  
  is.close();
  return word_list;
}

/*****************************************************************************************
 * convert a movie data file into a standard data file
 ****************************************************************************************/
  
void movie_data_to_standard_data(string movie_file, string data_file, string words_file) {
  //get word attributes
  vector<string> word_attributes = read_word_attributes(words_file); 
  
  ofstream data_os;
  data_os.open(data_file);
  
  //write header for data file
  int example_count = count_examples(movie_file);
  int attribute_count = word_attributes.size(); 
  data_os << example_count << " " << attribute_count << endl;
  data_os << "sentiment 2 1 0" << endl;
  
  //write attribute info
  for(int i=0; i<attribute_count; i++) {
     data_os  << word_attributes[i] << " 2 yes no" << endl;
  }
  
  ifstream movie_is;
  movie_is.open(movie_file);
  
  //convert each (label document) into data example
  int label;
  string document;
  while (movie_is >> label) { //iterating over documents
    getline(movie_is,document); //we now have (label document)
    
    //create set object to store words of document
    set<string> words_in_doc;
    
    //add words of document to set
    istringstream iss(document);
    string word;
    while(iss >> word) {
      clean_word(word);
      //add word to set
      words_in_doc.insert(word);
    }
    
    //iterate over word-attributes, determining whether each appears in the movie review
    //write data example based on the values of positive and negative words
    //vector iterator to go through vector and find word in set
    
    for(int i=0; i<attribute_count; i++) {
      string word = word_attributes[i];
      //if found write yes else write no
      if(words_in_doc.find(word) != words_in_doc.end()) data_os << "yes ";
      else data_os << "no ";
    }
    
    data_os << label << endl;
  }
  
  //close streams
  movie_is.close();
  data_os.close();
}


/*****************************************************************************************
 * end
 ****************************************************************************************/
  
