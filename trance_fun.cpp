//Copyright 2019 Hust.
//Author: HYL
//Descriptor:

/*

 */

#include <iostream>
#include <cstring>
#include <regex>
#include <cassert>
#include "tool_fun.h"
#include "trance_fun.h"
#include "out_put_fun.h"
#include "memory_manage_reg.h"
#include "memory_manage_data.h"


using namespace::std;
class RegManage;
class DataManage;

void ChangeComma(string &str){
//  cout << "i have benn call changeComma" << endl;
    for(int i = 0; i < str.size(); i++){
    if(str[i] == ',' || str[i] == ':' ||		\
      str[i] == '(' || str[i] == ')'){
        str[i] = ' ';
   }
  }
}

/*the beignning of program
 */
void ProgramBegin(){
  OutPutOrg("org", "0x0000");
  OutPutJump("bra", "Preprocess", " ");
  //OutPutJump("bra", "begin");
  OutPutOrg("org", "0x0008");
  OutPutJump("bra", "Hint", " ");
  //  OutPutLabel("begin");
  OutPutOrg("org", "0x0020");
  OutPutLabel("Preprocess", " ");
}

//the interrupt of the program
//
void ProgramHint(){
  OutPutLabel("Hint", " ");
  OutPutOrg("retfie", "1");
  OutPutOrg("end", " ");
}

//coreAdd_And
//core fun or logic operatore
void CoreFunOfLogicOperator(SplitWord wordCon, string namewf, \
			    string namelw, string IR_name){
  //Instr: a = R + Constant or reverse
  if(wordCon.opCol.size() <= 2){ 
    string op_des = wordCon.opCol[0];
    string op_src1 = wordCon.vaCol[4]; 
    string op_src2 = wordCon.vaCol[5]; 

    string op_des_type = wordCon.vaCol[3];
    string op_src1_type = wordCon.vaCol[3];
    string op_src2_type = wordCon.vaCol[3];

    RegManage* reg_manage_obj = RegManage::getInstance();
    
    //add fun name before variable
    string current_fun_name = \
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
    }
    //deal with op_des
    reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
    vector<string> op_des_addr =			     	\
      reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
    int reg_num = reg_manage_obj->HowBigType(op_src1_type);
    
    vector<string> value1, value2;
    regex reg(".*\%.*");
    if(regex_match(op_src1, reg)){
      op_src1 = current_fun_name + "." + op_src1;
      value1 = reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
    } else{
      //     value1 = opSrc1;
    }
    //Instr: a = Constant + R
    if(regex_match(op_src2, reg)){  
      if(!current_fun_name.empty()){
	op_src2 = current_fun_name + "." + op_src2;
      }
      value2 = reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);
      value1 = reg_manage_obj->GetSplitSectionOfANum(op_src1, \
						     reg_num);
      if(reg_manage_obj->CheckAllOperatorType(op_src1_type,	\
    					     op_src2_type,	\
					     op_des_type)){
	for(int i = 0; i < reg_num; i++){
	  OutPut("movf", value2[i], IR_name);
	  OutPut(namelw, value1[i], IR_name);
	  OutPut("movwf", op_des_addr[i], IR_name);
	}
      }
      return; 
    } 
    //Instr: a = R + Constant
    else {                      
      //Instr: ~A
      if(!op_src2.compare("-1") && !namelw.compare("xorlw")){
	
	if(reg_manage_obj->CheckAllOperatorType(op_src1_type, \
					      op_des_type)){  
	  for(int i = 0; i < reg_num; i++){
	    OutPut("comf", value1[i], IR_name); //Instr: ~A
	    OutPut("movwf", op_des_addr[i], IR_name);
	  }
	} 
      }
      //Instr: B = !A
      else if(!op_src2.compare("true") &&	\
		!namelw.compare("xorlw")){
	//Instr: B = !A
	vector<string> constant = \
	  reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
	if(reg_manage_obj->CheckAllOperatorType(op_src1_type, \
					       op_des_type)){
	  for(int i = 0; i < reg_num; i++){
	    OutPut("movlw", constant[i], IR_name);
	    //OutPut("movlw", "1");
	    OutPut(namelw, value1[i], IR_name);
	    OutPut("movwf", op_des_addr[i], IR_name);
	  }
	}
	
      }
      else {
	vector<string> constant = \
	  reg_manage_obj->GetSplitSectionOfANum(op_src2, reg_num);
	if(reg_manage_obj->CheckAllOperatorType(op_src1_type, \
					       op_des_type)){
	  for(int i = 0; i < reg_num; i++){
	    OutPut("movf", value1[i], IR_name);
	    OutPut(namelw, constant[i], IR_name);
	    OutPut("movwf", op_des_addr[i], IR_name);
	  }
	}
      }
    }
  } 
  //Instr: a = R + R
  else if(wordCon.opCol.size() >= 3){
    string op_des = wordCon.opCol[0];
    string op_src1 = wordCon.opCol[1];
    string op_src2 = wordCon.opCol[2];

    string op_des_type = wordCon.vaCol[3];
    
    RegManage* reg_manage_obj = RegManage::getInstance();
    
    //add fun name before variable
    string current_fun_name = \
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_src1 = current_fun_name + "." + op_src1;
      op_src2 = current_fun_name + "." + op_src2;
      op_des = current_fun_name + "." + op_des;
    }
    
    vector<string> value1 =				\
      reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
    vector<string> value2 =				\
      reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);
    reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
    vector<string> reg_name =			\
      reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);

    int reg_num = reg_name.size();
    if(value1.size() == value2.size() && \
       value2.size() == reg_name.size()){
      for(int i = 0; i < reg_num; i++){
	
	OutPut("movf", value1[i], IR_name);
	OutPut(namewf, value2[i], IR_name);
	OutPut("movwf", reg_name[i], IR_name);
      } 
    }else {
	cout << "the type is not correspond!" << endl;
    }  
  
  }
}

void TranceLoad(SplitWord wordCon, string IR_name){
#define VEC wordCon.vaCol

  regex regular_expr_struct("%struct.*");
  //get element from array that store in data area and reg 
  if(find(VEC.begin(), VEC.end(), "getelementptr") != VEC.end() && \
     find(VEC.begin(), VEC.end(), "inbounds") != VEC.end() && \
     find(VEC.begin(), VEC.end(), "x") != VEC.end()){ 

    string op_des = wordCon.vaCol[0];
    string op_des_type = wordCon.vaCol[3];
    string array_name = VEC[13];

    //whcih elem in the array that should be taken
    //change the index to dec number
    int elem_index = ChangeStrToDec(wordCon.vaCol[17]);
    
    MemoryManage memory_manage_obj = MemoryManage();   
    //get the index that dedicated the variable store which map
    int which_store_map_index =				\
      memory_manage_obj.VarStoreWhichMap(array_name);
    
    //the array variable store in reg_addr_map
    if(which_store_map_index == 1){
      RegManage* reg_manage_obj = RegManage::getInstance();

      //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	array_name = current_fun_name + "." + array_name;
	op_des = current_fun_name + "." + op_des;
      }
    
      //allocate addr for the op_des
      reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
      //get the actual addr of the op_des
      vector<string> reg_name = \
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
      //get the elem addr 
      vector<string> elem_addr_vec = \
	reg_manage_obj->GetActualAddrFromGenVal(array_name,\
						elem_index);
      
      int reg_num = elem_addr_vec.size();
      for(int i = 0; i < reg_num; i++){
	OutPut("movf", elem_addr_vec[i], IR_name);
	OutPut("movwf", reg_name[i], IR_name);
      }
    }
    //the array variable store in global_addr_map
    else if(which_store_map_index == 2){
      RegManage* reg_manage_obj = RegManage::getInstance();

      //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	op_des = current_fun_name + "." + op_des;
      }
    
      //allocate addr for the op_des
      reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
      //get the actual addr of the op_des
      vector<string> reg_name = \
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
      //get the elem addr 
      vector<string> elem_addr_vec = \
	reg_manage_obj->GetActualAddrFromGlobalVal(array_name, \
						   elem_index);
      
      int reg_num = elem_addr_vec.size();
      
      for(int i = 0; i < reg_num; i++){

	OutPut("movf", elem_addr_vec[i], IR_name);
	OutPut("movwf", reg_name[i], IR_name);
      }
    }
    //the array variable store in data_area_addr_map
    else if(which_store_map_index == 4){
   
      DataAreaManage* data_area_manage_obj = \
	DataAreaManage::getInstance();    
      RegManage* reg_manage_obj = RegManage::getInstance();

        //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	op_des = current_fun_name + "." + op_des;
      }
    
      //aloca acutal register for the op_des
      reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
      //get the actual reg of op_es
      vector<string> reg_name =				\
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
      //get the elem addr in map
      vector<string> elem_addr_vec =				  \
	data_area_manage_obj->GetActualAddrFromVal(array_name,\
						   elem_index);
  
      int reg_num = elem_addr_vec.size();
      for(int i = 0; i < reg_num; i++){
	string addr_h(elem_addr_vec[i].substr(0,2));
	string addr_l(elem_addr_vec[i].substr(2,2));
	OutPut("movlw", addr_h, IR_name);
	OutPut("movwf", "tblptrh", IR_name);
	OutPut("movlw", addr_l, IR_name);
	OutPut("movwf", "tblptrl", IR_name);
	OutPut("tblrd", "*", IR_name);
	OutPut("movwf", "tablat", IR_name);
	
	OutPut("movf", "tablat", IR_name);
	OutPut("movwf", reg_name[i], IR_name);
      }
    }
  } 
  //get element from global struct type  that store in global map
  else if(find(VEC.begin(), VEC.end(), "load") != VEC.end() && \
	  find(VEC.begin(), VEC.end(),"getelementptr")!=VEC.end()&&\
	  regex_match(VEC[7], regular_expr_struct)){
    
    //destination variable
    string op_des = VEC[0];
    //destination variable type
    string op_des_type = VEC[3];
    //struct name
    string struct_name = VEC[9];
    //which elem
    int elem_index = ChangeStrToDec(VEC[13]);

    RegManage * reg_manage_obj = RegManage::getInstance(); 
    MemoryManage memory_manage_obj = MemoryManage();   
    //add fun name before variable
    string current_fun_name =				\
      memory_manage_obj.GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
    }
    //get the index that dedicated the variable store which map
    int which_store_map_index =				\
      memory_manage_obj.VarStoreWhichMap(struct_name);
    //the struct variable store in reg_addr_map
    if(which_store_map_index == 1){
      
    }
    //the struct variable store in global_addr_map 
    else if(which_store_map_index == 2){
      //get the elem_index adddr in struct_name that store in 
      //global_addr_map
      vector<string> elem_addr =				\
	reg_manage_obj->GetActualAddrFromGlobalVal(struct_name, \
						   elem_index);
      reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
      vector<string> op_des_addr_vec =				\
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
      if(elem_addr.size() != op_des_addr_vec.size()){
	cout << "TranceLoad(): op_des size and op_src size no "	\
	  "equal!" << endl;
	abort();
      }
      for(int i = 0; i < elem_addr.size(); i++){
	OutPut("movf", elem_addr[i], IR_name);
	OutPut("movwf", op_des_addr_vec[i], IR_name);
      }
    }
    //the struct variable store in data_area_map
    else if(which_store_map_index == 4){
     DataAreaManage *data_area_manage_obj = \
       DataAreaManage::getInstance();
      //get the elem_index adddr in struct_name
      vector<string> elem_addr =			      \
	data_area_manage_obj->GetActualAddrFromVal(struct_name,\
						   elem_index);
      //deal with op_des
      reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
      vector<string> op_des_addr =				\
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);

      if(elem_addr.size() != op_des_addr.size()){
	cout << "TranceLoad(): op_des size and op_src size no "\
	  "equal!" << endl;
	abort();
      }
      for(int i = 0; i < elem_addr.size(); i++){
	string addr_h(elem_addr[i].substr(0,2));
	string addr_l(elem_addr[i].substr(2,2));
	OutPut("movlw", addr_h, IR_name);
	OutPut("movwf", "tblptrh", IR_name);
	OutPut("movlw", addr_l, IR_name);
	OutPut("movwf", "tblptrl", IR_name);
	OutPut("tblrd", "*", IR_name);
	OutPut("movwf", "tablat", IR_name);
	
	OutPut("movf", "tablat", IR_name);
	OutPut("movwf", op_des_addr[i], IR_name);
      }
    }
  }
  //get element from register
  else {

    string op_src = wordCon.vaCol[5];
    string op_des = wordCon.vaCol[0];
    DataStoreInfo core_info; //reg_name;
    //the two regex are used to distict global var or general var
    //global var
    regex reg1(".*@.+"); 
    // general var
    regex reg2(".*\%.+"); 
    RegManage* reg_manage_obj = RegManage::getInstance();
    
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_src = current_fun_name + "." + op_src;
      op_des = current_fun_name + "." + op_des;
    }
   
    if(regex_match(op_src, reg1)){
      core_info = reg_manage_obj->GetAllInfoFromGlobalVal(op_src);
    } else if(regex_match(op_src, reg2)){
      core_info = reg_manage_obj->GetAllInfoFromGenVal(op_src);
    }
   
    reg_manage_obj->CopyAMapForGenVal(op_des, core_info);
  }
#undef VEC
}

void TranceStore(SplitWord wordCon, string IR_name){
#define VEC wordCon.vaCol

  regex regular_expr_struct("%struct.*");
  //Store the value to the elem in array
  if(find(VEC.begin(), VEC.end(), "getelementptr") != VEC.end() && \
     find(VEC.begin(), VEC.end(), "x") != VEC.end()){
    string op_src = VEC[2];
    string op_src_type = VEC[1];
    string array_name = VEC[12];
    //store which elem in the array
    string which_elem_index = VEC[16];
    //change the index to dec number
    int elem_index = ChangeStrToDec(which_elem_index);
    
    MemoryManage memory_manage_obj = MemoryManage();
    RegManage* reg_manage_obj = RegManage::getInstance();
    int reg_num = memory_manage_obj.HowBigType(op_src_type);

    //source variable, it may be a reg addr or split value
    vector<string> src_addr_or_value_vec;
    //decide the op_src wheather a register or a constant
    //the reg_or_constant math register
    //get the src value
    regex reg_or_constant(".*\%.*");
    if(regex_match(op_src, reg_or_constant)){
      //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	op_src = current_fun_name + "." + op_src;
      }
      src_addr_or_value_vec =					\
	reg_manage_obj->GetActualAddrFromGenVal(op_src, 0);
    }
    //the src is constant
    else {
      src_addr_or_value_vec =					   \
	memory_manage_obj.GetSplitSectionOfANum(op_src, reg_num);
    }
    //get the index that dedicated the variable store which map
    int which_store_map_index =				\
      memory_manage_obj.VarStoreWhichMap(array_name);
    //the value should store in reg_addr_map
    if(which_store_map_index == 1){     
      //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	array_name = current_fun_name + "." + array_name;
      }
      //get the actual addr of the op_des
      vector<string> elem_addr_vec = \
	reg_manage_obj->GetActualAddrFromGenVal(array_name, \
						elem_index);
      
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", src_addr_or_value_vec[i], IR_name);
	OutPut("movwf", elem_addr_vec[i], IR_name);
      }
    }
    //the value should store in global_addr_map
    else if(which_store_map_index == 2){
      //get the actual addr of the op_des
      vector<string> elem_addr_vec =				  \
	reg_manage_obj->GetActualAddrFromGlobalVal(array_name,	  \
						   elem_index);
      
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", src_addr_or_value_vec[i], IR_name);
	OutPut("movwf", elem_addr_vec[i], IR_name);
      }
    }
    //the value should store in data_area_map
    else if(which_store_map_index == 4){
      DataAreaManage* data_area_manage_obj =	\
	DataAreaManage::getInstance();
      //get the actual addr of the op_des
      vector<string> elem_addr_vec =		      		\
	data_area_manage_obj->GetActualAddrFromVal(array_name,	 \
						   elem_index);
      
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", src_addr_or_value_vec[i], IR_name);
	OutPut("movwf", "tablat", IR_name);
	OutPut("movlw", elem_addr_vec[i].substr(0,2), IR_name);
	OutPut("movwf", "tblptrh", IR_name);
	OutPut("movlw", elem_addr_vec[i].substr(2,4), IR_name);
	OutPut("movwf", "tblptrl", IR_name);
	OutPut("tblwr", "*", IR_name);
	
      }
    } 
  }
  //store the value to the elem in struct
  else if(find(VEC.begin(),VEC.end(),"getelementptr")!= VEC.end()&& \
	  regex_match(VEC[6], regular_expr_struct)){

    string op_src = VEC[2];
    string struct_var_name = VEC[8];
    string elem_index_str = VEC[12];
    string op_src_type = VEC[1];
    //change the string elem_index_str to int type
    int elem_index = ChangeHexToDec(elem_index_str);

    RegManage *reg_manage_obj = RegManage::getInstance();
    int reg_num = reg_manage_obj->HowBigType(op_src_type);
    //source variable, it may be a reg addr or split value
    vector<string> src_addr_or_value_vec;
    //decide the op_src wheather a register or a constance
    //the reg_or_constant match register
    //get the src value
    regex reg_or_constant(".*\%.*");
    if(regex_match(op_src, reg_or_constant)){
      //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	op_src = current_fun_name + "." + op_src;
      }

      src_addr_or_value_vec = \
	reg_manage_obj->GetActualAddrFromGenVal(op_src, 0);
    }
    //the src is constant
    else {
      src_addr_or_value_vec = \
	reg_manage_obj->GetSplitSectionOfANum(op_src,reg_num);
    }

    //we should know where the struct_var_name store?
    int where_struct_var_name_store = \
      reg_manage_obj->VarStoreWhichMap(struct_var_name);

    //the struct_var_name store in reg_addr_map
    if(where_struct_var_name_store == 1){
 
      reg_manage_obj->GetActualAddrFromGenVal(struct_var_name,  \
					      elem_index);	    
    } 
    //the struct_var_name store in global_addr_map;
    //the shruct_var_name don't be stored in data area map.
    //because the variable store in data area map is constant
    else if(where_struct_var_name_store == 2){
      vector<string> elem_addr_vec =			       \
	reg_manage_obj->GetActualAddrFromGlobalVal(struct_var_name,\
						   elem_index); 
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", src_addr_or_value_vec[i], IR_name);
	OutPut("movwf", elem_addr_vec[i], IR_name); 
      }
    }
    //struc var_name store in data_area_map;
    else if(where_struct_var_name_store == 4){
      DataAreaManage *data_area_manage_obj = \
	DataAreaManage::getInstance();
      vector<string> elem_addr_vec =		\
	data_area_manage_obj->GetActualAddrFromVal(struct_var_name,\
						   elem_index);
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", src_addr_or_value_vec[i], IR_name);
	OutPut("movwf", "tablat", IR_name);
	OutPut("movlw", elem_addr_vec[i].substr(0,2), IR_name);
	OutPut("movwf", "tblptrh", IR_name);
	OutPut("movlw", elem_addr_vec[i].substr(2,4), IR_name);
	OutPut("movwf", "tblptrl", IR_name);
	OutPut("tblwr", "*", IR_name);
      }
    }
    
  }
  //Store the value to the single variabel
  else{
    
    string op_src1 = wordCon.vaCol[2];
    string op_src2 = wordCon.vaCol[4];
    string op_src1_type = wordCon.vaCol[1];
    string op_src2_type = wordCon.vaCol[3];

    RegManage* reg_manage_obj = RegManage::getInstance();

    regex res(".*\%.*");
    //Instr: store R to R
    if(regex_match(op_src1, res)){

      //if no fun parameter
      if(!reg_manage_obj->WhetherHaveFunPara()) { 
	//add fun name before variable
	string current_fun_name =			\
	  reg_manage_obj->GetValueFromWhichFunStack();
	if(!current_fun_name.empty()){
	  op_src1 = current_fun_name + "." + op_src1;
	  op_src2 = current_fun_name + "." + op_src2;
	}
	
	vector<string> reg_name_src1 =				\
	  reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);

	vector<string> reg_name_src2 =				\
	  reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);
	
	if(reg_name_src1.size() != reg_name_src2.size()){
	  cout << "TranceStore():Source src size not"	\
	    "equal des src size!" << endl;
	  return ;
	}
	for(int i = 0; i < reg_name_src1.size(); i++){
	  OutPut("movf", reg_name_src1[i], IR_name);
	  OutPut("movwf", reg_name_src2[i], IR_name);
	}
      } 
      //is fun para
      else if(reg_manage_obj->WhetherHaveFunPara()){
	vector<string> reg_name_src1 =				\
	  reg_manage_obj->GetActualAddrFromFunPara(op_src1, 0);
	vector<string> reg_name_src2 =				\
	  reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);
	
	if(reg_name_src1.size() != reg_name_src2.size()){
	  cout << "Source src size not equal des src size!" << endl;
	  return ;
	}
	for(int i = 0; i < reg_name_src1.size(); i++){
	  OutPut("movf", reg_name_src1[i], IR_name);
	  OutPut("movwf", reg_name_src2[i], IR_name);
	}
      } 
      return;
    } 
    //Instr: store Constant to R
    else {    
      //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	op_src2 = current_fun_name + "." + op_src2;
      }

      vector<string> reg_name_src =				\
	reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);

      //Store a constance value into reg. Because a reg is only 
      //8 bit,
      //we have to splice a constant if this constant is large.
      //---
      //reg_num record the reg type. how many reg do reg_name_src
      //have.
      int reg_num = reg_name_src.size();
      //trange the op_src1 to split section num
      vector<string> val_vec =					\
	reg_manage_obj->GetSplitSectionOfANum(op_src1, reg_num);

      for(int i = 0; i < val_vec.size(); i++){      
	OutPut("movlw", val_vec[i], IR_name);
	OutPut("movwf", reg_name_src[i], IR_name);
      } //end for
      
    }//end else
  }
#undef VEC
}

void TranceAlloca(SplitWord wordCon, string IR_name){
#define VEC wordCon.vaCol 

  string var_name = wordCon.opCol[0];
  
  string var_type = wordCon.vaCol[3];
  //the IR operate the strcut.
  //there is something that we should know.
  //the struct var_name is local variable, so we will 
  //store it into reg_addr_map
  regex regular_expr_struct("%struct.*");
  if(regex_match(var_type, regular_expr_struct)){
    RegManage* reg_manage_obj = RegManage::getInstance();

    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      var_name = current_fun_name + "." + var_name;
    }

    //var_type is a struct
    string var_type = wordCon.vaCol[3];
    reg_manage_obj->AllocateRegToGenVal(var_name, var_type, 999);
    
  }
  //for the array variable
  else if(find(VEC.begin(), VEC.end(), "x") != VEC.end()){
    
    string op_des = VEC[0];
    string op_des_type = VEC[5].substr(0, VEC[5].size() - 1);
    int elem_num = ChangeStrToDec(VEC[3].substr(1));
    RegManage *reg_manage_obj = RegManage::getInstance();
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
    }
    //0 parameter represents not need to allocate actual addr
    //to op_des. It just create a record in reg_addr_map
    //update:
    //we need to allocate actual addr for op_des 
    reg_manage_obj->AllocateRegToGenVal(op_des,	     \
					op_des_type, \
					elem_num);
  }
  else {
    RegManage* reg_manage_obj = RegManage::getInstance();
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      var_name = current_fun_name + "." + var_name;
    }

    reg_manage_obj->AllocateRegToGenVal(var_name, var_type, 1);
  }
#undef VEC
}

void  TranceAdd(SplitWord wordCon, string IR_name){

  RegManage* reg_manage_obj = RegManage::getInstance();  
  //Instr: a = R + Constant or reverse
  if(wordCon.opCol.size() <= 2){ 
    string op_des = wordCon.opCol[0];
    string op_src1 = wordCon.vaCol[5];
    string op_src2 = wordCon.vaCol[6];

    string op_des_type = wordCon.vaCol[4];
    string op_src1_type = wordCon.vaCol[4];
    string op_src2_type = wordCon.vaCol[4];
  
    vector<string> value1, value2;
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
    }
    //deal with destination
    reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);      
    vector<string> op_des_addr =			       \
      reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
    
    //the section judge op_src1 whether is a reg or a constant
    regex reg("\%.*");
    if(regex_match(op_src1, reg)){  
      if(!current_fun_name.empty()){
	op_src1 = current_fun_name + "." + op_src1;
      }
      value1 = reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
    } 
    else{
      //value1 = op_src1; //?
    }
    
    //Instr: a = Constant + R
    if(regex_match(op_src2, reg)){  
      //deal with the data type
      int reg_num = reg_manage_obj->HowBigType(op_src2_type);
      if(!current_fun_name.empty()){
	op_src2 = current_fun_name + "." + op_src2;
      }
      //get reg value2 addr 
      value2 = reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);
      //get constant
      vector<string> constant = \
	reg_manage_obj->GetSplitSectionOfANum(op_src1, reg_num);
      if(reg_num == 1){
	//value2 is reg, value1 is constant 
	OutPut("movf", value2[0], IR_name);
	OutPut("addlw", constant[0], IR_name);
	OutPut("movwf", op_des_addr[0], IR_name);
      }
      //if the data type not 8 bit, then still deal with 
      else if(reg_num != 1){
	for(int i = 0; i < reg_num; i++){
	  OutPut("movf", constant[i], IR_name);
	  OutPut("movwf", RegManage::reserve_reg[0], IR_name);
	  OutPut("movf", value2[i], IR_name);
	  if(i == 0)    
	    OutPut("addwf", RegManage::reserve_reg[0], IR_name);
	  else 	        
	    OutPut("addwfc", RegManage::reserve_reg[0], IR_name);
	  OutPut("movwf", op_des_addr[i], IR_name);
	}
      }
      return;  
    } 
    //Instr: a = R + Constant
    else {                      
      //for descrement. change add to sub
      if(!op_src2.compare("-1")){   
	//get number reg by data type
	int reg_num = reg_manage_obj->HowBigType(op_src1_type);
	//get constant corresponce to type
	vector<string> constant = \
	  reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
	//get destination
	reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
	vector<string> reg_name = \
	  reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
	
	if(reg_num == 1){
	  //value2 is reg, value1 is constant 
	  OutPut("movf", value2[0], IR_name);
	  OutPut("sublw", constant[0], IR_name);
	  OutPut("movwf", reg_name[0], IR_name);
	} //if the data type not 8 bit, then still deal with 
	else if(reg_num != 1){
	  for(int i = 0; i < reg_num; i++){
	    OutPut("movf", constant[i], IR_name);
	    OutPut("movwf", RegManage::reserve_reg[0], IR_name);
	    OutPut("movf", value2[i], IR_name);
	    
	    if(i == 0)
	      OutPut("subwf", RegManage::reserve_reg[0], IR_name);
	    else 
	      OutPut("subwfc", RegManage::reserve_reg[0], IR_name);
	    
	    OutPut("movwf", reg_name[i], IR_name);
	  }
	}
      } 
      else {
	int reg_num = reg_manage_obj->HowBigType(op_src2_type);
	vector<string> constant = \
	  reg_manage_obj->GetSplitSectionOfANum(op_src2, reg_num);

	if(reg_num == 1){
	  //value2 is reg, value1 is constant 
	  OutPut("movf", value1[0], IR_name);
	  OutPut("addlw", constant[0], IR_name);
	  OutPut("movwf", op_des_addr[0], IR_name);
	}
	//if the data type not 8 bit, then still deal with 
	else if(reg_num != 1){

	  for(int i = 0; i < reg_num; i++){
	    OutPut("movf", constant[i], IR_name);
	    OutPut("movwf", RegManage::reserve_reg[0], IR_name);
	    OutPut("movf", value1[i], IR_name);
	    if(i == 0)	 
	      OutPut("addwf", RegManage::reserve_reg[0], IR_name);
	    else         
	      OutPut("addwfc", RegManage::reserve_reg[0], IR_name);   
	    OutPut("movwf", op_des_addr[i], IR_name);
	  }
	}
	
      }
    }
  } 
  //Instr: a = R + R
  else if(wordCon.opCol.size() >= 3){ //Instr: a = R + R
    string op_des = wordCon.opCol[0];
    string op_src1 = wordCon.opCol[1];
    string op_src2 = wordCon.opCol[2];

    string op_des_type = wordCon.vaCol[4];
    string op_src1_type = wordCon.vaCol[4];
    string op_src2_type = wordCon.vaCol[4];

        //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
      op_src1 = current_fun_name + "." + op_src1;
      op_src2 = current_fun_name + "." + op_src2;
    }

    vector<string> value1 = \
      reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
    vector<string> value2 = \
      reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);

    reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);   
    vector<string> reg_name = \
      reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
    int reg_num = reg_manage_obj->HowBigType(op_des_type);
   
    for(int i = 0; i < reg_num; i++){
      OutPut("movf", value1[i], IR_name);
      if(i == 0)  OutPut("addwf", value2[i], IR_name);
      else        OutPut("addwfc", value2[i], IR_name);
      OutPut("movwf", reg_name[i], IR_name);
    }
  }
  
}

//In sub Instr,there is important the oprator order in dirfferent 
//Instr.like: a = 88 - b; a = b - 88; or a = c - b; and a = b - c;
//so we should use different method.
//   
void TranceSub(SplitWord wordCon, string IR_name){

  RegManage* reg_manage_obj = RegManage::getInstance();
  //Instr: a = constant - R or reverse
  if(wordCon.opCol.size() <= 2){  
    string op_des = wordCon.opCol[0];
    string op_src1 = wordCon.vaCol[5];
    string op_src2 = wordCon.vaCol[6];

    string op_des_type = wordCon.vaCol[4];
    string op_src1_type = wordCon.vaCol[4];
    string op_src2_type = wordCon.vaCol[4];
    
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
    }
    reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
    vector<string> op_des_addr =			       \
      reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
    int reg_num = reg_manage_obj->HowBigType(op_src1_type);
    
    vector<string> value1, value2;

    regex res("\%.*");
    if(regex_match(op_src1, res)){
      if(!current_fun_name.empty()){
	op_src1 = current_fun_name + "." + op_src1;
      }
      value1 = reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
    } else {  //op_src1 represent a constant value
      //value1 = op_src1;
    } 
    //Instr: a = constant - R
    if(regex_match(op_src2, res)){ 
      //value2 is Reg and value1 is a constant
      if(!current_fun_name.empty()){
	op_src2 = current_fun_name + "." + op_src2;
      }      
      value2 = reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);
      value1 = reg_manage_obj->GetSplitSectionOfANum(op_src1, \
						    reg_num);
      if(reg_manage_obj->CheckAllOperatorType(op_src1_type, \
					     op_src2_type, \
					     op_des_type)){
	if(reg_num == 1){
	  OutPut("movf", value2[0], IR_name);
	  OutPut("sublw", value1[0], IR_name);
	  OutPut("movwf", op_des_addr[0], IR_name);
	} 
	else if(reg_num != 1){
	  for(int i = 0; i < reg_num; i++){
	    OutPut("movf", value1[i], IR_name);
	    OutPut("movwf", RegManage::reserve_reg[0], IR_name);
	    OutPut("movf", value2[i], IR_name);
	    if(i == 0) 	
	      OutPut("subwf", RegManage::reserve_reg[0], IR_name);
	    else       
	      OutPut("subwfb", RegManage::reserve_reg[0], IR_name);
	    OutPut("movwf", op_des_addr[i], IR_name);
	  }
	}
      }
      return;
    } 
    //Instr: a = R - Constant
    else {                    
      //value2 is constant and value1 is reg
      value2 =							\
	reg_manage_obj->GetSplitSectionOfANum(op_src2, reg_num);

      if(reg_num == 1){
	OutPut("movlw", value2[0], IR_name);
	OutPut("subwf", value1[0], IR_name);
	OutPut("movwf", op_des_addr[0], IR_name);
      }
      else if(reg_num != 1){
	
	for(int i = 0; i < reg_num; i++){
	  OutPut("movlw", value2[i], IR_name);
	  if(i == 0)	  OutPut("subwf", value1[i], IR_name);
	  else            OutPut("subwfb", value1[i], IR_name);
	  OutPut("movwf", op_des_addr[i], IR_name);
	}
      }
    }
  }
  //Instr: a = R - R
  else if(wordCon.opCol.size() >= 3){
    string op_des = wordCon.opCol[0];
    string op_src1 = wordCon.opCol[1];
    string op_src2 = wordCon.opCol[2];

    string op_des_type = wordCon.vaCol[4];
    string op_src1_type = wordCon.vaCol[4];
    string op_src2_type = wordCon.vaCol[4];
    
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
      op_src1 = current_fun_name + "." + op_src1;
      op_src2 = current_fun_name + "." + op_src2;
    }

    reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);

    vector<string> op_src1_vec = \
      reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
    vector<string> op_src2_vec = \
      reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);
    vector<string> reg_name = \
      reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
    int reg_num = reg_manage_obj->HowBigType(op_des_type);

    if(reg_manage_obj->CheckAllOperatorType(op_src1_type,	\
					   op_src2_type,
					   op_des_type)){
      if(reg_num == 1){
	OutPut("movf", op_src2_vec[0], IR_name);
	OutPut("subwf", op_src1_vec[0], IR_name);
	OutPut("movwf", reg_name[0], IR_name);
      }
      else if(reg_num != 1){
	
	for(int i = 0; i < reg_num; i++){
	  OutPut("movf", op_src2_vec[i], IR_name);
	  if(i == 0)	  OutPut("subwf", op_src1_vec[i], IR_name);
	  else            OutPut("subwfb", op_src1_vec[i], IR_name);
	  OutPut("movwf", reg_name[i], IR_name);
	}
      }

      //OutPut("movf", reg_name_src2);
      //OutPut("subwf", reg_name_src1);
      //OutPut("movwf", reg_name_des);
    }
  }
}


//because the core havn't campare instr,
//we comlete this instr via sub instr.
//when use the sub instr for the two oprator, 
//check the sigal bit in STATUS register.
// 
void TranceFcmp(SplitWord wordCon, string IR_name){
  string instr_name = wordCon.vaCol[3];
  if(!instr_name.compare("sgt") || !instr_name.compare("ogt")){
    TranceSub(wordCon, IR_name);
    lastInstrName = "gt";
  } else if(!instr_name.compare("slt")|| !instr_name.compare("olt")){
    TranceSub(wordCon, IR_name);
    lastInstrName = "lt";
  } else if(!instr_name.compare("sle")||!instr_name.compare("ole")){ 
    TranceSub(wordCon, IR_name);
    lastInstrName = "le";
  } else if(!instr_name.compare("sge")|| !instr_name.compare("oge")){
    TranceSub(wordCon, IR_name);
    lastInstrName = "ge";
  } else if(!instr_name.compare("ne") || !instr_name.compare("one")){
    TranceSub(wordCon, IR_name);
    lastInstrName = "ne";
  } else if(!instr_name.compare("eq") || !instr_name.compare("oeq")){
    TranceSub(wordCon, IR_name);
    lastInstrName = "eq";
  }
}

//The BR instr is for the control statement.
//decide the condition by testing the 0 bit in STATUS register,
//btfsc: if the bit is 0, then ignore the next statement;
//btfss: if the bit is 1, then ignore the next statement
//
void TranceBr(SplitWord wordCon, string IR_name){
  //Instr: for br %x %x %x
  if(wordCon.opCol.size() >= 2){ 
    //lastInstrName is global variable. it write in trancefcmp() fun
    //because this instr need some info from last instr
    string instrName = lastInstrName;   
    RegManage* reg_manage_obj = RegManage::getInstance();
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    
    //----------
    string beforeOpBlock1 = wordCon.vaCol[4];
    string beforeOpBlock2 = wordCon.vaCol[6];
    
    //add fun name before variable
    if(!current_fun_name.empty()){
      beforeOpBlock1 = current_fun_name + "." + beforeOpBlock1;
      beforeOpBlock2 = current_fun_name + "." + beforeOpBlock2;
    }

    reg_manage_obj->AllocateRegToGenVal(beforeOpBlock1,		\
					"i8", 1);
    reg_manage_obj->AllocateRegToGenVal(beforeOpBlock2,\
					"i8", 1);
    vector<string> res_op1 =					\
      reg_manage_obj->GetActualAddrFromGenVal(beforeOpBlock1, 0);
    vector<string> res_op2 =					\
      reg_manage_obj->GetActualAddrFromGenVal(beforeOpBlock2, 0);

    OutPut("movlw", "0", IR_name);
    for(int i = 0; i < 1; i++){
      OutPut("movwf", res_op1[i], IR_name);
      OutPut("movwf", res_op2[i], IR_name);
     }
     //-------------    
        
     string test_name = wordCon.vaCol[2];
     string inner_label = wordCon.vaCol[2].substr(1);
     //add fun name before variable
     if(!current_fun_name.empty()){
       test_name = current_fun_name + "." + test_name;
     }
     vector<string> reg_name_test =				\
       reg_manage_obj->GetActualAddrFromGenVal(test_name, 0);
     int reg_num = reg_name_test.size();
    
     string opBlock1 = wordCon.vaCol[4].substr(1);
     string opBlock2 = wordCon.vaCol[6].substr(1);
     //Instr: a <= b
     if(!instrName.compare("le")){
       OutPut("btfsc", "STATUS", 0, 1, IR_name);
       OutPutJump("bra", opBlock1, IR_name);
       OutPutJump("bra", opBlock2, IR_name);
     } 
     //Instr: a >= b
     else if(!instrName.compare("ge")){  
       OutPut("btfss", "STATUS", 0, 1, IR_name);
       OutPutJump("bra", opBlock1, IR_name);
       OutPutJump("bra", opBlock2, IR_name);
     } 
     //Instr: a < b
     else if(!instrName.compare("lt")){
       OutPut("movlw", "0", IR_name);
       for(int i = 0; i < reg_num - 1; i++){
	 OutPut("cpfseq", reg_name_test[i], IR_name);
	 OutPutJump("bra", inner_label, IR_name);
       }
       OutPut("cpfseq", reg_name_test[reg_num - 1], IR_name);
       OutPutLabel(inner_label, IR_name);
       OutPut("btfss", "STATUS", 0, 1, IR_name);
       OutPutJump("bra", opBlock2, IR_name);
       OutPutJump("bra", opBlock1, IR_name);    
     }
     //Instr: a > b
     else if(!instrName.compare("gt")){
       OutPut("movlw", "0", IR_name);      
       for(int i = 0; i < reg_num - 1; i++){
	 OutPut("cpfseq", reg_name_test[i], IR_name);
	 OutPutJump("bra", inner_label, IR_name);
       }
       OutPut("cpfseq", reg_name_test[reg_num - 1], IR_name);
       OutPutLabel(inner_label, IR_name);
       OutPut("btfsc", "STATUS", 0, 1, IR_name);
       OutPutJump("bra", opBlock2, IR_name);
       OutPutJump("bra", opBlock1, IR_name);
     }
     //Instr: a != b ?
     else if(!instrName.compare("ne")){
       OutPut("movlw", "0", IR_name);
       for(int i = 0; i < reg_num - 1; i++){
	 OutPut("cpfseq", reg_name_test[i], IR_name);
	 OutPutJump("bra", inner_label, IR_name);
       }
       OutPut("cpfseq", reg_name_test[reg_num - 1], IR_name);
       OutPutLabel(inner_label, IR_name);
       OutPutJump("bra", opBlock1, IR_name);
       OutPutJump("bra", opBlock2, IR_name);
     }
     //Instr: a == b?
     else if(!instrName.compare("eq")){
       OutPut("movlw", "0", IR_name);
       for(int i = 0; i < reg_num - 1; i++){
	 OutPut("cpfseq", reg_name_test[i], IR_name);
	 OutPutJump("bra", inner_label, IR_name);
       }
       OutPut("cpfseq", reg_name_test[reg_num - 1], IR_name);
       OutPutLabel(inner_label, IR_name);
       OutPutJump("bra", opBlock2, IR_name);
       OutPutJump("bra", opBlock1, IR_name);
     }
    
  } 
//   //Instr: br label %x
  else if(wordCon.opCol.size() <= 1){ 
    string opBlock = wordCon.opCol[0].substr(1);
    OutPutJump("bra", opBlock, IR_name);
  }
  lastInstrName = " ";
}




void TranceZext(SplitWord wordCon, string IR_name){
  string op_des = wordCon.opCol[0];
  string op_src1 = wordCon.vaCol[4];
    
  string op_des_type = wordCon.vaCol[6];
  //    string op_src1_type = wordCon.vaCol[6];

  RegManage* reg_manage_obj = RegManage::getInstance();

  //add fun name before variable
  string current_fun_name =				\
    reg_manage_obj->GetValueFromWhichFunStack();
  if(!current_fun_name.empty()){
    op_des = current_fun_name + "." + op_des;
      op_src1 = current_fun_name + "." + op_src1;
    }

    vector<string> op_src1_vec =			\
      reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
    //deal with op_des
    reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);      
    vector<string> op_des_vec =				\
      reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);

    string instrName = lastInstrName;
    int reg_num = op_src1_vec.size();
    string inner_label = wordCon.vaCol[0].substr(1);
    //regNam=(regNameTest==0)?1:0;

    //m = (j == 0)
    if(!instrName.compare("eq")){
 
      vector<string> constant_1 = \
	reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
      
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", constant_1[i], IR_name);
	OutPut("movwf", op_des_vec[i], IR_name);          
      }
      OutPut("movlw", "0", IR_name);
      for(int j = 0; j < reg_num; j++){
	OutPut("cpfseq", op_src1_vec[j], IR_name); 
	OutPut("movwf", op_des_vec[j], IR_name);
      }
      //regName=(regNameTest==0)?0:1;
    } 
    //m = (j != 0)
    else if(!instrName.compare("ne")){

      vector<string> constant_1 = \
	reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
      
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", constant_1[i], IR_name);
	OutPut("movwf", op_des_vec[i], IR_name);
      }
      OutPut("movlw", "0", IR_name);
      for(int j = 0; j < reg_num; j++){	
	OutPut("cpfseq", op_src1_vec[j], IR_name);
	OutPutJump("bra", inner_label + "_next", IR_name);
	OutPut("movwf", op_des_vec[j], IR_name);
      }
      OutPutLabel(inner_label + "_next", IR_name);

	
    }
    //Instr: (a > b)
    else if(!instrName.compare("gt")){
      vector<string> constant_1 =				\
	reg_manage_obj->GetSplitSectionOfANum("1", reg_num);

      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", constant_1[i], IR_name);
	OutPut("movwf", op_des_vec[i], IR_name);
      }
      OutPut("movlw", "0", IR_name);
      for(int j = 0; j < reg_num; j++){
	OutPut("cpfseq", op_src1_vec[j], IR_name);
	OutPut("btfsc", "STATUS", 0, 1, IR_name);
	OutPut("movwf", op_des_vec[j], IR_name);   
      }
    } 
    //Instr: (a < b)
    else if(!instrName.compare("lt")){
      vector<string> constant_1 =				\
	reg_manage_obj->GetSplitSectionOfANum("1", reg_num);

      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", constant_1[i], IR_name);
	OutPut("movwf", op_des_vec[i], IR_name);
      }
      OutPut("movlw", "0", IR_name);      
      for(int j = 0; j < reg_num; j++){
	OutPut("cpfseq", op_src1_vec[j], IR_name);
	OutPut("btfss", "STATUS", 0, 1, IR_name);
	OutPut("movwf", op_des_vec[j], IR_name);
      }
    } 
    //Instr: (a >= b)
    else if(!instrName.compare("ge")){ 
      vector<string> constant_1 =				\
	reg_manage_obj->GetSplitSectionOfANum("1", reg_num);

      OutPut("movlw", "0", IR_name);
      for(int i = 0; i < reg_num; i++){
	OutPut("cpfseq", op_src1_vec[i], IR_name); //if equal, jump 
	OutPutJump("bra", inner_label + "_next", IR_name);
      }

      OutPutLabel(inner_label + "_next", IR_name);

      OutPut("btfss", "STATUS", 0, 1, IR_name);   //if equal 1, jump
      OutPutJump("bra", inner_label + "_next2", IR_name);
      OutPutJump("bra", inner_label + "_next1", IR_name);
      
      OutPutLabel(inner_label + "_next1", IR_name);  
      OutPut("movlw", "0", IR_name);
      for(int j = 0; j < reg_num; j++){
	OutPut("movwf", op_des_vec[j], IR_name);
      }

      OutPutJump("bra", inner_label + "_next3", IR_name);      
      
      OutPutLabel(inner_label + "_next2", IR_name);
      for(int j = 0; j < reg_num; j++){
	OutPut("movlw", constant_1[j], IR_name);
	OutPut("movwf", op_des_vec[j], IR_name);
      }
      OutPutLabel(inner_label + "_next3", IR_name);
    
    }
    //Instr: (a <= b)
    else if(!instrName.compare("le")){

      vector<string> constant_1 = \
	reg_manage_obj->GetSplitSectionOfANum("1", reg_num);

      OutPut("movlw", "0", IR_name);
      for(int i = 0; i < reg_num; i++){
	OutPut("cpfseq", op_src1_vec[i], IR_name);
	OutPutJump("bra", inner_label + "_next", IR_name);
      }
      OutPutLabel(inner_label + "_next", IR_name);
      
      OutPut("btfsc", "STATUS", 0, 1, IR_name);

      OutPutJump("bra", inner_label + "_next2", IR_name);
      OutPutJump("bra", inner_label + "_next1", IR_name);
      OutPutLabel(inner_label + "_next1", IR_name);
      OutPut("movlw", "0", IR_name);
      for(int j = 0; j < reg_num; j++){
	OutPut("movwf", op_des_vec[j], IR_name);
      }
      OutPutJump("bra", inner_label + "_next3", IR_name);
      
      OutPutLabel(inner_label + "_next2", IR_name);
      for(int j = 0; j < reg_num; j++){
	OutPut("movlw", constant_1[j], IR_name);
	OutPut("movwf", op_des_vec[j], IR_name);
      }
      OutPutLabel(inner_label + "_next3", IR_name);
    }
    lastInstrName = " ";
    
    //extension type
    int des_num = reg_manage_obj->HowBigType(op_des_type);
    //both tyep is equal, so we don't need to extension
    if(des_num == op_src1_vec.size()) return;
    //if destintion type bigger than op_src type
    if(des_num > op_src1_vec.size()){
      OutPut("movlw", "0", IR_name);
      for(int i = 0; i < des_num - op_src1_vec.size(); i++){
	OutPut("movwf", op_des_vec[op_src1_vec.size() + i], IR_name);
      }
    }
    //if op_src type bigger than destination type
    else{

    }



    

}

//print the label as a signal of jump statement
//
void TranceLabel(SplitWord wordCon, string IR_name){
  string label_name = wordCon.vaCol[2];
  OutPutLabel(label_name, IR_name);
  
  /*
  //?
  RegManage* reg_manage_obj = RegManage::getInstance();
  label_name = "%" + label_name;
  //add fun name before variable
  string current_fun_name =				\
    reg_manage_obj->GetValueFromWhichFunStack();
  if(!current_fun_name.empty()){
    label_name = current_fun_name + "." + label_name;
  }

  vector<string> op_des = \
    reg_manage_obj->GetActualAddrFromGenVal(label_name, 0);

  int reg_num = op_des.size();
  vector<string> constant_1 = \
    reg_manage_obj->GetSplitSectionOfANum("1", reg_num);

  if(op_des.size() != 0){
    for(int i = 0; i < reg_num; i++){
      OutPut("movlw", constant_1[i], IR_name);
      OutPut("movwf", op_des[i], IR_name);
    }
  }
  */
  //
  //if(getRegValue(labelName).compare(" ")){
   //string regName = getRegValue(labelName);
    //OutPut("movlw", "1");
    //OutPut("movwf", regName);
  //}
    
}

//global var 
//
void TranceGlobal(SplitWord wordCon, string IR_name){
#define  VEC wordCon.vaCol

  regex regular_expr_struct("%struct.*");
  RegManage* reg_manage_obj = RegManage::getInstance();
  if(find(VEC.begin(), VEC.end(), "global") != VEC.end() && \
     find(VEC.begin(), VEC.end(), "common") != VEC.end() && \
     find(VEC.begin(), VEC.end(), "zeroinitializer") != VEC.end() &&\
     regex_match(VEC[4], regular_expr_struct)){
    RegManage* reg_manage_obj = RegManage::getInstance();
    //ex: @outer ,remove @
    string struct_name = wordCon.vaCol[0];
    string struct_name_type = wordCon.vaCol[4];
    //allocate addr for the struct, -1 parameter represent the 
    //type is struct, each elem have different type
    reg_manage_obj->AllocateRegToGlobalVal(struct_name, \
					   struct_name_type,\
					   999);
    //get all addr of the struct_name that have been allocated
    vector<string> struct_addr = \
      reg_manage_obj->GetAllActualAddrFromGlobalVal(struct_name);
    //get each elem type
    const vector<string>& elem_type = \
      reg_manage_obj->GetRecordVarDecalare(struct_name_type);

    int k = 0;
    for(int i = 0; i < elem_type.size(); i++){
      int reg_num = \
	reg_manage_obj->HowBigType(elem_type[i]);
      vector<string> split_elem = \
	reg_manage_obj->GetSplitSectionOfANum("0", reg_num);
      for(int j = 0; j < reg_num; j++){
	OutPut("movlw", split_elem[j], IR_name);
	OutPut("movwf", struct_addr[k], IR_name);
	++k;
      }
    }
  }
  else if(find(VEC.begin(), VEC.end(), "global") != VEC.end() && \
	  regex_match(VEC[3], regular_expr_struct)){
    RegManage* reg_manage_obj = RegManage::getInstance();
    //ex: @outer ,remove @
    string struct_name = wordCon.vaCol[0];
    string struct_name_type = wordCon.vaCol[3];
    //allocate addr for the struct, -1 parameter represent the 
    //type is struct, each elem have different type
    reg_manage_obj->AllocateRegToGlobalVal(struct_name, \
					   struct_name_type,\
					   999);
    //get all addr of the struct_name that have been allocated
    vector<string> struct_addr =				\
      reg_manage_obj->GetAllActualAddrFromGlobalVal(struct_name);
    //get each elem type
    const vector<string>& elem_type = \
      reg_manage_obj->GetRecordVarDecalare(struct_name_type);
    //get struct data from the IR instr
    vector<string> struct_value = GetStructValueFromIRInstr(wordCon);
    int k = 0;
    for(int i = 0; i < elem_type.size(); i++){
      int reg_num = \
	reg_manage_obj->HowBigType(elem_type[i]);
      vector<string> split_elem = \
	reg_manage_obj->GetSplitSectionOfANum(struct_value[i], \
					      reg_num);
      for(int j = 0; j < reg_num; j++){
	OutPut("movlw", split_elem[j], IR_name);
	OutPut("movwf", struct_addr[k], IR_name);
	++k;
      }
    }
  }
  //it is a global array, but not initializer
  else if(find(VEC.begin(), VEC.end(), "global") != VEC.end() &&   \
	  find(VEC.begin(), VEC.end(), "common") != VEC.end() &&   \
	  find(VEC.begin(), VEC.end(), "x") != VEC.end() &&	   \
	  find(VEC.begin(), VEC.end(), "zeroinitializer") != \
	  VEC.end()){
    
    RegManage* reg_manage_obj = RegManage::getInstance();
    string array_name = wordCon.vaCol[0];
    //get the size of this array from IR instr
    int array_elem_num = GetArrayElemNum(wordCon);
    //get the elem type of the array from IR instr
    string array_elem_type = GetArrayElemType(wordCon);
    //allocate addr for the array
    reg_manage_obj->AllocateRegToGlobalVal(array_name, \
					   array_elem_type, \
					   array_elem_num);
    vector<string> array_addr_vec = \
      reg_manage_obj->GetAllActualAddrFromGlobalVal(array_name);
    int reg_num = reg_manage_obj->HowBigType(array_elem_type);
    
    int k = 0;
    for(int i = 0; i < array_elem_num; i++){  
      vector<string> split_value =				\
	reg_manage_obj->GetSplitSectionOfANum("0", reg_num);
      for(int j = 0; j < reg_num; j++){
	OutPut("movlw", split_value[j], IR_name);
	OutPut("movwf", array_addr_vec[k], IR_name);
	++k;
      }
    }
  }
  //it is a global array, with zeroinitializer
  else if(find(VEC.begin(), VEC.end(), "global") != VEC.end() && \
	  find(VEC.begin(), VEC.end(), "x") != VEC.end() && \
	  find(VEC.begin(), VEC.end(), "zeroinitializer") \
	  != VEC.end()){
    
    RegManage* reg_manage_obj = RegManage::getInstance();
    string array_name = wordCon.vaCol[0];
    //get the size of this array from IR instr
    int array_elem_num = GetArrayElemNum(wordCon);
    //get the elem type of the array from IR instr
    string array_elem_type = GetArrayElemType(wordCon);
    //allocate addr for the array
    reg_manage_obj->AllocateRegToGlobalVal(array_name,	    \
					   array_elem_type, \
					   array_elem_num);
    vector<string> array_addr_vec =				\
      reg_manage_obj->GetAllActualAddrFromGlobalVal(array_name);
    int reg_num = reg_manage_obj->HowBigType(array_elem_type);
    
    int k = 0;
    for(int i = 0; i < array_elem_num; i++){  
      vector<string> split_value =				\
	reg_manage_obj->GetSplitSectionOfANum("0", reg_num);
      for(int j = 0; j < reg_num; j++){
	OutPut("movlw", split_value[j], IR_name);
	OutPut("movwf", array_addr_vec[k], IR_name);
	++k;
      }
    }
  }
  //it is a global array
  else if(find(VEC.begin(), VEC.end(), "global") != VEC.end() &&  \
     find(VEC.begin(), VEC.end(), "x") != VEC.end()){
    RegManage* reg_manage_obj = RegManage::getInstance();
    string op_des = wordCon.vaCol[0];
    //get the elem num of this array
    int array_elem_num = GetArrayElemNum(wordCon);
    //get the elem type of this array
    string array_elem_type = GetArrayElemType(wordCon);
    //allocate addr to this array variable, but these addr will 
    //store int the variable together.
    reg_manage_obj->AllocateRegToGlobalVal(wordCon.vaCol[0], \
					   array_elem_type, \
					   array_elem_num);
    //get the array addr that have been allocated
    vector<string> array_addr_vec = \
      reg_manage_obj->GetAllActualAddrFromGlobalVal(wordCon.vaCol[0]);
    //get the array value from the IR instr
    vector<string> array_value_vec;
    getDataFromInstr(array_value_vec, wordCon);
    //get the type
    int reg_num = reg_manage_obj->HowBigType(array_elem_type);
  
    if(array_value_vec.size() != 0 && array_addr_vec.size() != 0){
      int k = 0;
      for(int i = 0; i < array_value_vec.size(); i++){
	//get a value from array_value_vec, and split it according 
	//to the type of the elem
	vector<string> array_value_split =			  \
	  reg_manage_obj->GetSplitSectionOfANum(array_value_vec[i],\
						reg_num);
	
	for(int j = 0; j < array_value_split.size(); j++){
	  OutPut("movlw", array_value_split[j], IR_name);
	  OutPut("movwf", array_addr_vec[k], IR_name);
	  /*
	  OutPut("movlw", array_value_split[j], IR_name);
	  OutPut("movwf", "tablat", IR_name);
	  OutPut("movlw", array_addr_vec[k].substr(0,2), IR_name);
	  OutPut("movwf", "tblptrh", IR_name);
	  OutPut("movlw", array_addr_vec[k].substr(2,4), IR_name);
	  OutPut("movwf", "tblptrl", IR_name);
	  OutPut("tblwr", "*", IR_name);
	  */
	  ++k;
	}//end for
      }//end for
    } //end if

  }
  //it is a global variable
  else {
    string op_des = wordCon.vaCol[0];
    //get the global variable name
    string op_src = wordCon.vaCol[4];
    //get op_des_type
    string op_des_type = wordCon.vaCol[3];
    //allocate addr for the global variable
    reg_manage_obj->AllocateRegToGlobalVal(op_des, op_des_type, 1);
    //get the addr that have been allocated
    vector<string>  reg_name_src =			\
      reg_manage_obj->GetActualAddrFromGlobalVal(op_des, 0);
    //get the bit of the type 
    int reg_num = reg_name_src.size();
    //get the split value according to the type
    vector<string> val_vec =					\
      reg_manage_obj->GetSplitSectionOfANum(op_src, reg_num);
  
    for(int i = 0; i < val_vec.size(); i++){
      OutPut("movlw", val_vec[i], IR_name);
      OutPut("movwf", reg_name_src[i], IR_name);   
    }//end for
  }
#undef VEC 
}

void TranceDefine(SplitWord wordCon, string IR_name){
  //used for main, if this fun is main then we should
  //do something
  if(firstFun == true){ 
    firstFun = false;
    OutPutJump("bra", "begin", IR_name);
    OutPutLabel("begin", IR_name);
    OutPutJump("bra","main", IR_name);
  } 

  string op_des = wordCon.vaCol[2];
  int index = op_des.find("(");
  string fun_name = op_des.substr(1, index-1);
  //print fun name label
  OutPutLabel(fun_name, IR_name); 

  MemoryManage memory_manage_obj = MemoryManage();
  //updata the fun name that now doing process
  memory_manage_obj.SetValueToWhichFunStack(fun_name);
  
  //this fun will receive parameter
  //ex: define void @max(i32 signext, i32 signext) #0 {
  if(find(wordCon.vaCol.begin(), wordCon.vaCol.end(), "signext") != \
     wordCon.vaCol.end()){
    vector<string> parameter_name;
    vector<string> parameter_type;
    //get the parameter name and type
    GetParameterNameAndParameterType(wordCon, \
				     parameter_name,
				     parameter_type);
    //how many parameter the fun have?
    int parameter_num = parameter_type.size();
    RegManage* reg_manage_obj = RegManage::getInstance();
    //deliever the parameter
    //put the reserve_reg to the local addr
    int k = 0;
    for(int i = 0; i < parameter_num; i++){
      string op_des = "%" + to_string(i);
      string op_des_type = parameter_type[i];
      //add fun name before variable
      string current_fun_name =					\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	op_des = current_fun_name + "." + op_des;
      }
      reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
      vector<string> op_des_addr_vec =				\
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
      
      for(int j = 0; j < op_des_addr_vec.size(); j++){
	OutPut("movf", RegManage::reserve_reg[k++], IR_name);
	OutPut("movwf", op_des_addr_vec[j], IR_name);
      }
    }
    
  }



  numPara = 0; //fun para number
  for(auto elem : wordCon.vaCol){ //How many para 
    if(strcmp(elem.c_str(), "signext") == 0){
      ++numPara;
    }
  }

}

void TranceCall(SplitWord wordCon, string IR_name){
#define VEC wordCon.vaCol
  RegManage *reg_manage_obj = RegManage::getInstance();
  regex regular_expr_struct("%struct.*");

  //for array variable initial//? not getelementptr in initial array
  if(find(VEC.begin(), VEC.end(), "getelementptr") != VEC.end() &&
     find(VEC.begin(), VEC.end(), "x") != VEC.end() ){
    string op_src = wordCon.vaCol[14];
    string op_des = wordCon.vaCol[4];

    RegManage *reg_manage_obj = RegManage::getInstance();  
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      //      op_src = current_fun_name + "." + op_src;
      op_des = current_fun_name + "." + op_des;
    }
    
    string last_op_src = IR_var_correspond_map.find(op_des)->second;
    IR_var_correspond_map.erase(op_des);
  
    DataStoreInfo core_info_op_src =			\
      reg_manage_obj->GetAllInfoFromGenVal(op_src);
    core_info_op_src.virtual_addr = last_op_src;

    reg_manage_obj->DeleteRecordInGenVal(last_op_src);
    
    reg_manage_obj->CreateMapForGenVal(last_op_src, \
				       core_info_op_src);

  }
  //for array variable have been initial
  //------
  //call void @llvm.memcpy.p0i8.p0i8.i32(i8* %5, i8* bitcast	\
  //([3 x i32]* @main.myint2 to i8*), i32 12, i32 4, i1 false)
  else if(find(VEC.begin(), VEC.end(), "bitcast") != VEC.end() && \
	  find(VEC.begin(), VEC.end(), "x") != VEC.end() && \
	  find(VEC.begin(), VEC.end(), "to") != VEC.end()) {
    string array_name = VEC[10];
    string op_des = VEC[4];

    RegManage *reg_manage_obj = RegManage::getInstance();
    //add fun name before variable
    string current_fun_name = \
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
    }
    string last_op_des = IR_var_correspond_map.find(op_des)->second;
    IR_var_correspond_map.erase(op_des);

    DataStoreInfo core_info_op_src =			\
      reg_manage_obj->GetAllInfoFromGenVal(array_name);
    core_info_op_src.virtual_addr = last_op_des;
    //first delete the older record, and creat a new record
    //with actual addr core_info_op_src
    reg_manage_obj->DeleteRecordInGenVal(last_op_des);
    reg_manage_obj->CreateMapForGenVal(last_op_des, \
				       core_info_op_src);
  }
  else if(find(VEC.begin(), VEC.end(), "bitcast") != VEC.end() && \
	  find(VEC.begin(), VEC.end(), "to") != VEC.end() && \
	  regex_match(VEC[7], regular_expr_struct)){
    string struct_name = wordCon.vaCol[8];
    string op_des = wordCon.vaCol[4];
    
    RegManage *reg_manage_obj = RegManage::getInstance();
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_des = current_fun_name + "." + op_des;
    }
    
    string last_op_src = IR_var_correspond_map.find(op_des)->second;
    IR_var_correspond_map.erase(op_des);

    DataStoreInfo core_info_op_src = \
      reg_manage_obj->GetAllInfoFromGenVal(struct_name);
    core_info_op_src.virtual_addr = last_op_src;

    reg_manage_obj->DeleteRecordInGenVal(last_op_src);
    reg_manage_obj->CreateMapForGenVal(last_op_src, \
				       core_info_op_src);
    
  }
  //this is fun call and there are parameter to deliver
  
  else if(find(VEC.begin(), VEC.end(), "signext") != VEC.end()){ 
    regex reg("\%.+");
    //because call fun will return value or don't return value,
    //we should judge firstly.
    //if VEC[0] == call, then the fun don't return value
    if(!VEC[0].compare("call")){
      vector<string> parameter_name;
      vector<string> parameter_type;
      //get the parameter name and type
      GetParameterNameAndParameterType(wordCon,	 \
				       parameter_name,	\
				       parameter_type);
      //the index of the reserve_reg
      int k = 0;
      //deal with each parameter, deliver the parameter
      //to reserve_reg
      for(int i = 0; i < parameter_name.size(); i++){
	string each_parameter_name = parameter_name[i];
	string each_parameter_type = parameter_type[i];
	int reg_num = reg_manage_obj->HowBigType(parameter_type[i]);
	//the parameter is register
	if(regex_match(each_parameter_name, reg)){
	  //add fun name before variable
	  string current_fun_name =			\
	    reg_manage_obj->GetValueFromWhichFunStack();
	  if(!current_fun_name.empty()){
	    each_parameter_name =				\
	      current_fun_name + "." + each_parameter_name;
	  }
	  //get the parameter addr
	  vector<string> parameter_addr_vec = reg_manage_obj-> \
	    GetActualAddrFromGenVal(each_parameter_name, 0);
	  //deliver the parameter_addr to reserve_reg
	  for(int i = 0; i < parameter_addr_vec.size(); i++){
	    OutPut("movf", parameter_addr_vec[i], IR_name);
	    OutPut("movwf",RegManage::reserve_reg[k++], IR_name);
	  } 
	}
	//the parameter is constant
	else {
	  vector<string> split_number = reg_manage_obj->\
	    GetSplitSectionOfANum(each_parameter_name, reg_num);
	  //deliver the constant to the reserve_reg
	  for(int i = 0; i < split_number.size(); i++){
	    OutPut("movlw", split_number[i], IR_name);
	    OutPut("movwf", RegManage::reserve_reg[k++], IR_name);
	  }
	}
      }
    }
    //if VEC[2] == call dedicate that the fun have return value
    else if(!VEC[2].compare("call")){
      string op_des = VEC[0];
      string op_des_type = VEC[3];

      vector<string> parameter_name;
      vector<string> parameter_type;
      //get the parameter name and type
      GetParameterNameAndParameterType(wordCon,\
				       parameter_name,
				       parameter_type);
      //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	op_des = current_fun_name + "." + op_des;
      }

      reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
      vector<string> op_des_addr_vec = \
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
      //the index of the reserve_reg
      int k = 0;
      //deal with each parameter
      for(int i = 0; i < parameter_name.size(); i++){
	string each_parameter_name = parameter_name[i];
	string each_parameter_type = parameter_type[i];
	int reg_num = reg_manage_obj->HowBigType(parameter_type[i]);
	//the parameter is register
	if(regex_match(each_parameter_name, reg)){
	  //add fun name before variable
	  string current_fun_name =				\
	    reg_manage_obj->GetValueFromWhichFunStack();
	  if(!current_fun_name.empty()){
	    each_parameter_name =				\
	      current_fun_name + "." + each_parameter_name;
	  }
	  //get the parameter addr
	  vector<string> parameter_addr_vec = reg_manage_obj-> \
	    GetActualAddrFromGenVal(each_parameter_name, 0);

	  //deliver the parameter_addr to reserve_reg
	  for(int i = 0; i < parameter_addr_vec.size(); i++){
	    OutPut("movf", parameter_addr_vec[i], IR_name);
	    OutPut("movwf",RegManage::reserve_reg[k++], IR_name);
	  } 
	}
	//the parameter is constant
	else {
	  vector<string> split_number = reg_manage_obj->\
	    GetSplitSectionOfANum(each_parameter_name, reg_num);
	  //deliver the constant to the reserve_reg
	  for(int i = 0; i < split_number.size(); i++){
	    OutPut("movlw", split_number[i], IR_name);
	    OutPut("movwf", RegManage::reserve_reg[k++], IR_name);
	  }
	}
      }
      //change @funName to %funName
      string fun_name = wordCon.vaCol[4].substr(1);  
      OutPutJump("call", fun_name, IR_name);

      //receive the return value to op_des
      for(int i = 0; i < op_des_addr_vec.size(); i++){
	OutPut("movf", RegManage::reserve_reg[i], IR_name);
	OutPut("movwf", op_des_addr_vec[i], IR_name);
      }
      
    }

  }  
  //this is fun call and there aren't parameter to deliver
  else {
    //the fun no return value and parameter
    if(!VEC[0].compare("call")){
      //change @fun_name to fun_name
      string fun_name = VEC[2].substr(1);
      //call fun name
      OutPutJump("call", fun_name, IR_name);
     
    }
    //the fun have return value but not parameter
    else if(!VEC[2].compare("call")){
      string op_des = VEC[0];
      string op_des_type = VEC[3];
      //change @fun_name to fun_name
      string fun_name = VEC[4].substr(1);
      //RegManage *reg_manage_obj = RegManage::getInstance();
      //add fun name before variable
      string current_fun_name =				\
	reg_manage_obj->GetValueFromWhichFunStack();
      if(!current_fun_name.empty()){
	op_des = current_fun_name + "." + op_des;
      }

      reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);
      vector<string> op_des_addr_vec =				\
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
      //call fun_name
      OutPutJump("call", fun_name, IR_name);
      //receive the return value to op_des
      for(int i = 0; i < op_des_addr_vec.size(); i++){
	OutPut("movf", RegManage::reserve_reg[i], IR_name);
	OutPut("movwf", op_des_addr_vec[i], IR_name);
      }
    }
  }
  
#undef VEC
}

// and operator
//
void TranceAnd(SplitWord wordCon, string IR_name){
  CoreFunOfLogicOperator(wordCon, "andwf", "andlw", IR_name);
}

// or operator
//
void TranceOr(SplitWord wordCon, string IR_name){
  CoreFunOfLogicOperator(wordCon, "iorwf", "iorlw", IR_name);
}

// xor operator
//
void TranceXor(SplitWord wordCon, string IR_name){
  CoreFunOfLogicOperator(wordCon, "xorwf", "xorlw", IR_name);
}

// << operator
//
void TranceShl(SplitWord wordCon, string IR_name){
  CoreFunOfLogicOperator(wordCon, "rlncf", "rlncf", IR_name);
}

// >> operator
//
void TranceAshr(SplitWord wordCon, string IR_name){
  CoreFunOfLogicOperator(wordCon, "rrncf", "rrncf", IR_name);
}

//&& and || operator
//
void TrancePhi(SplitWord wordCon, string IR_name){

  RegManage* reg_manage_obj = RegManage::getInstance();
  
  string index_op = wordCon.vaCol[5]; 
  string op_des = wordCon.vaCol[0];
  string op_src1 = wordCon.vaCol[9];
  string op_src2 = wordCon.vaCol[10];

  string op_des_type = "i8";
  string op_src2_type = "i8";

  //add fun name before variable
  string current_fun_name =				\
    reg_manage_obj->GetValueFromWhichFunStack();
  if(!current_fun_name.empty()){
    op_src1 = current_fun_name + "." + op_src1;
    op_src2 = current_fun_name + "." + op_src2;
    op_des = current_fun_name + "." + op_des;
  }

  reg_manage_obj->AllocateRegToGenVal(op_des, op_des_type, 1);  
  vector<string> reg_des = \
    reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);
  int reg_num =						\
    reg_manage_obj->HowBigType(op_src2_type);
  string inner_label = wordCon.vaCol[0].substr(1);

  //Instr: for || operator
  if(!index_op.compare("true")){
    regex reg(".*\%.+");
    if(regex_match(op_src1, reg)){

      vector<string> op_src1_vec = \
	reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
      vector<string> op_src2_vec = \
	reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);
      
      vector<string> constant_0 = \
	reg_manage_obj->GetSplitSectionOfANum("0", reg_num);
      vector<string> constant_1 = \
	reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
      
      OutPut("movlw", "0", IR_name);
      for(int i = 0; i < reg_num; i++){
	OutPut("cpfseq", op_src2_vec[i], IR_name);
	OutPutJump("bra", inner_label + "_next1", IR_name);
      }
      OutPut("movlw", "0", IR_name);
      for(int  i = 0; i < op_src1_vec.size(); i++){
	OutPut("cpfseq", op_src1_vec[i], IR_name);
	OutPutJump("bra", inner_label + "_next1", IR_name);
      }
      
      OutPutJump("bra", inner_label + "_next2", IR_name);
      
      OutPutLabel(inner_label + "_next1", IR_name);
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", constant_1[i], IR_name);
	OutPut("movwf", reg_des[i], IR_name);
      }
      OutPutJump("bra", inner_label + "_next3", IR_name);

      OutPutLabel(inner_label + "_next2", IR_name);
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", constant_0[i], IR_name);
	OutPut("movwf", reg_des[i], IR_name);
      }

      OutPutLabel(inner_label + "_next3", IR_name);
    }
    else {
      if(!op_src1.compare("true")){
	
	vector<string> constant_1 = \
	  reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
	for(int i = 0; i < reg_num; i++){
	  OutPut("movlw", constant_1[i], IR_name);
	  OutPut("movwf", reg_des[i], IR_name);
	
	}
      }
      else if(!op_src2.compare("false")){
	
	vector<string> op_src2_addr = \
	  reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);

	vector<string> constant_1 =				\
	  reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
	
	OutPut("movlw", "0", IR_name);
	for(int i = 0; i < reg_num; i++){
	  OutPut("cpfseq", op_src2_addr[i], IR_name);
	  OutPutJump("bra", inner_label + "_next1", IR_name);
	  OutPut("movwf", reg_des[i], IR_name);
	}
	
	OutPutJump("bra", inner_label + "_next2", IR_name);
	
	OutPutLabel(inner_label + "_next1", IR_name);
	for(int i = 0; i < reg_num; i++){
	  OutPut("movlw", constant_1[i], IR_name);
	  OutPut("movwf", reg_des[i], IR_name);
	}
	OutPutLabel(inner_label + "_next2", IR_name);
      }

    }
  } 
  //Instr: for && operator
  else if(!index_op.compare("false")){ 
    regex reg(".*\%.+");
    if(regex_match(op_src1, reg)){

      vector<string> reg_op_src1 =				\
	reg_manage_obj->GetActualAddrFromGenVal(op_src1, 0);
      vector<string> reg_op_src2 = \
	reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);

      reg_manage_obj->AllocateRegToGenVal(op_des, "i8", 1);  
      vector<string> reg_des =					\
	reg_manage_obj->GetActualAddrFromGenVal(op_des, 0);

      vector<string> constant_1 =				\
	reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
      
      for(int i = 0; i < reg_num; i++){
	OutPut("movlw", constant_1[i], IR_name);
	OutPut("movwf", reg_des[i], IR_name);
      }

      OutPut("movlw", "0", IR_name);
      for(int i = 0; i < reg_op_src1.size(); i++){
	OutPut("cpfseq", reg_op_src1[i], IR_name);
	OutPutJump("bra", inner_label + "_next1", IR_name);
      }
      OutPutJump("bra", inner_label + "_next2", IR_name);
      
      OutPutLabel(inner_label + "_next1", IR_name);
      OutPut("movlw", "0", IR_name);
      for(int i = 0; i < reg_num; i++){
	  OutPut("cpfseq", reg_op_src2[i], IR_name);      
	  OutPutJump("bra", inner_label + "_next3", IR_name);
      }

      OutPutJump("bra", inner_label + "_next2", IR_name);
      OutPutLabel(inner_label + "_next2", IR_name);
      
      OutPut("movlw", "0", IR_name);
      for(int i = 0; i < reg_num; i++){
	OutPut("movwf", reg_des[i], IR_name);
      }
      OutPutLabel(inner_label + "_next3", IR_name);
    
    } 
    else {
      if(!op_src1.compare("false")){

	OutPut("movlw", "0", IR_name);
	for(int i = 0; i < reg_num; i++){
	  OutPut("movwf", reg_des[i], IR_name);
	}
      }
      else if(!op_src1.compare("true")){
	vector<string> op_src2_addr = \
	  reg_manage_obj->GetActualAddrFromGenVal(op_src2, 0);

	vector<string> constant_1 = \
	  reg_manage_obj->GetSplitSectionOfANum("1", reg_num);
	
	OutPut("movlw", "0", IR_name);
	for(int i = 0; i < reg_num; i++){
	  OutPut("cpfseq", op_src2_addr[i], IR_name);
	  OutPutJump("bra", inner_label + "_next1", IR_name);
	  OutPut("movwf", reg_des[i], IR_name);
	}
	OutPutJump("bra", inner_label + "_next2", IR_name);
	
	OutPutLabel(inner_label + "_next1", IR_name);
	for(int i = 0; i < reg_num; i++){
	  OutPut("movlw", constant_1[i], IR_name);
	  OutPut("movwf", reg_des[i], IR_name);
	}
        OutPutLabel(inner_label + "_next2", IR_name);
      }
    }
    
  }

}

//Instr for array.
//there are two places to store the element of array
//the first place is the register
//the other is the data area which in the programmer area
//
//
void TranceConstant(SplitWord wordCon, string IR_name){
#define  VEC wordCon.vaCol
  regex regular_expr_struct("%struct.*");

  //it is inter array  without constant
  if(find(VEC.begin(), VEC.end(), "private") !=	VEC.end() &&	  \
     find(VEC.begin(), VEC.end(), "unnamed_addr") != VEC.end() && \
     find(VEC.begin(), VEC.end(),  "constant") != VEC.end() &&	  \
     find(VEC.begin(), VEC.end(),  "x") !=  VEC.end()) {
    
    RegManage *reg_manage_obj = RegManage::getInstance();
    //get the elem num of this array
    int array_elem_num = GetArrayElemNum(wordCon);
    //get the elem type of this array
    string array_elem_type = GetArrayElemType(wordCon);
    //array name
    string array_name = wordCon.vaCol[0];
    //allocate addr to this array variable, but these addr will 
    //store in the variable together
    reg_manage_obj->AllocateRegToGenVal(array_name, \
					array_elem_type, \
					array_elem_num);
    //get the array addr that have benn allocated
    //vector<string> array_addr_vec =				    \
    // reg_manage_obj->GetActualAddrFromGenVal(wordCon.vaCol[0], 0);
    vector<string> array_addr_vec =		\
      reg_manage_obj->GetAllActualAddrFromGenVal(array_name);
    //get the array value from the IR instr
    vector<string> array_value_vec;
    getDataFromInstr(array_value_vec, wordCon);
    //get the type 
    int reg_num = reg_manage_obj->HowBigType(array_elem_type);
   
    if(array_value_vec.size() != 0 && array_addr_vec.size() != 0){
      int k = 0;
      for(int i = 0; i < array_value_vec.size(); i++){
	//get a value from array_value_vec, and split it according 
	//to the type of the elem
	vector<string> array_value_split =			   \
	  reg_manage_obj->GetSplitSectionOfANum(array_value_vec[i],\
						reg_num);
	
	for(int j = 0; j < array_value_split.size(); j++){
	  //  OutPut("movlw", array_value_split[j], IR_name);
	  //OutPut("movwf", "tablat", IR_name);
	  //OutPut("movlw", array_addr_vec[k].substr(0,2), IR_name);
	  //OutPut("movwf", "tblptrh", IR_name);
	  //OutPut("movlw", array_addr_vec[k].substr(2,4), IR_name);
	  //OutPut("movwf", "tblptrl", IR_name);
	  //OutPut("tblwr", "*", IR_name);
	  OutPut("movlw", array_value_split[j], IR_name);
	  OutPut("movwf", array_addr_vec[k], IR_name);
	  

	  ++k;
	}//end for
      }//end for
    } //end if
	
  }
  //it is inter struct type with having been initialized 
  else if(find(VEC.begin(), VEC.end(), "private") != VEC.end() &&   \
	  find(VEC.begin(), VEC.end(), "unnamed_addr") != VEC.end()&&\
	  find(VEC.begin(), VEC.end(), "constant")!= VEC.end() && \
	  regex_match(VEC[5], regular_expr_struct)){
    
    RegManage *reg_manage_obj = RegManage::getInstance();
    //struct variable name
    string struct_name = VEC[0];
    //the type of the struct
    string struct_type = VEC[5];

    //allocate addr for the struct
    reg_manage_obj->AllocateRegToGenVal(struct_name, 
					struct_type,
					999);
    //get all addr fo the struct_name that have been allocated
    vector<string> struct_addr =				\
      reg_manage_obj->GetAllActualAddrFromGenVal(struct_name);

    //get struct data from the IR instr
    vector<string> struct_value = GetStructValueFromIRInstr(wordCon);
    //get struct elem type 
    const vector<string>& elem_type = \
      reg_manage_obj->GetRecordVarDecalare(struct_type);
    int k = 0;

    for(int i = 0; i < struct_value.size(); i++){
      int reg_num = \
	reg_manage_obj->HowBigType(elem_type[i]);
      vector<string> split_elem = \
	reg_manage_obj->GetSplitSectionOfANum(struct_value[i], \
					      reg_num);

      for(int j = 0; j < reg_num; j++){
	OutPut("movlw", split_elem[j], IR_name);
	OutPut("movwf", struct_addr[k], IR_name);
	++k;
      }
    }
    //get the elem num of this array
    // int array_elem_num = GetArrayElemNum(wordCon);
    //get the 
  }
  //it is for const array with zeroinitialiaer
  else if(find(VEC.begin(), VEC.end(), "x") != VEC.end() && \
	  find(VEC.begin(), VEC.end(), "constant") != VEC.end() && \
	  find(VEC.begin(), VEC.end(), "zeroinitializer") != \
	  VEC.end()){

    //get how manay elem the array have
    int array_elem_num = GetArrayElemNum(wordCon);
    //get the type of the array
    string array_elem_type = GetArrayElemType(wordCon);
    DataAreaManage *data_area_manage_obj =	\
      DataAreaManage::getInstance();
    //store the begin addr of the array, we will put all the 
    //addr together and use the array name to identifiy
    data_area_manage_obj->AllocateDataAreaToVal(VEC[0],\
						array_elem_type, \
						array_elem_num);
    //Get actual all addr of the array
    vector<string> addr_name =					 \
      data_area_manage_obj->GetAllActualAddrFromVal(VEC[0]);
    int reg_num = data_area_manage_obj->HowBigType(array_elem_type);
    //store value to the addr
    //a array have many elem
    //a elem may own many addr
    if(addr_name.size() != 0){
      //k  is for the whole addr
      int k = 0;

      for(int i = 0; i < array_elem_num; i++){
	// string addr = ManageDataArea::getAddr_plus();
	vector<string> value_vec = \
	  data_area_manage_obj->GetSplitSectionOfANum("0", \
						      reg_num);
	for(int j = 0; j < value_vec.size(); j++){
	  OutPut("movlw", value_vec[j], IR_name);
	  OutPut("movwf", "tablat", IR_name);
	  OutPut("movlw", addr_name[k].substr(0,2), IR_name);
	  OutPut("movwf", "tblptrh", IR_name);
	  OutPut("movlw", addr_name[k].substr(2,4), IR_name);
	  OutPut("movwf", "tblptrl", IR_name);
	  OutPut("tblwr", "*", IR_name);
	  ++k;
	}
      }
    }
   
  }
  //inter constant array and global constant array, these variable
  //should be stored in the data area
  else if((find(VEC.begin(),VEC.end(), "internal") !=VEC.end() &&  \
	  find(VEC.begin(), VEC.end(), "constant") != VEC.end()&& \
	   find(VEC.begin(), VEC.end(), "x") != VEC.end())   ||	  \
	  (find(VEC.begin(),VEC.end(),"constant") != VEC.end() && \
	   VEC[1] == "=" && VEC[4] == "x")){

    //get how many elem the array have
    int array_elem_num = GetArrayElemNum(wordCon);
    //get the type of the array
    string array_elem_type = GetArrayElemType(wordCon);
    DataAreaManage *data_area_manage_obj =	\
      DataAreaManage::getInstance();
    //store the begin addr of the array, we will put all the 
    //addr together and use the array name to identifiy
    data_area_manage_obj->AllocateDataAreaToVal(wordCon.vaCol[0],\
						array_elem_type, \
						array_elem_num);
    vector<string> dataVec;
    //absort actual value from instr in wordCon 
    getDataFromInstr(dataVec, wordCon);
    
    //Get actual all addr of the array
    vector<string> addr_name = \
      data_area_manage_obj->GetAllActualAddrFromVal(wordCon.vaCol[0]);
    
    int reg_num = data_area_manage_obj->HowBigType(array_elem_type);
    
    //store value to the addr
    //a array have many elem
    //a elem may own many addr
    if(dataVec.size() != 0){
      //k  is for the whole addr
      int k = 0;
      for(int i = 0; i < dataVec.size(); i++){
	// string addr = ManageDataArea::getAddr_plus();
	vector<string> value_vec = \
	  data_area_manage_obj->GetSplitSectionOfANum(dataVec[i], \
						      reg_num);
	
	for(int j = 0; j < value_vec.size(); j++){
	  OutPut("movlw", value_vec[j], IR_name);
	  OutPut("movwf", "tablat", IR_name);
	  OutPut("movlw", addr_name[k].substr(0,2), IR_name);
	  OutPut("movwf", "tblptrh", IR_name);
	  OutPut("movlw", addr_name[k].substr(2,4), IR_name);
	  OutPut("movwf", "tblptrl", IR_name);
	  OutPut("tblwr", "*", IR_name);
	  ++k;
	}
      }
    }
  }
  //for const struct variable that have been initial
  //@conter = constant %struct.mystruct { i32 1, i8 97 }, align 4
  else if(find(VEC.begin(), VEC.end(), "constant") != VEC.end() && \
	  regex_match(VEC[3], regular_expr_struct)) {
    DataAreaManage *data_area_manage_obj = \
      DataAreaManage::getInstance();
    //struct variable name
    string struct_name = VEC[0];
    //the type of the struct
    string struct_type = VEC[3];
    //allocate addr for the struct, -1 parameter represent the 
    //type is struct, each elem have different type
    data_area_manage_obj->AllocateDataAreaToVal(struct_name, \
					   struct_type, \
					   -1);
    //get all addr of the struct_name that have been allocated
    vector<string> struct_addr = \
      data_area_manage_obj->GetAllActualAddrFromVal(struct_name);

    //get struct data from the IR instr
    vector<string> struct_value = GetStructValueFromIRInstr(wordCon);
    //get each elem type
    const vector<string>& elem_type = \
      data_area_manage_obj->GetRecordVarDecalare(struct_type);

    int k = 0;
    for(int i = 0; i < struct_value.size(); i++){
      int reg_num = \
	data_area_manage_obj->HowBigType(elem_type[i]);
      vector<string> split_elem = \
	data_area_manage_obj->GetSplitSectionOfANum(struct_value[i], \
					      reg_num);
      for(int j = 0; j < reg_num; j++){
	OutPut("movlw", split_elem[j], IR_name);
	OutPut("movwf", "tablat", IR_name);
	OutPut("movlw", struct_addr[k].substr(0,2), IR_name);
	OutPut("movwf", "tblptrh", IR_name);
	OutPut("movlw", struct_addr[k].substr(2,4), IR_name);
	OutPut("movwf", "tblptrl", IR_name);
	OutPut("tblwr", "*", IR_name);
	++k;
      }
    }
  }
  //for constant val
  else {
    
  }
#undef VEC 
}

void TranceType(SplitWord wordCon, string IR_name) {
#define VEC wordCon.vaCol

  regex regular_expr_struct("%struct.*");
  //record the type of the struct in record_ var_decalare_map
  if(find(VEC.begin(), VEC.end(), "type") != VEC.end() &&	\
     regex_match(VEC[0], regular_expr_struct)){
      
    stack<string> temp_store_type;
    for(int i = VEC.size() - 2; i >= 0; i--){
      if(VEC[i] == "{") break;
      temp_store_type.push(VEC[i]);
    }

    vector<string> res;
    while(!temp_store_type.empty()){
      res.push_back(temp_store_type.top());
      temp_store_type.pop();
    }
       
    string var_name = wordCon.vaCol[0];
    
    MemoryManage memory_manage_obj = MemoryManage();
   
    memory_manage_obj.SetRecordVarDecalare(var_name, res);
  }
  
#undef VEC
}

void TranceGetelementptr(SplitWord wordCon, string IR_name){
#define VEC wordCon.vaCol  
  
  regex regular_expr_struct("%struct.*");
  regex regular_expr_ir("%.*");
  //get elem addr from struct variable
  if(regex_match(VEC[4], regular_expr_struct)){
    //struct name
    string struct_name = VEC[6];
    //op_destination 
    string op_des = VEC[0];
    string op_des_type = VEC[9];
    //which elem addr that we should take
    int elem_index = ChangeHexToDec(wordCon.vaCol[10]);
    
    RegManage *reg_manage_obj = RegManage::getInstance();
    //the size of the elem that we need
    int reg_num =					\
      reg_manage_obj->HowBigType(op_des_type);
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      struct_name = current_fun_name + "." + struct_name;
      op_des = current_fun_name + "." + op_des;
    }

    DataStoreInfo core_info_op_src =			\
      reg_manage_obj->GetAllInfoFromGenVal(struct_name);
    //get elem_index addr of this struct variable
    vector<string> elem_addr_vec =				\
      reg_manage_obj->GetActualAddrFromGenVal(struct_name,elem_index);
    
    DataStoreInfo core_info;
    core_info.virtual_addr = core_info_op_src.virtual_addr;
    core_info.data_type = op_des_type;
    //put the actual addr into the addr of the op_des 
    for(int i = 0; i < elem_addr_vec.size(); i++){
      core_info.actual_addr.push_back(elem_addr_vec[i]);

    }
    reg_manage_obj->CreateMapForGenVal(op_des, core_info);

  }
  //get elem addr from array variable
  else if(find(VEC.begin(), VEC.end(), "x") != VEC.end()){
    string array_name = VEC[10];
    string op_des = VEC[0];
    string op_des_type = VEC[6].substr(0, VEC[6].size()-1);
    int elem_index = ChangeStrToDec(VEC[14]);

    RegManage *reg_manage_obj = RegManage::getInstance();
    
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      array_name = current_fun_name + "." + array_name;
      op_des = current_fun_name + "." + op_des;
    }

    DataStoreInfo core_info_op_src =			\
      reg_manage_obj->GetAllInfoFromGenVal(array_name);
    vector<string> elem_addr_vec =				   \
      reg_manage_obj->GetActualAddrFromGenVal(array_name,elem_index);

    DataStoreInfo core_info;
    //record the array name
    core_info.virtual_addr = core_info_op_src.virtual_addr;
    core_info.data_type = op_des_type;
    for(int i = 0; i < elem_addr_vec.size(); i++){
      core_info.actual_addr.push_back(elem_addr_vec[i]);
    }
    reg_manage_obj->CreateMapForGenVal(op_des, core_info);	
  }
  //get elem addr from a reg variable
  else if(regex_match(VEC[6], regular_expr_ir)){
    string op_src = VEC[6];
    string op_src_type = VEC[4];
    string op_des = VEC[0];
    int index_num = ChangeStrToDec(VEC[8]);

    RegManage* reg_manage_obj = RegManage::getInstance();
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_src = current_fun_name + "." + op_src;
      op_des = current_fun_name + "." + op_des;
    }
    
    int reg_num = reg_manage_obj->HowBigType(op_src_type);
    DataStoreInfo core_info_op_src =			\
      reg_manage_obj->GetAllInfoFromGenVal(op_src);
    //get the array name
    string array_name = core_info_op_src.virtual_addr;
    
    vector<string> op_src_addr = \
      reg_manage_obj->GetActualAddrFromGenVal(array_name, index_num);

    DataStoreInfo core_info;
    
    core_info.virtual_addr = array_name;
    core_info.data_type = op_src_type;
    for(int i = 0; i < op_src_addr.size(); i++){
      core_info.actual_addr.push_back(op_src_addr[i]);
    }
    reg_manage_obj->CreateMapForGenVal(op_des, core_info);
  }
    
}


void TranceBitcast(SplitWord wordCon, string IR_name){
#define VEC wordCon.vaCol
  regex regular_expr_struct("%struct.*");

  RegManage *reg_manage_obj = RegManage::getInstance();
  //for bitcast , array
  if(find(VEC.begin(), VEC.end(), "x") != VEC.end() &&
     find(VEC.begin(), VEC.end(), "to") != VEC.end()){

    string op_des = wordCon.vaCol[0];
    string op_src = wordCon.vaCol[6];

    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_src = current_fun_name + "." + op_src;
      op_des = current_fun_name + "." + op_des;
    }
    //actually, op_des is a brige between op_src and actual 
    //addr, so we don't need to allocated actual addr for it
    IR_var_correspond_map.insert(make_pair(op_des, op_src));
  }
  //deal with like "%4 = bitcat %struct.mystruct* %2 to i8*"
  else if(regex_match(VEC[3], regular_expr_struct) && 
	  find(VEC.begin(), VEC.end(), "to") != VEC.end()){
    string op_des = wordCon.vaCol[0];
    string op_src = wordCon.vaCol[4];

    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_src = current_fun_name + "." + op_src;
      op_des = current_fun_name + "." + op_des;
    }

    IR_var_correspond_map.insert(make_pair(op_des, op_src));
  }
}

void getDataFromInstr(vector<string> &dataVec, SplitWord wordCon){
  //typedef wordCon.vaCol vec;
  auto index = find(wordCon.vaCol.begin(), wordCon.vaCol.end(), "x");
  int num = GetArrayElemNum(wordCon);
  index += 2;
  
  //char array
  //we should deal the single character and int type data
  //like char example[5] = {'1', 'd', 100}
  if((*index)[0] == 'c'){ 
    string data = (*(index)).substr(2);
    for(int i = 0; i < data.size()-1; i++){
      
      if(data[i] == '\\'){
	string elem = data.substr(i+1, 2);
	i = i + 2;

	int elem_dec = ChangeHexToDec(elem);
       
	dataVec.push_back(to_string(elem_dec));
      } 
      else {
	string elem = data.substr(i, 1);
	if(elem >= "a" && elem <= "z") {
	  elem = to_string(data[i]);
	}
	else if(elem >= "A" && elem <= "Z"){
	  elem = to_string(data[i]);
	}
	dataVec.push_back(elem);
      }
    }

  } 
  //int array
  else {   

    index += 1;
    for(int i = 0; i < num; i++){
      if(i == num - 1){ //remove ']' in 8]
	string val = *index;
	int valSize = val.size();
	val = val.substr(0, valSize - 1);
	dataVec.push_back(val);
      } else {
	dataVec.push_back(*index);
	index += 2;
      }
    }
  }
}


int GetArrayElemNum(SplitWord wordCon){
  auto index = find(wordCon.vaCol.begin(), wordCon.vaCol.end(), "x");
  
  string temp(*(index - 1));
  
  string temp2(temp.substr(1));
  
  int num = ChangeStrToDec(temp2);
  return num;
}

string GetArrayElemType(SplitWord wordCon){
  auto index = find(wordCon.vaCol.begin(), wordCon.vaCol.end(), "x");
  
  string temp(*(index + 1));
  
  string temp2(temp.substr(0, temp.size()-1));
  
  return temp2;

}

vector<string>
GetStructValueFromIRInstr(SplitWord wordCon){
#define VEC wordCon.vaCol

  vector<string> res;
  regex regular_expr_type("i.*");
  int begin_index = 0;
  for(int i = 0; i < VEC.size(); i++){
    if(!VEC[i].compare("{")){
      begin_index = i;
    }
  }
  for(int j = begin_index+1; j < VEC.size(); j++){
    if(!VEC[j].compare("}")) break;
    if(regex_match(VEC[j], regular_expr_type)) continue;
    res.push_back(VEC[j]);
  }
      
  return res;
}

//the function get the parameter name and type when
//we transalate the call fun
void
GetParameterNameAndParameterType(SplitWord wordCon,		  \
				 vector<string>& parameter_name,   \
				 vector<string>& parameter_type){
#define VEC wordCon.vaCol
  //the IR instr is call a fun
  if(find(VEC.begin(), VEC.end(), "call") != VEC.end()){

    for(int i = 0; i < VEC.size(); i++){
      //befor the variable is signext
      //ex:  %14 = call i32 @max(i32 signext %12, i32 signext %13)
      if(strcmp(VEC[i].c_str(), "signext") == 0) {
	if(i+1 >= VEC.size()) return;
	string op_src = VEC[++i];
	string op_src_type = VEC[i - 2];
	
	parameter_name.push_back(op_src);
	parameter_type.push_back(op_src_type);
      }
    }
  }
  //the IR instr is define a fun
  else if(find(VEC.begin(), VEC.end(), "define") != VEC.end()){
    
    for(int i = 0; i < VEC.size(); i++){
      //befor the variable is signext
      //ex:  define i32 @max(i32 signext, i32 signext) #0 {
      if(strcmp(VEC[i].c_str(), "signext") == 0) {
	string op_src = " ";
	string op_src_type = VEC[i - 1];
	
	parameter_name.push_back(op_src);
	parameter_type.push_back(op_src_type);
      }
    }
  }
  
}

void TranceRet(SplitWord wordCon, string IR_name){
  string second_para = wordCon.vaCol[1];
  //if the fun not return value
  if(!second_para.compare("void")){
    return ;
  }
  
  string op_src = wordCon.vaCol[2];
  RegManage* reg_manage_obj = RegManage::getInstance();
  int reg_num = reg_manage_obj->HowBigType(second_para);
 
  regex reg("\%.+");
  //return a constant or a variable
  //the first is variable
  if(regex_match(op_src, reg)){
    //add fun name before variable
    string current_fun_name =				\
      reg_manage_obj->GetValueFromWhichFunStack();
    if(!current_fun_name.empty()){
      op_src = current_fun_name + "." + op_src;
    }
    
    vector<string> op_src_addr_vec = \
      reg_manage_obj->GetActualAddrFromGenVal(op_src, 0);
    for(int i = 0; i < reg_num; i++){
      OutPut("movf", op_src_addr_vec[i], IR_name);
      OutPut("movwf", RegManage::reserve_reg[i], IR_name);
    } 
  }
  //the fun return a constant
  else {
    vector<string> op_src_split_vec =			    	\
      reg_manage_obj->GetSplitSectionOfANum(op_src, reg_num);

    for(int i = 0; i < reg_num; i++){
      OutPut("movlw", op_src_split_vec[i], IR_name);
      OutPut("movwf", reg_manage_obj->reserve_reg[i], IR_name);
    }
  }  
}


