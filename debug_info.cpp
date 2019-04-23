//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* This file contains information about debug.
 * They are two type debug infomation, instr trancefer info and 
 * var addr infomation. 
 */

#include <fstream>
#include <vector>
#include <regex>
#include "debug_info.h"

using namespace::std;

vector<AddrDebugInfoElem> DebugInfo::addr_debug_info_vec;
vector<InstrDebugInfoElem> DebugInfo::instr_debug_info_vec;
unordered_map<string, CCodeLinkInstr> DebugInfo::ccode_and_instr_map;
string DebugInfo::ccode_instr_file_fun_name;

void DebugInfo::CreateAAddrDebugRecord(string var_name, string type){
  AddrDebugInfoElem debug_info;
  debug_info.IR_name = var_name;
  debug_info.var_type = type;

  addr_debug_info_vec.push_back(debug_info);

}

void DebugInfo::AddAddrDebugInfoToRecord(string var_name, \
					 string addr_name){
  if(!addr_debug_info_vec.empty()){
    AddrDebugInfoElem &core_info = addr_debug_info_vec.back();
    if(!var_name.compare(core_info.IR_name)){
      core_info.allocated_addr.push_back(addr_name);
    }
  }
}

void DebugInfo::AddAdditionalDebugInfoToRecord(string var_name, \
					       string additional_info){
 
  for(auto &elem : addr_debug_info_vec){
    if(!var_name.compare(elem.IR_name)){
      elem.allocated_addr.insert(elem.allocated_addr.begin(), \
				 additional_info);
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
    InstrDebugInfoElem &core_info = instr_debug_info_vec.back();
    if(!instr_name.compare(core_info.Instr_name)){
      core_info.after_trance_instr_vector.push_back(trance_instr);
    }
  }

}

void DebugInfo::CreateCodeLink(string file_fun_name){
  if(file_fun_name.empty() ||
     ccode_and_instr_map.find(file_fun_name) != 
     ccode_and_instr_map.end()){
    cout << "DebugInfo::CreateCodeLink: file_fun_name fail" << endl;
    return ;
  }
  CCodeLinkInstr code_link;
  ccode_and_instr_map.insert(make_pair(file_fun_name, code_link)); 
}

void 
DebugInfo::AddInfoToCodeLink(string file_fun_name, 
			     string gdb_first_name,
			     string gdb_second_name){
  if(file_fun_name.empty() ||
     ccode_and_instr_map.find(file_fun_name) == \
     ccode_and_instr_map.end()){
    // cout << "DebugInfo::AddInfoToCodeLink: file_fun_name fail" << endl;
    return ;
  }
  
  CCodeLinkInstr &elem=ccode_and_instr_map.find(file_fun_name)->second;
  elem.ccode_and_instr_elem_map.insert(make_pair(gdb_first_name, gdb_second_name));
  
}

string DebugInfo::GetInfoFromCodeLink(string instr_name){
  
  string instr_index;
  string temp = instr_name;
  
  auto index_1 = temp.rfind('!');
  int index_2 = temp.rfind(" ");
  
  instr_index = temp.substr(index_1, index_2 - index_1);
  
  string file_name = DebugInfo::ccode_instr_file_fun_name;

  CCodeLinkInstr elem;
  if(!file_name.empty() && 
     ccode_and_instr_map.find(file_name)!= ccode_and_instr_map.end()){ 
     elem = ccode_and_instr_map.find(file_name)->second;;
  }
  //There may be a error in this, since not verity whether the elem is 
  //valid or not
  string ccode_line;
  if(elem.ccode_and_instr_elem_map.find(instr_index) != \
     elem.ccode_and_instr_elem_map.end()){
    ccode_line =						\
      elem.ccode_and_instr_elem_map.find(instr_index)->second;
  }
  return ccode_line;
}

void DebugInfo::PrintInstrDebugInfo(string output_file_name){
  string file_name(output_file_name);
  std::ofstream fout;
  fout.open(file_name.c_str(), ios_base::out);
  if(!fout.is_open()) {
    cout << "this file open failed!" << endl;
    return ;
  }
 
  for(auto outer_elem : instr_debug_info_vec){
    fout << "!Single Statement------------------------------"<< endl;
    fout << "!IR INSTR: " << endl;;
    vector<string>  core_info = outer_elem.after_trance_instr_vector;
    for(int i = 0; i < core_info.size(); i++){
      if(i == 0){
	fout << core_info[i] << endl;

	string instr_name = core_info[i];
	regex gdb(".*!dbg.*");
	if(regex_match(instr_name, gdb)){
	  string ccode_line = this->GetInfoFromCodeLink(instr_name);
	  if(!ccode_line.empty())
	    fout << "!C CODE : " << ccode_line << "  LINE" << endl;;
	}
	
	fout << "!ASSEMBLY INSTR: " << endl;
      }
      else {
	fout << "\t\t" << core_info[i] << endl;
      }
    }
    fout << endl;
  }
}

void DebugInfo::PrintAddrDebugInfo(string output_file_name){
  //  char file_name[] = output_file_name;
  string file_name(output_file_name);
  std::ofstream fout;
  fout.open(file_name.c_str(), ios_base::out);
  if(!fout.is_open()){
    cout << "this flie open failed!" << endl;
    return ;
  }
  for(auto outer_elem: addr_debug_info_vec){
    fout << outer_elem.IR_name << ": " << endl;
    fout << "type: " << outer_elem.var_type << endl;
    vector<string> core_info = outer_elem.allocated_addr;
    for(auto inner_elem: core_info){
      fout << "\t\t" << inner_elem << endl;
    }
    fout << endl;
  }
}
