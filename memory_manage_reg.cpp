//Copyright 2019 Hust.
//Author: HYL
//Descriptor:

/*

 */
#include <regex>
#include "debug_info.h"
#include "tool_fun.h"
#include "memory_manage_reg.h"


using namespace::std;

class DebugInfo;

RegManage* RegManage::reg_manage_instance = NULL;

unordered_map<string, DataStoreInfo> RegManage::reg_addr_map;

vector<DataStoreInfo> RegManage::fun_para_map;

unordered_map<string, DataStoreInfo> RegManage::global_addr_map;

vector<string> RegManage::available_use_reg;
vector<string> RegManage::have_used_reg;

vector<string> RegManage::reserve_reg = {"0x1100", "0x1101", 
					 "0x1102", "0x1103", 
					 "0x1104", "0x1105",
					 "0x1106", "0x1107"};

RegManage::RegManage(int total_reg){
  for(int i = 0; i < total_reg; i++){
    string res = "0X1" + ChangeDecToHex(i);
    available_use_reg.push_back(res);
  }
}

//CoreAllocateRegFun is the core of allocate addr
//var_name is the name of the variable
//var_type is the type of the variable
//elem_num is a index of how many elem in this var_name.
//if elem_num equal 999. it dedicate the
//variable is a struce type variable. 
//if it equal 1, it is dedicate this variable is single var.
//if the variabl is a array type, the elem_num represent 
//the size of the array or how manay elem in this array
DataStoreInfo RegManage::CoreAllocateRegFun(string var_name, \
					    string var_type, \
					    int elem_num) {
  DataStoreInfo core_info;
  //elem_num equal 999 represent the var_name own more than one
  //addr, but it not like array that all elem belong to same type.
  //Every elem in the var_name may have different type.YES, it is
  //a struct.
  if(elem_num == 999){
    //get all elem type about this struct 
    const vector<string>& type_vec = 			\
      this->GetRecordVarDecalare(var_type);
  
    //    DataStoreInfo core_info;
    core_info.virtual_addr = var_name;
    core_info.data_type = var_type;
    core_info.store_where_place = 1;
   
    //how many reg that need.
    //    core_info.actual_addr_num = reg_need_num;
    core_info.belong_which_fun_name = this->GetBelongWhatCallName();
    //addr debug info 
    DebugInfo debug_info_obj = DebugInfo();
    debug_info_obj.CreateAAddrDebugRecord(var_name);
    //the outer statement for is for array if the elem_num
    //bigger than 1 the inter for statement is for register 
    //of a variabel
    for(int i = 0; i < type_vec.size(); i++){
      int reg_num = this->HowBigType(type_vec[i]);
      //get reg and make record
      for(int j = 0; j < reg_num; j++){
	string allocated_reg = available_use_reg.back();
	available_use_reg.pop_back();

	core_info.actual_addr.push_back(allocated_reg);
	//add addr debug info into record
	debug_info_obj.AddAddrDebugInfoToRecord(var_name,\
						allocated_reg);
      }
    }

  }
  //it is for single variable. These variable just own one 
  //elem.
  //
  else {
    //check the type that decide how many reg need
    int reg_need_num = this->HowBigType(var_type);
    int total_addr_used_for_var = reg_need_num * elem_num;
    //check whether still have enough reg to allocate
    if(available_use_reg.empty() ||				\
       total_addr_used_for_var > available_use_reg.size()){
      cout << "No enough reg to allocate!" << endl;
      abort();
    }
  
    //    DataStoreInfo core_info;
    core_info.virtual_addr = var_name;
    core_info.data_type = var_type;
    core_info.store_where_place = 1;
    //how many reg that need.
    core_info.actual_addr_num = reg_need_num;
    core_info.belong_which_fun_name = this->GetBelongWhatCallName();
    //addr debug info 
    DebugInfo debug_info_obj = DebugInfo();
    debug_info_obj.CreateAAddrDebugRecord(var_name);
    
    
    //the outer statement for is for array if the elem_num
    //bigger than 1 the inter for statement is for register 
    //of a variabel
    for(int i = 0; i < elem_num; i++){
      //get reg and make record
      for(int j = 0; j < reg_need_num; j++){
	string allocated_reg = available_use_reg.back();
	available_use_reg.pop_back();
	
	core_info.actual_addr.push_back(allocated_reg);
	//add addr debug info into record
	debug_info_obj.AddAddrDebugInfoToRecord(var_name,\
						allocated_reg);
      }
    }
    //update availabel_use_reg
    //available_use_reg -= reg_need_num;
  }
  return core_info;
}

//allocate addr for var_name according to the type of var_type
//elem_num dedicated the variable whether a array or a single 
//variable. if it is a array, then we should allocate more than
//one addr fro the var_name according to the elem_num
//if elem_num equal 0, then we just create a racord in reg_addr_map
//but not allocate actual addr for var_name
void RegManage::AllocateRegToGenVal(string var_name, string var_type, int elem_num) {
  
  DataStoreInfo core_info;
  if(elem_num != 0){
    core_info = this->CoreAllocateRegFun(var_name, var_type, elem_num);
  }
  else if(elem_num == 0){
 
  }
  this->SetVarStoreWhichMap(var_name, 1);
  //create record in map
  this->CreateMapForGenVal(var_name, core_info);  
}

//get var_name addr from reg_addr_map. we also should dedicate 
//array or a single variable. So we use a elem_index to do it.
//if elem_index not equal 1, then we konw that it is a array 
//variable.Thus the elem_index is the index of the array that
//we ought to get
//var_name parameter is the name of variable like %2 or 
//struct.mystruct
//elem_index parameter is the index elem of the var_name, if
//var_name is struct type, then it will more than one elem.
//if var_name is a single variable, elem_index will equal 0
vector<string> 
RegManage::GetActualAddrFromGenVal(string var_name, int elem_index) {
  
  DataStoreInfo core_info = this->GetAllInfoFromGenVal(var_name);
  vector<string> addr = core_info.actual_addr;
  string type = core_info.data_type;
  vector<string> res;
  //this type is struct type, we will get the elem_index elem
  //in var_name
  regex which_type_struct("%struct.*");
  if(regex_match(type, which_type_struct)){
    
    const vector<string>& struct_elem_type_vec =	\
      this->GetRecordVarDecalare(type);
    int elem_base = 0;
    
    for(int i = 0; i < elem_index; i++){
      elem_base += this->HowBigType(struct_elem_type_vec[i]);
    }

    int reg_num = this->HowBigType(struct_elem_type_vec[elem_index]);
    for(int i = 0; i < reg_num; i++){
      res.push_back(addr[elem_base + i]);
    }
    return res;

  }
  //this type is array or single variable
  else {
    int type_num = this->HowBigType(type);
    for(int i = 0; i < type_num; i++){
      res.push_back(addr[elem_index * type_num + i]);
    }
  }
  return res;
}

vector<string> 
RegManage::GetAllActualAddrFromGenVal(string var_name){

  if(reg_addr_map.find(var_name) == reg_addr_map.end()){
    cout << "This var haven't actual addr!" << endl;
    abort();
  }
  DataStoreInfo core_info = this->GetAllInfoFromGenVal(var_name);
  return core_info.actual_addr;
}

DataStoreInfo RegManage::GetAllInfoFromGenVal(string var_name){

  if(reg_addr_map.find(var_name) == reg_addr_map.end()){
    cout << "This var don't store in the map!" << endl;
    abort();
  }
  DataStoreInfo core_info = reg_addr_map.find(var_name)->second;
  return core_info;
}

DataStoreInfo RegManage::GetAllInfoFromGlobalVal(string var_name){

  if(global_addr_map.find(var_name) == global_addr_map.end()){
    cout << "This var don't store in the map!" << endl;
    abort();
  }
  DataStoreInfo core_info = global_addr_map.find(var_name)->second;
  return core_info;
}

void RegManage::CopyAMapForGenVal(string var_name, \
				  DataStoreInfo core_info){
  if(reg_addr_map.find(var_name) != reg_addr_map.end()) {
    cout << "This var have stored in map!" << endl;
    return ;
  }
  
  core_info.virtual_addr = var_name;
  this->CreateMapForGenVal(var_name, core_info);
}
  
void RegManage::CreateMapForGenVal(string var_name, DataStoreInfo core_info){
  
  //store the core_info into reg_addr_map
  reg_addr_map.insert(make_pair(var_name, core_info));
}

void RegManage::CreateMapForFunPara(string var_name, DataStoreInfo core_info){
  fun_para_map.push_back(core_info);
}

bool RegManage::WhetherHaveFunPara(){
  return !fun_para_map.empty();
}

void RegManage::AllocateRegToFunPara(string var_name, \
				     string var_type, \
				     int elem_num = 1){

  DataStoreInfo core_info = \
    this->CoreAllocateRegFun(var_name, var_type, elem_num); 
  this->SetVarStoreWhichMap(var_name, 3);
  //create record in map
  this->CreateMapForFunPara(var_name, core_info);  
}

vector<string> RegManage::GetActualAddrFromFunPara(string var_name, \
						   int elem_index){
  
  if(fun_para_map.size() == 0){
    cout << "This var haven't actual addr!" << endl;
    abort();
  }
  DataStoreInfo core_info = \
    fun_para_map.back(); 
  vector<string> addr = core_info.actual_addr;
  return addr;  

}

void RegManage::ClearFunParaMap(string var_name){
  
}

//if elem_num equal 0, then we don't need to allocate actual
//addr to this variable. we just create a record in the map
void RegManage::AllocateRegToGlobalVal(string var_name, \
				       string var_type,
				       int elem_num){
  DataStoreInfo core_info;
  if(elem_num != 0){
    core_info= this->CoreAllocateRegFun(var_name, var_type, elem_num); 
  } 
  //create a recored in map, but not allocate addr
  else if(elem_num == 0){ 
    
  }
  this->SetVarStoreWhichMap(var_name, 2);
    //create record in map
  this->CreateMapForGlobalVal(var_name, core_info);  

}

void RegManage::CreateMapForGlobalVal(string var_name, \
				      DataStoreInfo core_info){
  //store the core_info into reg_addr_map
  global_addr_map.insert(make_pair(var_name, core_info));
}


//this fun get the addr that have been allocated to var_name
//---------
//var_name represents the variable name
//elem_index represents which element addr in this var_name will
//be get. Because array and struct type variable own more than
//one element.
vector<string> RegManage::GetActualAddrFromGlobalVal(string var_name,\
						     int elem_index){

  if(global_addr_map.find(var_name) == global_addr_map.end()){
    cout << "This var haven't actual addr!" << endl;
    abort();
  }
  //get core_info from global_addr_map via the name of variable
  DataStoreInfo core_info = global_addr_map.find(var_name)->second;
  //get all addr of the variable
  vector<string> addr = core_info.actual_addr;
  //get the type of the variable
  string type = core_info.data_type;
  //the element addr that we actual need
  vector<string> res;
  //this type is struct type
  regex which_type_struct("%struct.*");
  
  //if the variable type is struct
  if(regex_match(type, which_type_struct)){
    //the variable is struct, so we get all element type  of
    //it from record_var_decalare_map
    const vector<string>& struct_elem_type_vec = \
      this->GetRecordVarDecalare(type);
    int elem_base = 0;
    for(int i = 0; i < elem_index; i++){
      int type_num = this->HowBigType(struct_elem_type_vec[i]);
      elem_base += type_num;
    }
    int reg_num = this->HowBigType(struct_elem_type_vec[elem_index]);
    for(int i = 0; i < reg_num; i++){
      res.push_back(addr[elem_base + i]);
    }
    
    return res;
  }
  else {
    int type_num = this->HowBigType(type);
    const vector<string> &type_vec = this->GetRecordVarDecalare(type);
    
    vector<string> res;
    for(int i = 0; i < type_num; i++){
      res.push_back(addr[elem_index * type_num + i]);
    }	
  return res;  
  }
  
  return res;  
}

vector<string> \
RegManage::GetAllActualAddrFromGlobalVal(string var_name){

  if(global_addr_map.find(var_name) == global_addr_map.end()){
    cout << "This var haven't actual addr!" << endl;
    abort();
  }
  //get core_info from global_addr_map via the name of variable
  DataStoreInfo core_info = global_addr_map.find(var_name)->second;
  //get all addr of the variable
  return core_info.actual_addr;
}
bool RegManage::CheckAllOperatorType(string op_1, string op_2){
  int op_1_num = this->HowBigType(op_1);
  int op_2_num = this->HowBigType(op_2);
  return (op_1_num == op_2_num);
}

bool RegManage::CheckAllOperatorType(string op_1, string op_2,	\
				     string op_3){
  int op_1_num = this->HowBigType(op_1);
  int op_2_num = this->HowBigType(op_2);
  int op_3_num = this->HowBigType(op_3);
  
  return (op_1_num == op_2_num && op_2_num == op_3_num);
}

//delete a reacord in reg_addr_map and recycle the addr if this 
//var_name have been allocated addr
void RegManage::DeleteRecordInGenVal(string var_name){
  if(reg_addr_map.find(var_name) == reg_addr_map.end()){
    return ;
  }

  DataStoreInfo core_info = reg_addr_map.find(var_name)->second;
  vector<string> addr_vec = core_info.actual_addr;
  reg_addr_map.erase(var_name);
  //reuse the addr 
  if(!addr_vec.empty()){
    for(auto elem : addr_vec){
      available_use_reg.push_back(elem);
    }
  }
 
  DebugInfo debug_info_obj = DebugInfo();
  debug_info_obj.AddAdditionalDebugInfoToRecord(var_name, \
					  "!!! Have been recycle");

  
}
