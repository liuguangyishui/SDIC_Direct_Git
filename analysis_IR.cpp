//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/*

 */

#include <iostream>
#include <fstream>
#include <regex>
#include "trance_fun.h"
#include "debug_info.h"

using namespace::std;

class DebugInfo;

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
    SplitWord word_con;
    //get every word in line
    while(iss >> single_word){       
      if(SDIC_operate_set.find(single_word) != SDIC_operate_set.end()){
	auto index = SDIC_operate_set.find(single_word);	
	word_con.instrName = static_cast<SDICKeyWord>(index->second);
      } else if(regex_match(single_word, reg1)){
	word_con.opCol.push_back(single_word);
      } 
      word_con.vaCol.push_back(single_word);
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
	TranceBitcast(word_con, "bitcast"); break;}
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
    default: break;
    }

    //    debug_info_object.CreateAInstrDebugRecord(case_name,	   \
    //					      single_line_debug); 
  }
 
};
