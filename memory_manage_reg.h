//Copyright 2019 Hust.
//Author: HYL
//Descriptor:

/*This is a head file of manage register which contain allocating reg,
 *storing data and read data.
 */

#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>
#include "memory_manage.h"
#include "debug_info.h"
#include "tool_fun.h"

#ifndef _MEMORY_MANAGE_REG_H_
#define _MEMORY_MANAGE_REG_H_

using namespace::std;

class MemoryManage;

//we have total 512 registers
const int kTotalRegNum = 512;

//how many function parameter register have been used
static int have_use_fun_para_num = 0;


class RegManage: public MemoryManage{
 public:

  static RegManage* getInstance(){
    if(reg_manage_instance == NULL)
      reg_manage_instance = new RegManage(kTotalRegNum);
    
    return reg_manage_instance;
  }
  //the range of ram come from outer file,when the compier begin
  //run, we should initial the ram range firstly.
  void InitialRamRange(vector<string>&, string&);
  //It is core fun for allocate reg.It will first check whether
  //have enough reg to allocate.Then create a DataStoreInfo type
  //value.All operation is done in this value.
  DataStoreInfo CoreAllocateRegFun(string, string, int);

  //for general val, allocate reg to general val.then call 
  //CreateMapForGenVal to create a record in reg_addr_map
  void AllocateRegToGenVal(string, string, int);
  //since 2DArray need to record array size and elem size(ex:2 x 3), 
  //it should be deal with 
  void AllocateRegToGenVal_2DArray(string, string, int, int, int);

  //for fun para. allocate reg to every fun para.then call 
  //CreateMapForFunPara to create a record in fun_para_map
  void AllocateRegToFunPara(string, string, int);

  //for global val. allocate reg to global val. then call
  //CreateMapForGlobalVal to create a record in global_addr_map
  //global val is constant value.Never change in whole program.
  void AllocateRegToGlobalVal(string, string, int);
  void AllocateRegToGlobalVal_2DArray(string, string, int, int, int);
  //we should know that allocate reg for var and create map in
  //reg_addr_map is different.Thus CreateMap put the record
  //into the reg_addr_map, fun_para_map or global_addr_map
  void CreateMapForGenVal(string, DataStoreInfo);
  void CreateMapForFunPara(string, DataStoreInfo);
  void CreateMapForGlobalVal(string, DataStoreInfo);
  
  //some IR var like "%1" have same addr. Thus we should copy 
  //original value info to new value when we deal with this value.
  void CopyAMapForGenVal(string, DataStoreInfo);

  //These funs get the addr from map.The addr is the acutal addr of
  //reg.It is a vector struct.
  //int is which elem in this variable.
  //int == 0, reprensent it is a single variable
  //int != 0, reprensent the variable is array or struct 
  vector<string> GetActualAddrFromGenVal(string, int);
  vector<string> GetActualAddrFromGlobalVal(string, int);
  vector<string> GetActualAddrFromFunPara(string, int);
 
  vector<string> GetAllActualAddrFromGenVal(string);
  vector<string> GetAllActualAddrFromGlobalVal(string);

  //These fun get entry collection info from map.The type of the 
  //return value is DataStoreInfo.
  DataStoreInfo GetAllInfoFromGenVal(string);

  DataStoreInfo GetAllInfoFromGlobalVal(string);

  //This fun is used in deliver fun para.
  bool WhetherHaveFunPara();

  //add ptr info to ptr_info_map, 
  //the first parameter is the ptr name,
  //the second parameter is the ptr content
  //ex : p = &a;
  void AddPtrInfoToRecord(string, string);
  //get the ptr info from ptr_info_map;
  //the parameter is the ptr name
  //return the content of this ptr
  string GetPtrInfoFromRecord(string);
  //judge the name whether it is a ptr
  bool WhetherIsPtr(string);
  //it ptr operator, there are some additonal info
  //must to record
  void AddPtrAdditionalInfo(string, string);
  string GetPtrAdditionalInfo(string);
  bool WhetherPtrAdditionalInfo(string);

  bool WhetherPtrDeliverMap(string);
  void AddElemIntoPtrDeliverMap(string, vector<string>&);
  vector<string> GetElemFromPtrDeliverMap(string);

  vector<string> GetElemAddrFromPtrAddr(string, int);

  //check all operator type whether correspond each other
  bool CheckAllOperatorType(string, string);
  bool CheckAllOperatorType(string, string, string);

  void DeleteRecordInGenVal(string);
  
  //After we have delivered fun para, the fun para became useless.
  //So we should clear it.
  void ClearFunParaMap(string);
  
  //This fun get value from addr
  string GetValueFromAddr(string);

  string GetAvailableReg();
  
  string ReturnReg();
  static string reserve_reg_1;

  //reserve reg 
  static vector<string> reserve_reg;

 private:
  //prohibit create object
  RegManage(int);

  //single object
  static RegManage* reg_manage_instance;  
  //record how many reg have been used from little to big
  //static int have_used_for_gen_var;
  //record how many reg have been used from big to little
  //static int have_used_for_global_var;

  /*reg_addr_map stores the map between the virtual reg
   *like %x and actual reg like 0xAH
   */
  static unordered_map<string, DataStoreInfo> reg_addr_map;
  // for funcational parameter deliver
  static vector<DataStoreInfo> fun_para_map;
  // global variable addr map
  static unordered_map<string, DataStoreInfo> global_addr_map;
  // available_use_reg and have_used_reg record the status of
  // 512 registers.
  static vector<string> available_use_reg;
  static vector<string> have_used_reg;

  //this is for ptr info
  static unordered_map<string, string> ptr_info_map;
  static unordered_map<string, string> ptr_additional_info_map;

  //this if for ptr deliver map
  //because we have to store register addr to reg, but the size
  //of addr is 12 bits, the size of reg is only 8 bits. we just
  //store the first of the address.
  static unordered_map<string, vector<string>> ptr_deliver_map;
};



#endif 
