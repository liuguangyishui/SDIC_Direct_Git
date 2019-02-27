//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/*main fun. All begin of the program.
 */

#include <iostream>
#include <cstring>
#include <assert.h>
#include <regex>
#include "analysis_IR.h"
#include "out_put_fun.h"
#include "trance_fun.h"


using namespace::std;

int main(int argv, char **argc){

  vector<string> input_file_name;
  //  string input_file_name;
  string output_file_name;
  //identifiy '-' in -F -f or -o 
  regex reg("-.+"); 
  //deal with the para
  for(int i = 0; i < argv; i++){
    //whether is valid para
    if(regex_match(argc[i], reg)){
      //it is about input file
      if(strcmp(argc[i],"-f") == 0 || strcmp(argc[i],"-F") == 0){
	
	for(int j = i+1; j < argv; j++){
	  if(regex_match(argc[j], reg))
	    break;

	  input_file_name.push_back(argc[j]);
	  ++i;
	}
	//it is about outout file
      } else if(strcmp(argc[i], "-o") == 0){
	
	for(int j = i+1; j < argv; j++){
	  if(regex_match(argc[j], reg))
	    break;
	  
	  output_file_name = argc[j];
	  ++i;
	}
	//it is invalid paremeter
      } else {
	cout << "Not valid parameter: " << argc[i] << endl;
	abort();
      } //end else
    } //end if(regex_match(argc[i], reg))
  } //for
  
  //open outputFile
  OpenOutPutFile(output_file_name);


  //precedure section of the program 
  //the beginning of program
   ProgramBegin();
  //deal with mutifiles
  for(auto elem : input_file_name){
  //open inputFile
    OpenFileAndDeal(elem);
    OutPutOrg("\n;;;----Another file", " ");
  }
  //the interrupt of the program 
   ProgramHint();
  
  //close outputFile
  CloseOutPutFile();
  
  return 0;
}
