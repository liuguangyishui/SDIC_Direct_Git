//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* This file contains information about debug.
 * They are two type debug infomation, instr trancefer info and 
 * var addr infomation. 
 */

#include <fstream>
#include <vector>
#include "debug_info.h"

using namespace::std;

vector<AddrDebugInfoElem> DebugInfo::addr_debug_info_vec;
vector<InstrDebugInfoElem> DebugInfo::instr_debug_info_vec;

void DebugInfo::CreateAAddrDebugRecord(string var_name){
  AddrDebugInfoElem debug_info;
  debug_info.IR_name = var_name;
  
  addr_debug_info_vec.push_back(debug_info);
}

void DebugInfo::AddAddrDebugInfoToRecord(string var_name, \
					 string addr_name){
  if(!addr_debug_info_vec.empty()){
    AddrDebugInfoElem core_info = addr_debug_info_vec.back();
    if(!var_name.compare(core_info.IR_name)){
      core_info.allocated_addr.push_back(addr_name);
    }
  }
}

void DebugInfo::CreateAInstrDebugRecord(string instr_name, string instr){
  InstrDebugInfoElem core_info;
  core_info.Instr_name = instr_name;
  core_info.after_trance_instr_vector.push_back(instr);
  instr_debug_info_vec.push_back(core_info);
}

void DebugInfo::AddInstrDebugInfoToRecord(string instr_name, string trance_instr) {
  if(!instr_debug_info_vec.empty()){
    InstrDebugInfoElem core_info = instr_debug_info_vec.back();
    if(!instr_name.compare(core_info.Instr_name)){
      core_info.after_trance_instr_vector.push_back(trance_instr);
    }
  }
}

void DebugInfo::PrintInstrDebugInfo(){
  string file_name("debug_info_instr.txt");
  std::ofstream fout;
  fout.open(file_name.c_str(), ios_base::out);
  if(!fout.is_open()) {
    cout << "this file open failed!" << endl;
    return ;
  }

  for(auto outer_elem : instr_debug_info_vec){
    fout << outer_elem.Instr_name << ":" << endl;
    vector<string>  core_info = outer_elem.after_trance_instr_vector;
    for(auto inner_elem: core_info){
      fout << "\t\t" << inner_elem << endl;
    }
    fout << endl;
  }
}

void DebugInfo::PrintAddrDebugInfo(){
  char file_name[] = "addr_info_instr.txt";
  std::ofstream fout;
  fout.open(file_name, ios_base::out);
  if(!fout.is_open()){
    cout << "this flie open failed!" << endl;
    return ;
  }
  for(auto outer_elem: addr_debug_info_vec){
    fout << outer_elem.IR_name << ":" << endl;
    vector<string> core_info = outer_elem.allocated_addr;
    for(auto inner_elem: core_info){
      fout << "\t\t" << inner_elem << endl;
    }
    fout << endl;
  }
}
