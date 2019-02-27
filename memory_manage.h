//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* This is a head file of memory manage. It is a super class of 
 * reg_memory_manage class and data_memory_manage class.
 */

#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>
#include <stack>

#ifndef _MEMORY_MANAGE_H_
#define _MEMORY_MANAGE_H_

using namespace::std;

/*DataStoreInfo record the allocating infomation In IR format.
 *the addr is like the %x. When allocation a acutal memory, we
 *should know the data type that decide how much memory should 
 *be allocate. And store in ROM(data area) or RAM(reg) and so on.
 */
struct DataStoreInfo {
  //virtual reg like %x
  string virtual_addr; 
  //i8, i32, i64
  string data_type;    
  //actual register that allocated
  vector<string> actual_addr;
  //what fun do the value belong, 
  //if the variable is global, then
  //the belong_fun_name is _global.
  string belong_which_fun_name;    
  //store in reg(1) or data area(2)
  int store_where_place;   
  //allocated how many reg(i8/i16)
  int actual_addr_num;     

};


class MemoryManage {
 public:
  //caculate how many reg will be needed
  int HowBigType(string);

  void DataTypeCheck(string);
  //Return the fun name that is being run now.
  string GetBelongWhatCallName();

  //split number that can be stored into 8 bit register. 
  vector<string> GetSplitSectionOfANum(string, int);
  
  //return the index that dedicated the var shtore which map
  int VarStoreWhichMap(string);
  //set the map
  void SetVarStoreWhichMap(string, int);

  //set record variable delear
  void SetRecordVarDecalare(string, vector<string>);
  //get record from the variable_decalare_mape
  const vector<string>&  GetRecordVarDecalare(string);
  
  string GetCallLevelStack(){
    return call_level_stack.top();
  }
  int GetSizeOfCallLevelStack(){
    return call_level_stack.size();
  }

 protected:
  //string stand for the var name;
  //DataStoreInfo stand for the concrete info of store addr
  //unordered_map<string, DataStoreInfo> correspond_map;

    
 private:
  //What is call level? It record the caller layer. When A call
  //B during run, then B's fun name will be put into call_level.
  //When the program come back from B, B'fun name will pop. It 
  //used to recycle the reg when a fun have been done.
  static stack<string> call_level_stack;

  //record var store which map
  //1 == reg_addr_map
  //2 == global_addr_map
  //3 == fun_para_map
  //4 == data_area_map
  static unordered_map<string, int> var_store_which_map;

  //this map record some data about variable when the variable
  //just decalare without initial. When we decalare a sturct
  //type but not decalare a variable. Thus we should record the 
  //type of this struct. These info will be used when decalare
  //a struct variable
  static unordered_map<string, vector<string>> variable_decalare_map;

  
};

#endif 
