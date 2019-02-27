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
  
  //precedure section of the program 
  //the beginning of program
  ProgramBegin();

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
      debug_info_object.CreateAInstrDebugRecord("alloc", \
						single_line_debug); 
      TranceAlloca(word_con, "alloc"); break; }
    case load:               TranceLoad(word_con, "load");   break;
    case store:              TranceStore(word_con, "store");  break;
    case add:                TranceAdd(word_con, "add");    break;
    case sub:                TranceSub(word_con, "sub");    break;
    case ret:                TranceRet(word_con, "ret");    break;
    case fcmp:               TranceFcmp(word_con, "fcmp");   break;
    case br:                 TranceBr(word_con, "br");     break;
    case label:              TranceLabel(word_con, "label");  break;
    case globa:              TranceGlobal(word_con, "globa"); break;
    case defin:              TranceDefine(word_con, "defin"); break;
    case call:               TranceCall(word_con, "call");   break;
    case anda:               TranceAnd(word_con, "and");    break;
    case ora:                TranceOr(word_con, "ora");     break;
    case xora:               TranceXor(word_con, "xora");    break;
    case shl:                TranceShl(word_con, "shl");    break;
    case ashr:               TranceAshr(word_con, "ashr");   break;
    case zext:               TranceZext(word_con, "zext");   break;
    case phi:                TrancePhi(word_con, "phi");    break;
    case constant:           TranceConstant(word_con, "constant"); break;
    case type:               TranceType(word_con, "type");  break;
    case getelementptr: {
      
      if(find(word_con.vaCol.begin(), word_con.vaCol.end(), \
	      "store")!= word_con.vaCol.end()){
	TranceStore(word_con, "store");
      }
      else if(find(word_con.vaCol.begin(), word_con.vaCol.end(), \
		   "call") != word_con.vaCol.end()){
	TranceCall(word_con, "call"); 
	
      }
      else if(find(word_con.vaCol.begin(), word_con.vaCol.end(), \
		   "load") != word_con.vaCol.end()){
	TranceLoad(word_con, "load");
      }
      else{
	TranceGetelementptr(word_con, "getelementptr");      
      }
      break;
    }
    case bitcast:            TranceBitcast(word_con, "bitcast"); break;
    default: break;
    }
  }
  ProgramHint(); //the interrupt of the program 
};
