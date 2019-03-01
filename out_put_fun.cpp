//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* this file maily deal with the infomation output
 */

#include <iostream>
#include <fstream>
#include <cassert>
#include "out_put_fun.h"
#include "debug_info.h"

using namespace::std;

class DebugInfo;

std::ofstream f_out;


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

void OutPut(string instr_name, string op, string IR_name){
  string content_1, content_2;
  if(instr_name_no_para.find(instr_name) !=	\
     instr_name_no_para.end()){
    content_1 = "\t" +  instr_name + "\t\t" + op; 
    content_2 = "\t" +  instr_name + "\t\t" + op;
  }
  else if(instr_name_one_para.find(instr_name) != \
	  instr_name_one_para.end()){
    content_1 = "\t" +  instr_name + "\t\t" + op + ",\t1"; 
    content_2 = "\t" +  instr_name + "\t\t" + op + ",\t1";
  }
  else if(instr_name_two_para.find(instr_name) != \
	  instr_name_two_para.end()){
    content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t1"; 
    content_2 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t1";
  
  } else {
    content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t1"; 
    content_2 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t1";
  }
  
  DebugInfo debug_info_object = DebugInfo();

  if(!out_put_file_name.empty() && f_out.is_open()){
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1);
    f_out << content_1 << endl;
  } else {
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_2);
    cout << content_2 << endl;
  }
}

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

