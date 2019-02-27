//Copyright 2019 Hust.
//Author: HYL
//Descriptor:

/*

 */

#include "debug_info.h"
#include "tool_fun.h"
#include "memory_manage_data.h"

using namespace::std;

class DebugInfo;

DataAreaManage* DataAreaManage::data_manage_instance = NULL;

long int DataAreaManage::begin_addr_dataArea = 15872;

long int DataAreaManage::end_addr_dataArea = 16384;

int DataAreaManage::available_data_area_addr = 512;

int DataAreaManage::have_used_data_area_addr = 0;

unordered_map<string, DataStoreInfo> \
 DataAreaManage::data_area_addr_map;


DataAreaManage::DataAreaManage(){
  
}

//var_name is the variable name, var_type is the type of the variable
//elem_num is used for the array.Because we just record the first 
//elem addr of the array as it's whole addr. elem_num record how many
//elem the array have. if the vriable is not array, the value of the 
//elem num will be 1.
DataStoreInfo 
DataAreaManage::CoreAllocateDataAreaFun(string var_name, \
					string var_type, \
					int elem_num){
  //check the type that decide how many data area need
  int data_area_need_num = this->HowBigType(var_type);
  int total_addr_used_for_var = data_area_need_num * elem_num;
  //check whether still have enough data area to allocate
  if(available_data_area_addr <= 0 || \
     total_addr_used_for_var > available_data_area_addr){
    cout << "No enough data area to allocate!" << endl;
    abort();
  }
  
  DataStoreInfo core_info;
  core_info.virtual_addr = var_name;
  core_info.data_type = var_type;
  core_info.store_where_place = 2;
  //how many reg.
  core_info.actual_addr_num = data_area_need_num; 
  //record addr debug infomation
  DebugInfo debug_info_obj = DebugInfo();
  debug_info_obj.CreateAAddrDebugRecord(var_name);
  this->SetVarStoreWhichMap(var_name, 4);

  //the outer statement for is for array, 
  //the inter for statement if for register of a variable
  for(int i = 0; i < elem_num; i++){
    //get data area addr and make record
    for(int j = 0; j < data_area_need_num; j++){
      int data_area_addr_int = have_used_data_area_addr + \
	begin_addr_dataArea;
      string data_area_addr_str = ChangeDecToHex(data_area_addr_int);
      
      core_info.actual_addr.push_back(data_area_addr_str);
      //add data area debug info into record
      debug_info_obj.AddAddrDebugInfoToRecord(var_name,\
					      data_area_addr_str);
    
      //updata data area addr status
      ++have_used_data_area_addr;
      --available_data_area_addr;
    }
  }
  
  return core_info;
  
}//end 

//addr_num is for array. we just record the first elem addr of a 
//array when we translate array
void DataAreaManage::AllocateDataAreaToVal(string var_name, string var_type, int elem_num){
 
  DataStoreInfo core_info = this->CoreAllocateDataAreaFun(var_name, \
							  var_type, \
							  elem_num);
  this->CreateDataAreaMapForVal(var_name, core_info);
}

void DataAreaManage::CreateDataAreaMapForVal(string var_name, DataStoreInfo core_info){
  //store the core_info into data_area_addr_map
  data_area_addr_map.insert(make_pair(var_name, core_info));
}

//get addr from data_area_addr_map via var_name
vector<string> DataAreaManage::GetActualAddrFromVal(string var_name, \
						    int elem_index){
  
  if(data_area_addr_map.find(var_name) == data_area_addr_map.end()){
   abort();
  }
  DataStoreInfo core_info = data_area_addr_map.find(var_name)->second;
  vector<string> addr = core_info.actual_addr;
  string type = core_info.data_type;
  int type_num = this->HowBigType(type);
  
  vector<string> res;
  for(int i = 0; i < type_num; i++){
    res.push_back(addr[elem_index * type_num + i]);
  }
  return res;
}

vector<string> \
DataAreaManage::GetAllActualAddrFromVal(string var_name){
   if(data_area_addr_map.find(var_name) == data_area_addr_map.end()){
   abort();
  }
  DataStoreInfo core_info = data_area_addr_map.find(var_name)->second;
  return core_info.actual_addr;
}

string DataAreaManage::IntegrateVectorAddr(vector<string>& actual_addr){
  int addr_num = actual_addr.size();
  
}
void DataAreaManage::GetValueFromGivenDataAreaAddr(string var_name, int addr_num){

  
}

vector<string> DataAreaManage::SplitStringAddrToVector(string var_name){

  
  
}
