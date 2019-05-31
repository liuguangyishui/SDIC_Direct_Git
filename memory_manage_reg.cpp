//Copyright 2019 Hust.
//Author: HYL
//Descriptor:

/*

 */
#include <regex>
#include <fstream>
#include "debug_info.h"
#include "tool_fun.h"
#include "memory_manage_reg.h"


using namespace::std;

class DebugInfo;

RegManage* RegManage::reg_manage_instance = NULL;

unordered_map<string, DataStoreInfo> RegManage::reg_addr_map;

vector<DataStoreInfo> RegManage::fun_para_map;

unordered_map<string, DataStoreInfo> RegManage::global_addr_map;

unordered_map<string, string> RegManage::ptr_info_map;

unordered_map<string, string> RegManage::ptr_additional_info_map;

unordered_map<string, vector<string>> RegManage::ptr_deliver_map;

vector<string> RegManage::available_use_reg;
vector<string> RegManage::have_used_reg;

vector<string> RegManage::reserve_reg;
                                   /* = {"0x001", "0x002", 
					 "0x003", "0x004", 
					 "0x005", "0x007",
					 "0x007", "0x008"};
	                           */
RegManage::RegManage(int total_reg){
  //Reserve reg;
  /*  for(int i = 1; i <= 32; i++){
    string res = "0x" + ChangeDecToHex(i);
    RegManage::reserve_reg.push_back(res);
  }
  //RAMRange=0x001-0x04c
  for(int i = 33; i <= 76; i++){
    string res = "0x" + ChangeDecToHex(i);
    available_use_reg.push_back(res);
  }
  //RAMRange=0x100-0x1FF
  //RAMRange=0x200-0x2FF
  for(int i = 256; i <= 767; i++){
    string res = "0x" + ChangeDecToHex(i);
    available_use_reg.push_back(res);
  }
  //RAMRange=0xE51-0xE53
  for(int i = 3665; i <= 3667; i++){
    string res = "0x" + ChangeDecToHex(i);
    available_use_reg.push_back(res);
  }
  //RAMRange=0xF58-0xFFE
  for(int i = 3928; i <= 4094; i++){
    string res = "0x" + ChangeDecToHex(i);
    available_use_reg.push_back(res);
  }
  */
}

//InitialRamRange fun aim to inital General reg of ram.The general
//reg divide into different banks.But we just use a vector to store
//all genreal reg.Then we must print these general reg according to
//it's bank.We can find the index that which bank the general reg 
//belong to.
void 
RegManage::InitialRamRange(vector<string> &ram_range_vec, string &core_name){
  int vec_size = ram_range_vec.size();
  
  const int kReserveRegNum = 32;
  int reserve_reg_num = 0;
  
  for(int i = 0; i < vec_size; i++){
    string range_elem = ram_range_vec[i];
    auto comma_index = range_elem.find('-');
    string range_begin_str = range_elem.substr(0, comma_index);
    string range_end_str = range_elem.substr(comma_index+1);

    int range_begin_num = ChangeHexToDec(range_begin_str);
    int range_end_num = ChangeHexToDec(range_end_str);
      
    for(int i = range_begin_num; i <= range_end_num; i++){
      string reg = "0x" + ChangeDecToHex(i);

      if(reserve_reg_num++ < kReserveRegNum){
	RegManage::reserve_reg.push_back(reg);	
      } 
      else {
	RegManage::available_use_reg.push_back(reg);
      }
    }							
  }

  int reserve_reg_size = RegManage::reserve_reg.size();
  int available_use_reg_size = RegManage::available_use_reg.size();
  
  //print the map of actual addr and addr name about general reg
  //
  std::ofstream f_out;
  
  f_out.open(core_name + "_GeneralReg.inc", ios_base::out);
  
  if(!f_out.is_open()){
    throw \
      "Error:RegManage::InitialRamRange() core_name file create fail";
  }
  //which bank that the register belong
  string which_bank_index;

  //for reserve_reg
  for(int i = 0; i < reserve_reg_size; i++){
    //FFF. No 0x 
    string reg_name = reserve_reg[i].substr(2);
    if(which_bank_index.empty()){
      f_out << "cblock\t" << reserve_reg[i] << endl;      
      f_out << "\tReg" + reg_name << endl;
      //get the first letter of the reg_name;
      which_bank_index = reg_name.substr(0,1);
    }
    else {
      string which_bank_index_temp = reg_name.substr(0,1);
      //if both equal
      if(!which_bank_index.compare(which_bank_index_temp)){
	f_out << "\tReg" + reg_name << ";" << endl;
      }
      //both not equal
      else {
	f_out << "endc" << endl;
	f_out << "cblock\t" << reserve_reg[i] << endl;
	f_out << "\tReg" + reg_name << ";" << endl;
	which_bank_index = which_bank_index_temp;
      }
    }
  }
  //for available_use_reg
  for(int i = 0; i < available_use_reg_size; i++){
    //FFF. No 0x
    string reg_name = available_use_reg[i].substr(2);
    if(which_bank_index.empty()){
      f_out << "cblock\t" << available_use_reg[i] << endl;
      f_out << "\tReg" + reg_name << ";" << endl;
    }
    else {
      string which_bank_index_temp = reg_name.substr(0,1);
      //if both equal
      if(!which_bank_index.compare(which_bank_index_temp)){
	f_out << "\tReg" + reg_name << ";" << endl;
      }
      else {
	f_out << "endc" << endl;
	f_out << "cblock\t" << available_use_reg[i] << endl;
	f_out << "\tReg" + reg_name << ";" << endl;
	which_bank_index = which_bank_index_temp;
      }
    }
  }
  f_out << "endc" << endl;

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
    core_info.unique_index = 1;
    //how many reg that need.
    //    core_info.actual_addr_num = reg_need_num;
    core_info.belong_which_fun_name = this->GetBelongWhatCallName();
    //addr debug info 
    DebugInfo debug_info_obj = DebugInfo();
    debug_info_obj.CreateAAddrDebugRecord(var_name, \
					  var_type, \
					  type_vec);
    //the outer statement for is for array if the elem_num
    //bigger than 1 the inter for statement is for register 
    //of a variabel
    for(int i = 0; i < type_vec.size(); i++){
		debug_info_obj.AddAddrDebugInfoToRecord(var_name, \
			"element: " + to_string(i));
		debug_info_obj.AddAddrDebugInfoToRecord(var_name, \
			"type: " + type_vec[i]);//yzk 2019.4.28 添加了struct中的元素类型信息
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
      throw "No enough reg to allocate!";
    
    }
  
    //    DataStoreInfo core_info;
    core_info.virtual_addr = var_name;
    core_info.data_type = var_type;
    core_info.store_where_place = 1;
    core_info.unique_index = 1;
    //how many reg that need.
    core_info.actual_addr_num = reg_need_num;
    core_info.belong_which_fun_name = this->GetBelongWhatCallName();
    //addr debug info 
    DebugInfo debug_info_obj = DebugInfo();
    debug_info_obj.CreateAAddrDebugRecord(var_name, var_type);
    
    
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
void 
RegManage::AllocateRegToGenVal(string var_name, \
			       string var_type, \
			       int elem_num) {
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
//for two dimension array
void 
RegManage::AllocateRegToGenVal_2DArray(string var_name, \
				       string var_type, \
				       int elem_num,
				       int first_size,
				       int second_size) {
  DataStoreInfo core_info;
  if(elem_num != 0){
    core_info = this->CoreAllocateRegFun(var_name, var_type, elem_num);
    //record the 2D array size and elemnt size
    //    core_info.2DArray_first = first_size;
    //    core_info.2DArray_second = second_size;
    //addr debug info 
    DebugInfo debug_info_obj = DebugInfo(); 
    debug_info_obj.AddAddrDebugInfoToRecord_2DArray(var_name, \
						    first_size,\
						    second_size);
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
    throw "This var haven't actual addr!";
    
  }
  DataStoreInfo core_info = this->GetAllInfoFromGenVal(var_name);
  return core_info.actual_addr;
}

DataStoreInfo RegManage::GetAllInfoFromGenVal(string var_name){

  if(reg_addr_map.find(var_name) == reg_addr_map.end()){
    throw "This var don't store in the map!";
    
  }
  DataStoreInfo core_info = reg_addr_map.find(var_name)->second;
  return core_info;
}

DataStoreInfo RegManage::GetAllInfoFromGlobalVal(string var_name){

  if(global_addr_map.find(var_name) == global_addr_map.end()){
    throw "This var don't store in the map!";
    
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
    throw "This var haven't actual addr!";
    
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
    throw "This var haven't actual addr!";
    
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
    throw "This var haven't actual addr!";
   
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

void 
RegManage::AddPtrInfoToRecord(string ptr_name, string ptr_content){
  if(!ptr_name.empty() && !ptr_content.empty()){
    ptr_info_map.insert(make_pair(ptr_name, ptr_content));
  }
}

string 
RegManage::GetPtrInfoFromRecord(string ptr_name){
  string res;
  if(!ptr_name.empty()){
    if(ptr_info_map.find(ptr_name) != ptr_info_map.end()){
      res = ptr_info_map.find(ptr_name)->second;
    }
  }
  return res;
}

bool
RegManage::WhetherIsPtr(string var_name){
  if(var_name.empty()) return false;
  return (ptr_info_map.find(var_name) != ptr_info_map.end());
}

void
RegManage::AddPtrAdditionalInfo(string ptr_name, string ptr_content){
  if(!ptr_name.empty() && !ptr_content.empty()){
    ptr_additional_info_map.insert(make_pair(ptr_name, ptr_content));
  }
}

string 
RegManage::GetPtrAdditionalInfo(string ptr_name){
  string res;
  if(!ptr_name.empty()){
    if(ptr_additional_info_map.find(ptr_name) != \
       ptr_additional_info_map.end()){
      res = ptr_additional_info_map.find(ptr_name)->second;
    }
  }
  return res;
}

bool
RegManage::WhetherPtrAdditionalInfo(string var_name){
  if(var_name.empty()) return false;
  return (ptr_additional_info_map.find(var_name) != \
	  ptr_additional_info_map.end());
}

bool
RegManage::WhetherPtrDeliverMap(string ptr_front_elem){
  return ptr_deliver_map.find(ptr_front_elem) != ptr_deliver_map.end();
}

void 
RegManage::AddElemIntoPtrDeliverMap(string ptr_front_elem, 
				    vector<string>& ptr_vec){
  if(ptr_front_elem.empty() || ptr_vec.size() == 0){
    throw "Error: AddElemIntoPtrDeliverMap() The ptr data error!";
  }
  //update the value of ptr_front_elem 
  if(ptr_deliver_map.find(ptr_front_elem) != ptr_deliver_map.end()){
    ptr_deliver_map.erase(ptr_front_elem);
  }
  ptr_deliver_map.insert(make_pair(ptr_front_elem, ptr_vec));
  
}

vector<string> 
RegManage::GetElemFromPtrDeliverMap(string ptr_front){
  if(ptr_deliver_map.find(ptr_front) == ptr_deliver_map.end()){
    throw "Error: GetElemFromPtrDeliverMap() The ptr data error!";
  }

  vector<string> res = \
    ptr_deliver_map.find(ptr_front)->second;
  return res;
}

vector<string> 
RegManage::GetElemAddrFromPtrAddr(string ptr_addr, \
				  int elem_index){
  vector<string> res;
  //repair the font addr from two addr
  //string font_addr = "0x" + ptr_addr[1].substr(3) +	\
  //  ptr_addr[0].substr(2);
  string font_addr = ptr_addr;
  //iterater all variable in reg_addr_map to find the font addr 
  //belong which variable
  for(auto elem : reg_addr_map){
    DataStoreInfo &all_info = elem.second;
    vector<string> addr_vec = all_info.actual_addr;
    string type = all_info.data_type;
    int elem_size = this->HowBigType(type);
    string name =  all_info.virtual_addr;

    for(int i = 0; i < addr_vec.size(); i++){
      string addr_elem = addr_vec[i];
      //if we found the font addr belong which variable
      if(!font_addr.compare(addr_elem) && all_info.unique_index == 1){
      
	int begin = i + elem_index * elem_size;
	
	if(begin >= addr_vec.size() * elem_size) {
	  throw "Error! GetElemAddrFromPtrAddr() ";
	
	}
	
	for(int j = 0; j < elem_size; j++){
	  res.push_back(addr_vec[begin + j]);
	}
	return res;
      }
    }
  }

  for(auto elem : global_addr_map){
    DataStoreInfo &all_info = elem.second;
    vector<string> addr_vec = all_info.actual_addr;
    string type = all_info.data_type;
    int elem_size = this->HowBigType(type);
    string name = all_info.virtual_addr;
    
    for(int i = 0; i < addr_vec.size(); i++){
      string addr_elem = addr_vec[i];
      // cout << "name " << name << addr_elem << " " << font_addr << all_info.unique_index << endl;
      //if we found the font addr belong which variable
      if(!font_addr.compare(addr_elem) && all_info.unique_index == 1){
     
	int begin = i + elem_index * elem_size;
	if(begin >= addr_vec.size() * elem_size) {
	  throw "Error! GetElemAddrFromPtrAddr() ";
    
	}
	for(int j = 0; j < elem_size; j++){
	  res.push_back(addr_vec[begin + j]);
	}
	return res;
      }

    }
  }
  if(res.size() == 0) { 
    throw "Error! The ptr not data";
  }
  return res;
}
