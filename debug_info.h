//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* This file contains information about debug.
 */

#include <unordered_map>
#include <vector>
#include <iostream>

#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

using namespace::std;

/* DebugInfoStruct is struct of recording the debug infomation.
 */
struct AddrDebugInfoElem {
  string IR_name;
  string var_type;
  vector<string> allocated_addr;
  int number_addr;
};

struct InstrDebugInfoElem {
  string Instr_name;
  vector<string> after_trance_instr_vector;
  int number_instr;
};

//for debug info. Link the trance instr and the c code
struct CCodeLinkInstr {
  string file_name;
  string fun_name;
  unordered_map<string, string> ccode_and_instr_elem_map;
};

class DebugInfo {
 public:
  //for debug info of ccode and instr
  static string ccode_instr_file_fun_name;
 
  //Creat a addr debug info record in addr debug map
  void CreateAAddrDebugRecord(string, string);
  //add addr info into exited record via var_name(first parameter)
  void AddAddrDebugInfoToRecord(string, string);
  void AddAdditionalDebugInfoToRecord(string, string);
  //Create a instr debug info record in instr debug map
  void CreateAInstrDebugRecord(string, string);
  //add info into exited record
  void AddInstrDebugInfoToRecord(string, string);

  void CreateCodeLink(string);

  string GetInfoFromCodeLink(string);

  void AddInfoToCodeLink(string, string, string);

  void PrintAddrDebugInfo(string);
  
  void PrintInstrDebugInfo(string);
  
 private:

  /* When we allocated addr for a var, we should record the map info
 * into the addr_debug_info.
 */
  static vector<AddrDebugInfoElem> addr_debug_info_vec;

  static vector<InstrDebugInfoElem> instr_debug_info_vec;

  static unordered_map<string, CCodeLinkInstr> ccode_and_instr_map;
};

#endif

