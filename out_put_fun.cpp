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
//yzk 2019.4.8 record line numbers
int trance_line;

void OpenOutPutFile(string file_name){
  //out_put_file_name is a global var, be used in other function
  out_put_file_name = file_name;
  if(!out_put_file_name.empty()){
    f_out.open(out_put_file_name, ios_base::out);
    if(!f_out.is_open())
      throw "this file open failed";
	trance_line = 0;//yzk 2019.4.8 line numbers initialize
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
  bool is_num_index = false;
  //whether the reg is special,so the A parameter is different
  bool is_special_instr = false;
  //identify special reg and general reg
  //general reg
  regex general_reg_regex("0x.+");
  //special reg
  regex special_reg_regex("0s.+");
  //0x num
  regex addr_regex("0a.+");
  regex mul_div_regex("0k.+");
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
      is_special_instr = true;
    }
    //NO special reg in map
    else {
      is_special_instr = true;
      op = "ERROR: OutPut() Not this special reg!";
    }
  }
  else if(regex_match(op, addr_regex)){
    origin_op = op.substr(2);
    op =  origin_op;
    is_num_index = true;

  }
  else if(regex_match(op, mul_div_regex)){
    //register name no 0k
    //ex: 0k0Divdend
    origin_op = op.substr(2);
    op = op.substr(3);
  }
  else {
    is_num_index = true;
  }

  //deal with different instr have different parameter
  if(instr_name_no_para.find(instr_name) !=	\
     instr_name_no_para.end()){
    content_1 = "\t" +  instr_name + "\t\t" + op; 
  }
  else if(instr_name_one_para.find(instr_name) != \
	  instr_name_one_para.end()){
    if(is_special_instr) 
      content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0";
    else
      content_1 = "\t" +  instr_name + "\t\t" + op + ",\t1"; 
  }
  else if(instr_name_two_para.find(instr_name) != \
	  instr_name_two_para.end()){
    if(is_special_instr)
      content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t0"; 
    else
      content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t1"; 
  
  } 
  else {
    if(is_special_instr)
      content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t0";
    else
      content_1 = "\t" +  instr_name + "\t\t" + op + ",\t0,\t1"; 
  }
  
  trance_line++;//yzk
				//record the instr debug info 
				//and decide where the instr output
  //record the instr debug info 
  //and decide where the instr output
  DebugInfo debug_info_object = DebugInfo();

  //out put the instr into a file
  if(!out_put_file_name.empty() && f_out.is_open()){
    
    //deal with bank
    string which_bank_index_temp = origin_op.substr(0,1);
    //the op is num, so we direct output the instr, not to deal with
    //the reg and bank
    if(is_num_index){
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1 + \
				      "\tline:" + to_string(trance_line));
      f_out << content_1 << endl;
    }
    //the op is't num and it is the first instr, so we use the default
    //bank num 0x00
    else if(!is_num_index && which_bank_index.empty()){
      //this instr is't specail instr. IF the instr is special instr
      //then the bank is valid, so we don't deal with it 
      if(!is_special_instr) {
	which_bank_index = which_bank_index_temp;
	string content_bank = "\tmovlb\t\t0x0" + which_bank_index; 
	debug_info_object.AddInstrDebugInfoToRecord(IR_name,   	\
		      content_bank + "\tline:" + to_string(trance_line));
	f_out << content_bank << endl;
      }      
	  trance_line++;
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1 + \
			   "\tline:" + to_string(trance_line));
      f_out << content_1 << endl;
    }
    //the op is't num and it is not first instr, so we must deal with
    //the bank
    else if(!is_num_index && !which_bank_index.empty()){
      //if the bank of current instr is same with the bank of 
      //last instr. so we don't change it
      if(!which_bank_index.compare(which_bank_index_temp)){
	debug_info_object.AddInstrDebugInfoToRecord(IR_name,\
			 content_1 + "\tline:" + to_string(trance_line));
	f_out << content_1 << endl;
      } 
      else {
	if(!is_special_instr){
	  which_bank_index = which_bank_index_temp;
	  string content_bank = "\tmovlb\t\t0x0" + which_bank_index; 
	  debug_info_object.AddInstrDebugInfoToRecord(IR_name,	    \
			content_bank + "\tline:" + to_string(trance_line));
	  f_out << content_bank << endl;  
	  trance_line++;
	}
	debug_info_object.AddInstrDebugInfoToRecord(IR_name,\
			content_1 + "\tline:" + to_string(trance_line));
	f_out << content_1 << endl;
	
      }
    }
  
  } 
  //out put into terminate
  else {

    //deal with bank
    string which_bank_index_temp = origin_op.substr(0,1);

    if(is_num_index){
      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1 + "\tline:" + to_string(trance_line));
      cout << content_1 << endl;
    }
    else if(!is_num_index && which_bank_index.empty()){

      if(!is_special_instr){
	which_bank_index = which_bank_index_temp;
	string content_bank = "\tmovlb\t\t0x0" + which_bank_index; 
	debug_info_object.AddInstrDebugInfoToRecord(IR_name,	      \
						  content_bank + "\tline:" + to_string(trance_line));
	cout << content_bank << endl;
	trance_line++;
      }

      debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1 + "\tline:" + to_string(trance_line));
      cout << content_1 << endl;
    }
    else if(!is_num_index && !which_bank_index.empty()){
      if(!which_bank_index.compare(which_bank_index_temp)){
	debug_info_object.AddInstrDebugInfoToRecord(IR_name,\
						    content_1 + "\tline:" + to_string(trance_line));
	cout << content_1 << endl;
      } 
      else {
	if(!is_special_instr){
	  which_bank_index = which_bank_index_temp;
	  string content_bank = "\tmovlb\t\t0x0" + which_bank_index; 
	  debug_info_object.AddInstrDebugInfoToRecord(IR_name,	   \
						      content_bank + "\tline:" + to_string(trance_line));
	  cout << content_bank << endl;
	  trance_line++;
	}
	
	debug_info_object.AddInstrDebugInfoToRecord(IR_name,	   \
						    content_1 + "\tline:" + to_string(trance_line));
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
  trance_line++;
  DebugInfo debug_info_object = DebugInfo();
  
  if(!out_put_file_name.empty() && f_out.is_open()){
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1 + "\tline:" + to_string(trance_line));
    f_out << content_1 << endl;
  } else {
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_2 + "\tline:" + to_string(trance_line));
    cout << content_2 << endl;
  }
}

void OutPutOrg(string instrName, string op){
  if(!out_put_file_name.empty() && f_out.is_open()){
    f_out << "\t" << instrName << "\t\t" << op << "\t" << endl;
  } else {
    cout << "\t" << instrName << "\t\t" << op << "\t" << endl;
  }
  trance_line++;
}

void OutPutJump(string instr_name, string op, string IR_name){
  string content_1 = "\t" + instr_name + "\t\t" + "Label" + "_" + op;
  string content_2 = "\t" + instr_name + "\t\t" + "Label" + "_" + op;
  if (instr_name == "call") {//yzk 2019/6/5 修改call指令使参数为1从而使用影子寄存器
	  content_1 += ",1";
	  content_2 += ",1";
  }
  trance_line++;
  DebugInfo debug_info_object = DebugInfo();

  if(!out_put_file_name.empty() && f_out.is_open()){
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_1 + "\tline:" + to_string(trance_line));
    f_out << content_1 << endl;
  } else {
    debug_info_object.AddInstrDebugInfoToRecord(IR_name, content_2 + "\tline:" + to_string(trance_line));
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
  trance_line += 2;
}

void OutPutPure(string instrName){
  if(!out_put_file_name.empty() && f_out.is_open()){
    f_out << "\n" << flush;
    f_out << instrName << endl;
  } else {
    cout << "\n" << flush;
    cout << instrName  << endl;
  }
  trance_line += 2;
}

void OutPutDirect(string instrName){
  if(!out_put_file_name.empty() && f_out.is_open()){
    f_out << instrName << " ";
  } else {
    cout << instrName  << " ";
  }
}
void OutPutJumpPure(string instrName, string op, string IR_name){
  string content = "\t" + instrName + "\t\t" + op;
  if (instrName == "call") {//yzk 2019/6/5 修改call指令使参数为1从而使用影子寄存器
	  content += ",1";
  }
  DebugInfo debug_info_object = DebugInfo();
  trance_line++;
  debug_info_object.AddInstrDebugInfoToRecord(IR_name, content + "\tline:" + to_string(trance_line));
  if(!out_put_file_name.empty() && f_out.is_open()){
    f_out << content << endl;
  } else {
    cout << content  << endl;
  }
}
