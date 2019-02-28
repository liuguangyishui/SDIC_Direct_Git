//Copyright 2019 Hust.
//Author: HYL
//Descriptor:

/* 

 */
#include <cstring>
#include "memory_manage.h"
#include "tool_fun.h"

stack<string> MemoryManage::call_level_stack;

unordered_map<string, int> MemoryManage::var_store_which_map;

unordered_map<string, vector<string>> MemoryManage::variable_decalare_map;

stack<string> MemoryManage::which_fun;

//caculate how many reg will need
int MemoryManage::HowBigType(string var_type){
  
  if(!var_type.compare("i8") || !var_type.compare("i8*")){
    return 1;
  } else if(!var_type.compare("i32") || !var_type.compare("i32*")){
    return 4;
  } else if(!var_type.compare("i64") || !var_type.compare("i64*")){
    return 8;
  }
  return 0;
}

//return the first element in call_level_stack
string MemoryManage::GetBelongWhatCallName(){
  if(GetSizeOfCallLevelStack() == 0){
    return " ";
  }
  return GetCallLevelStack();
}


//when a constant number cant't store in 8 bit reg, then we should
//split it and store it more than ont reg. Firstly, we change string
//type src_val_str to int type data.
vector<string> MemoryManage::GetSplitSectionOfANum(string src_val_str, int var_type){
  int src_val = ChangeStrToDec(src_val_str);
  vector<string> res;
  int temp = src_val;

  while(temp >= 256){
    int mod = temp % 256;
    temp /= 256;
    res.push_back(to_string(mod));
  }
  res.push_back(to_string(temp));
  //align the bit between res and reg number
  for(int i = res.size(); i < var_type; i++){
    res.push_back("0");
  }

  return res;
}


int MemoryManage::VarStoreWhichMap(string var_name){
  if(var_store_which_map.find(var_name) == var_store_which_map.end()){
    cout << "No this Variable!" << endl;
    abort();
  }
  int res = var_store_which_map.find(var_name)->second;
  return res;
}

void MemoryManage::SetVarStoreWhichMap(string var_name, int index_num){
  if(var_store_which_map.find(var_name) != var_store_which_map.end()){
    return ;
  }
  var_store_which_map.insert(make_pair(var_name, index_num));
}


void MemoryManage::SetRecordVarDecalare(string var_name,\
				   vector<string> type_vec){

  variable_decalare_map.insert(make_pair(var_name, type_vec));
}


const vector<string>&					\
MemoryManage::GetRecordVarDecalare(string var_name){
  //vector<string> &res =				\
    //  variable_decalare_map.find(var_name)->second;
  //return res;
  return variable_decalare_map.find(var_name)->second;
}

void MemoryManage::SetValueToWhichFunStack(string fun_name){
  which_fun.push(fun_name);
}

string MemoryManage::GetValueFromWhichFunStack(){
  string fun_name;
  if(which_fun.empty()){
    return fun_name;
  }
  fun_name =  which_fun.top();
  return fun_name;
}

void MemoryManage::PopValueFromWhichFunStack(){
  if(which_fun.empty()) return;
  which_fun.pop();
}
