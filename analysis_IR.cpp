//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/*

 */

#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <vector>
#include "trance_fun.h"
#include "debug_info.h"
#include "memory_manage_data.h"
#include "memory_manage_reg.h"
using namespace::std;

class DebugInfo;

//This fun get ram and rom info from cfg file
void GetRamAndRomInfo(string ram_info_file_name, \
		     vector<string>& ram_range_vec, \
		      vector<string>& rom_range_vec,\
		      string &core_name){
  //Open file and judge whether it is open
  ifstream IR_file(ram_info_file_name, ios_base::in);
  if(!IR_file.is_open()){
    cout << "Error: GetRamAndRomInfo() Open file failed!" << endl;
    abort();
  }
  //find core name
  //../../../sdic3004.inc
  /*string core_name_temp = ram_info_file_name;
  auto char_index = ram_info_file_name.rfind('/');
  auto comma_index = ram_info_file_name.rfind('.');
  int len = comma_index - char_index;
  core_name = core_name_temp.substr(char_index, len);
  */
  //get ram info
  string single_line;
  bool whether_have_find_target_core = false;
  bool whether_have_finished = false;
  //[SD3101]
  regex core_name_regex(".+" + core_name + ".+");
  smatch r1;
  //RAMRange=0x001-0x04C
  regex ram_range_regex("RAMRange.+");
  //ROMRange=0x0000-0x3FFF
  regex rom_range_regex("ROMRange.+");
  //[SD3101]
  regex bracket_regex(".+SD.+");//.+]");
  //get every line of this file
  while(getline(IR_file, single_line)){ 
    
    string single_word; 
    istringstream iss(single_line);
    //get every word of this line
    while(iss >> single_word) { 
      //find the target core
      if(regex_match(single_word, r1, bracket_regex)){

	if(whether_have_find_target_core){
	  whether_have_finished = true;
	  break;
	}
	else if(regex_match(single_word, core_name_regex)){
	  whether_have_find_target_core = true;
	  continue;
	}
      }
      
      if(whether_have_find_target_core == false) continue;
     
      //it is processor item
      if(!single_word.compare("Processor")){
	
      }
      //it is range of ram
      if(regex_match(single_word, ram_range_regex)){
	
	auto equal_index = single_word.find("=");
	string ram_range = single_word.substr(equal_index + 1);
	ram_range_vec.push_back(ram_range);
      }
      //it is range of rom
      if(regex_match(single_word, rom_range_regex)){
	
	auto equal_index = single_word.find("=");
	string rom_range = single_word.substr(equal_index + 1);
	
	rom_range_vec.push_back(rom_range);

      }

    }
    if(whether_have_finished) break;
  }
  //if there are not core name in cfg file
  if(!(whether_have_finished && whether_have_find_target_core)){
    cout << "Error: There aren't " << core_name << " in " <<	\
      ram_info_file_name << endl;
    abort();
  }
}

void DealWithRamAndRomInfo(vector<string>& ram_range_vec, \
			   vector<string>& rom_range_vec,\
			   string& core_name){
  RegManage* reg_manage_obj = RegManage::getInstance();
  reg_manage_obj->InitialRamRange(ram_range_vec, core_name);
  
  DataAreaManage* data_area_manage_obj = DataAreaManage::getInstance();
  data_area_manage_obj->InitialRomRange(rom_range_vec, core_name);
}

void DealWithSpecialRegInfo(map<string, string>& special_reg_map, \
			    string& path_name, \
			    string& core_name){
  //Open file and judge whether it is open
  ifstream IR_file(path_name, ios_base::in);
  if(!IR_file.is_open()){
    cout << "Error: DealWithSpecialRegInfo Open special reg "\
      "file failed!" << endl;
    abort();
  }
  string single_line;
  //get every line of this file
  while(getline(IR_file, single_line)){
    string single_word;
    istringstream iss(single_line);
    vector<string> map_vec;
    //get every word of this line
    while(iss >> single_word){
      map_vec.push_back(single_word);
    }
    special_reg_map.insert(make_pair(map_vec[1], map_vec[0]));
  }

}

void OpenFileAndDeal(string &file_name) {
  //Open file
  ifstream IR_file(file_name,ios_base::in);
  if(!IR_file.is_open()){
    cout << "Open file failed!" << endl;
    return;
  }
  
  string single_line;
  while(getline(IR_file, single_line)){//get every line
  
    //deal with the special char
    ChangeComma(single_line);
    //deal debug info and create a record in Instr_degbug_info
    DebugInfo debug_info_object = DebugInfo();
    string single_line_debug = single_line;
    
    string single_word;
    istringstream iss(single_line);
    //    regex reg1("\%\\d＋\b");        //for operator %x
    regex reg1("\%.+");
    //regex reg2("^\@\w＋\b=\bglobal");//for global var @varName ＝ global
    regex gdb_index("!.*");
    SplitWord word_con;
    int word_order = 0;
    //get every word in line
    while(iss >> single_word){       
      if(SDIC_operate_set.find(single_word) != SDIC_operate_set.end()){
	auto index = SDIC_operate_set.find(single_word);	
	word_con.instrName = static_cast<SDICKeyWord>(index->second);
      } 
      else if(regex_match(single_word, reg1)){
	word_con.opCol.push_back(single_word);
      }
      //whether it is a gdb statement
      else if(word_order == 0 && regex_match(single_word, gdb_index)){
	word_con.instrName = static_cast<SDICKeyWord>(27);

      }
      word_con.vaCol.push_back(single_word);
      ++word_order;
    }
    //this line is invalid
    if(word_con.instrName == knull) continue; 

    switch(word_con.instrName){
    case alloc: { 
      debug_info_object.CreateAInstrDebugRecord("alloc",    \
					      single_line_debug); 
      TranceAlloca(word_con, "alloc"); break; }
    case load:  {
      debug_info_object.CreateAInstrDebugRecord("load",	  \
						single_line_debug); 
      TranceLoad(word_con, "load");   break;}
    case store: {
      debug_info_object.CreateAInstrDebugRecord("store",	   \
						single_line_debug); 
      TranceStore(word_con, "store");  break;}
    case add: {
      debug_info_object.CreateAInstrDebugRecord("add",	\
						single_line_debug); 
      TranceAdd(word_con, "add");    break;}
    case sub: {
      debug_info_object.CreateAInstrDebugRecord("sub",	   \
						single_line_debug); 
      TranceSub(word_con, "sub");    break;}
    case ret: {
      debug_info_object.CreateAInstrDebugRecord("ret",	   \
						single_line_debug); 
      TranceRet(word_con, "ret");    break;}
    case fcmp: {
      debug_info_object.CreateAInstrDebugRecord("fcmp",	  \
						single_line_debug); 
      TranceFcmp(word_con, "fcmp");   break;}
    case br:  {
      debug_info_object.CreateAInstrDebugRecord("br",	  \
						single_line_debug); 
      TranceBr(word_con, "br");     break;}
    case label: {
      debug_info_object.CreateAInstrDebugRecord("label",	   \
						single_line_debug); 
      TranceLabel(word_con, "label");  break;}
    case globa: {
      debug_info_object.CreateAInstrDebugRecord("global",	  \
						single_line_debug); 
      TranceGlobal(word_con, "global"); break;}
    case defin: {
      debug_info_object.CreateAInstrDebugRecord("define",	   \
						single_line_debug); 
      TranceDefine(word_con, "define"); break;}
    case call:  {
      debug_info_object.CreateAInstrDebugRecord("call",	   \
						single_line_debug); 
      TranceCall(word_con, "call");   break;}
    case anda: {
      debug_info_object.CreateAInstrDebugRecord("and",	   \
						single_line_debug); 
      TranceAnd(word_con, "and");    break;}
    case ora:  {
      debug_info_object.CreateAInstrDebugRecord("or",	  \
						single_line_debug); 
      TranceOr(word_con, "or");     break;}
    case xora: {
      debug_info_object.CreateAInstrDebugRecord("xor",	  \
						single_line_debug); 
      TranceXor(word_con, "xor");    break;}
    case shl:  {
      debug_info_object.CreateAInstrDebugRecord("shl",	    \
						single_line_debug); 
      TranceShl(word_con, "shl");    break;}
    case ashr: {
      debug_info_object.CreateAInstrDebugRecord("ashr",	   \
						single_line_debug); 
      TranceAshr(word_con, "ashr");   break;}
    case zext: {
      debug_info_object.CreateAInstrDebugRecord("zext",	   \
						single_line_debug); 
      TranceZext(word_con, "zext");   break;}
    case phi:  {
      debug_info_object.CreateAInstrDebugRecord("phi",	   \
						single_line_debug); 
      TrancePhi(word_con, "phi");    break;}
    case constant: {
      debug_info_object.CreateAInstrDebugRecord("constant",	    \
						single_line_debug); 
      TranceConstant(word_con,"constant");break;}
    case type: {
      debug_info_object.CreateAInstrDebugRecord("type",	   \
						single_line_debug); 
      TranceType(word_con, "type");  break;}
    case getelementptr: {
      if(find(word_con.vaCol.begin(), word_con.vaCol.end(), \
	      "store")!= word_con.vaCol.end()){
	
	debug_info_object.CreateAInstrDebugRecord("store",	   \
						  single_line_debug); 
	TranceStore(word_con, "store");
      }
      else if(find(word_con.vaCol.begin(), word_con.vaCol.end(), \
		   "call") != word_con.vaCol.end()){
	debug_info_object.CreateAInstrDebugRecord("call",	     \
						  single_line_debug); 
	TranceCall(word_con, "call"); 
	
      }
      else if(find(word_con.vaCol.begin(), word_con.vaCol.end(), \
		   "load") != word_con.vaCol.end()){
	debug_info_object.CreateAInstrDebugRecord("load",	   \
						  single_line_debug); 
	TranceLoad(word_con, "load");
      }
      else{
	debug_info_object.CreateAInstrDebugRecord("getelementptr", \
						  single_line_debug); 
	TranceGetelementptr(word_con, "getelementptr");      
	
      }
      break;
    }
    case bitcast: {
     
      if(find(word_con.vaCol.begin(), word_con.vaCol.end(), \
	      "call") != word_con.vaCol.end()){
	debug_info_object.CreateAInstrDebugRecord("call",	 \
						single_line_debug); 
	TranceCall(word_con, "call");
      }
      else {
	debug_info_object.CreateAInstrDebugRecord("bitcast",	  \
						  single_line_debug); 
	TranceBitcast(word_con, "bitcast");
      }
      break;
    }
    case sext: {
      debug_info_object.CreateAInstrDebugRecord("sext", \
						single_line_debug);
      TranceSext(word_con, "sext");  break;}
    case trunct: { 

      debug_info_object.CreateAInstrDebugRecord("trunc", \
						single_line_debug);
      TranceTrunc(word_con, "trunc"); break; }
    case selectt: {
      debug_info_object.CreateAInstrDebugRecord("select", \
						single_line_debug);
      TranceSelect(word_con, "select"); break;}
    case gdb: {
      TranceGdb(word_con, "gdb"); break;
      
    }
    case switch_begin: {
      debug_info_object.CreateAInstrDebugRecord("switch", \
						single_line_debug);
      TranceSwitch(word_con, "switch"); break;
    }
    case switch_label: {
      if(find(word_con.vaCol.begin(), word_con.vaCol.end(),	\
	      "switch") != word_con.vaCol.end()){
	debug_info_object.CreateAInstrDebugRecord("switch",	   \
						  single_line_debug);
	TranceSwitch(word_con, "switch"); break;
      }
      else if(find(word_con.vaCol.begin(), word_con.vaCol.end(),
		   "br") != word_con.vaCol.end()){
	debug_info_object.CreateAInstrDebugRecord("br", \
						  single_line_debug);
	TranceBr(word_con, "br"); break;
      }
      else {
	debug_info_object.CreateAInstrDebugRecord("switch",	   \
						  single_line_debug);
	TranceLabelSwitch(word_con, "switch"); break;
      }
    }
    case switch_end: {
      debug_info_object.CreateAInstrDebugRecord("switch", \
						single_line_debug);  
      TranceSwitchEnd(word_con, "switch"); break;
    }
    default: break;
    }

    //    debug_info_object.CreateAInstrDebugRecord(case_name,	   \
    //					      single_line_debug); 
  }
 
};
