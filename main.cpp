//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/*main fun. All begin of the program.
 */

#include <iostream>
#include <map>
#include <cstring>
#include <assert.h>
#include <regex>
#include "analysis_IR.h"
#include "out_put_fun.h"
#include "trance_fun.h"
#include "debug_info.h"


using namespace::std;

int main(int argv, char **argc){

  vector<string> input_file_name;
  //string input_file_name;
  //ram info file name
  //core name
  //special reg name
  string output_file_name, ram_info_file_name,\
    core_name, special_reg_name;

  //identifiy '-' in -F -f or -o 
  regex reg("-.+"); 
  //deal with the para
  for(int i = 0; i < argv; i++){
    //whether is valid para
    if(regex_match(argc[i], reg)){
      //it is about input file
      if(strcmp(argc[i],"-f") == 0 || \
	 strcmp(argc[i],"-F") == 0){
	
	for(int j = i+1; j < argv; j++){
	  if(regex_match(argc[j], reg))
	    break;

	  input_file_name.push_back(argc[j]);
	  ++i;
	}
      }
      //it is about outout file
      else if(strcmp(argc[i], "-o") == 0 || \
	      strcmp(argc[i], "-O") == 0){
	
	for(int j = i+1; j < argv; j++){
	  if(regex_match(argc[j], reg))
	    break;
	  
	  output_file_name = argc[j];
	  ++i;
	}
      } 
      //it is ram addr file
      else if(strcmp(argc[i], "-i") == 0 || \
	      strcmp(argc[i], "-I") == 0){
	for(int j = i + 1; j < argv; j++){
	  if(regex_match(argc[j], reg)) break;
	  
	  ram_info_file_name = argc[j];
	  ++i;
	}
      }
      //it is core name
      else if(strcmp(argc[i], "-c") == 0 || \
	      strcmp(argc[i], "-C") == 0){
	  core_name = argc[++i];
     
      }
      //it is for special reg
      else if(strcmp(argc[i], "-s") == 0 || \
	      strcmp(argc[i], "-S") == 0){
	special_reg_name = argc[++i];
      }
      //it is invalid paremeter
      else {
	cout << "Not valid parameter: " << argc[i] << endl;
	abort();
      } //end else
    } //end if(regex_match(argc[i], reg))
  } //for
  
  //judge whether the command have enough parameter
  if(input_file_name.size() == 0 ||		\
     ram_info_file_name.empty() ||		\
     core_name.empty() ||			\
     special_reg_name.empty()){
    cout << "Error: main() Not enough command parameter! " << endl;
    abort();
  }
 

  vector<string> ram_range_vec, rom_range_vec;
  //deal with ram file
  GetRamAndRomInfo(ram_info_file_name, \
		   ram_range_vec, rom_range_vec,\
		   core_name);
  //deal with ram info include ram and rom 
  DealWithRamAndRomInfo(ram_range_vec,   \
			rom_range_vec, \
			core_name);

  //deal with special register in ram
  DealWithSpecialRegInfo(special_reg_vec,	\
  			 special_reg_name,	\
  			 core_name);

  //open outputFile
  OpenOutPutFile(output_file_name);
  
  //get the parent file path of ram_info_file_name
  //if the run environment is linux
#if defined(__linux__)
  int path_name_index = ram_info_file_name.rfind("/");
  //if the run environment is windos
#elif defined(_WIN32)
  int path_name_index = ram_info_file_name.rfind("\\");
#endif
string path_name = ram_info_file_name.substr(0, path_name_index + 1);
  //include some head file, core_name GeneralReg
  OutPutPure("#include \"" + core_name + ".INC.inc\"");
  OutPutPure("#include \"" + core_name + "_GeneralReg.inc\"");
  OutPutPure("#include \"" + path_name + "SubProgram.inc\"");
  OutPutPure("#include \"" + path_name + "SubProgram.asm\"");
  //precedure section of the program 
  //the beginning of program
  ProgramBegin();
  //deal with mutifiles
  for(int i = 0; i < input_file_name.size(); i++){
    //open inputFile
    OpenFileAndDeal(input_file_name[i]);
    if(i != input_file_name.size() - 1){
      OutPutOrg("\n;;;    Another file", " ");
    }
  }
  //the interrupt of the program 
   ProgramHint();
   //debug info file name
   string debug_file_name = output_file_name;
   
   DebugInfo debug_info_obj = DebugInfo();
   //if output file name is NULL
   if(debug_file_name.empty()){
     debug_file_name = "output";
   }
   //if a . carater in the debug_file_name, remove
   string::size_type idex;
   idex = debug_file_name.find(".");
   if(idex != string::npos){
     debug_file_name = debug_file_name.substr(0, idex);
   }
   //output the addr debug info to debug_file_name
   debug_info_obj.PrintAddrDebugInfo(debug_file_name + \
				     "_addr_debug_info.txt");
   //output the instr debug info to debug_file_name
   debug_info_obj.PrintInstrDebugInfo(debug_file_name + \
				      "_instr_debug_info.txt");
   
  //close outputFile
  CloseOutPutFile();
  
  return 0;
}
