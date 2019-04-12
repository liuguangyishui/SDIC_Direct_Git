//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* this file maily deal with the infomation output
 */

#include <iostream>
#include <fstream>
#include <cassert>
#include <regex>
#include "out_put_fun.h"
#include "debug_info.h"

using namespace::std;

class DebugInfo;

std::ofstream f_out;

map<string, string> special_reg_vec;

void OpenOutPutFile(string file_name){
  //out_put_file_name is a global var, be used in other function
  out_put_file_name = file_name;
  if(!out_put_file_name.empty()){
    f_out.open(out_put_file_name, ios_base::out);
    if(!f_out.is_open())
      cout << "this file open failed" << endl;
  }
}

void CloseOutPutFile(){
  if(f_out.is_open())
    f_out.close();
}

//output the instr.But we have some question have to solve:
//identify the special reg and general reg
//deal with the parameter of D and A
//output to terminate of  file
//record the instr debug
//deal with the banks 
string which_bank_index;
void OutPut(string instr_name, string op, string IR_name){
 
  string content_1, content_2;
  //record the bank index
  string origin_op;
  //whether the op is num
  bool isnum_index = false;

  //identify special reg and general reg
  //general reg
  regex general_reg_regex("0x.+");
  //special reg
  regex special_reg_regex("0s.+");
  if(regex_match(op, general_reg_regex)){
    //register name no 0x
    origin_op = op.substr(2);    
    op = "REG" + op.substr(2);
  }
  //the register is special reg. we should use it name.
  else if(regex_match(op, special_reg_regex)){
    //register name no 0x
    origin_op = op.substr(2);
    //PCL EQU 0XXXH (XXX)
    string special_reg = op.substr(2);
    if(special_reg_vec.find(special_reg) != special_reg_vec.end()){
      auto result = special_reg_vec.find(special_reg);
      op = result->second;
    }
    //NO special reg in map
    else {
      op = "ERROR";
    }
  }
  else {
    isnum_index = true;
  }

  //deal with different instr have different parameter
  if(instr_name_no_para.find(instr_name) !=	\
     instr_name_no_para.end()){
    content_1 = "\t" +  instr_name + "\t\t" + op; 
  }
  else if(instr_name_one_para.find(instr_name) != \
	  instr_name_one_para.end()){
    content_1 = "\t" +  instr_name + "\t\t" + op + ",\t1"; 
  }
  else if(instr_name_two_para.find(instr_name) != \
	  instr_name_two_para.end()){
    content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t1"; 
  
  } else {
    content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t1"; 
  }
  
  //record the instr debug info 
  //and decide where the instr output
  DebugInfo debug_info_object = DebugInfo();
  
  if(!out_put_file_name.empty() && f_out.is_open()){
    
    //deal with bank
    string which_bank_index_temp = origin_op.substr(0,1);
    if(isnum_index){
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
      f_out << content_1 << endl;
    }
    else if(!isnum_index && which_bank_index.empty()){
      which_bank_index = which_bank_index_temp;
      string content_bank = "\tmovlb\t\t" + which_bank_index; 
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_bank);
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
	
      f_out << content_bank << endl;
      f_out << content_1 << endl;

    }
    else if(!isnum_index && !which_bank_index.empty()){
      if(!which_bank_index.compare(which_bank_index_temp)){
	debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
	f_out << content_1 << endl;
      } 
      else {
	which_bank_index = which_bank_index_temp;
	string content_bank = "\tmovlb\t\t" + which_bank_index; 
	debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_bank);
	debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
	
	f_out << content_bank << endl;
	f_out << content_1 << endl;
	
      }
    }
  
  } 
  else {

    //deal with bank
    string which_bank_index_temp = origin_op.substr(0,1);

    if(isnum_index){
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
      cout << content_1 << endl;
    }
    else if(!isnum_index && which_bank_index.empty()){
      which_bank_index = which_bank_index_temp;
      string content_bank = "\tmovlb\t\t" + which_bank_index; 
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_bank);
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
	
      cout << content_bank << endl;
      cout << content_1 << endl;

    }
    else if(!isnum_index && !which_bank_index.empty()){
      if(!which_bank_index.compare(which_bank_index_temp)){
	debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
	cout << content_1 << endl;
      } 
      else {
	which_bank_index = which_bank_index_temp;
	string content_bank = "\tmovlb\t\t" + which_bank_index; 
	debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_bank);
	debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
	
	cout << content_bank << endl;
	cout << content_1 << endl;
	
      }
    }
    
  }
}

//output instr that the d and a parameter have been known
void OutPut(string instr_name, string op,int index1 = 0, \
	    int index2 = 1, string IR_name = " "){
  string content_1 = "\t" + instr_name + "\t\t" + op + ",\t"	\
    + to_string(index1) + ",\t" + to_string(index2);
  string content_2 = "\t" + instr_name + "\t\t" + op + ",\t"	\
    + to_string(index1) + ",\t" + to_string(index2);

  DebugInfo debug_info_object = DebugInfo();
  
  if(!out_put_file_name.empty() && f_out.is_open()){
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
    f_out << content_1 << endl;
  } else {
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_2);
    cout << content_2 << endl;
  }
}

void OutPutOrg(string instrName, string op){
  if(!out_put_file_name.empty() && f_out.is_open()){
    f_out << "\t" << instrName << "\t\t" << op << "\t" << endl;
  } else {
    cout << "\t" << instrName << "\t\t" << op << "\t" << endl;
  }
}

void OutPutJump(string instr_name, string op, string IR_name){
  string content_1 = "\t" + instr_name + "\t\t" + "Label" + "_" + op;
  string content_2 = "\t" + instr_name + "\t\t" + "Label" + "_" + op;

  DebugInfo debug_info_object = DebugInfo();

  if(!out_put_file_name.empty() && f_out.is_open()){
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
    f_out << content_1 << endl;
  } else {
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_2);
    cout << content_2 << endl;
  }
}

void OutPutLabel(string orderName, string IR_name){
  if(!out_put_file_name.empty() && f_out.is_open()){
    f_out << "\n" << flush;
    f_out << "Label" << "_" << orderName << ":" << endl;
  } else {
    cout << "\n" << flush;
    cout << "Label" << "_" << orderName << ":" << endl;
  }
}


void OutPutPure(string instrName){
  if(!out_put_file_name.empty() && f_out.is_open()){
    f_out << "\n" << flush;
    f_out << instrName << endl;
  } else {
    cout << "\n" << flush;
    cout << instrName  << endl;
  }

}
