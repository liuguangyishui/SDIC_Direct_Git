//Copyright 2019 Hust.
//Author: HYL
//Descriptor:

/*This is a head file of manage Data area
 */

#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>
#include "memory_manage.h"

#ifndef _MEMORY_MANAGE_DATA_H_
#define _MEMORY_MANAGE_DATA_H_

using namespace::std;

class MemoryManage;


class DataAreaManage : public MemoryManage {
 public:

  static DataAreaManage* getInstance() { //only one instance
    if(data_manage_instance == NULL)
      data_manage_instance = new DataAreaManage();
    return data_manage_instance;
  }
  
  //Core fun to allocate data area to val.
  DataStoreInfo CoreAllocateDataAreaFun(string, string, int);
  //allocate data area to val, then call CreateDataAreaMapForVal
  //to create map in data_area_addr_map
  //the first parameter: is the variable name
  //the second parameter: is the type of this variable name, now we
  //just think this type is for array. if the type is struct, then
  //there are more than one type.
  //the third parameter: is the elem number of this variable
  void AllocateDataAreaToVal(string, string, int);
  //put DataStoreInfo data into data_area_addr_map
  void CreateDataAreaMapForVal(string, DataStoreInfo);
  //get actual addr via para str
  vector<string> GetActualAddrFromVal(string, int);
  vector<string> GetAllActualAddrFromVal(string);
  //since difference type own difference number reg,
  //now we want to get a whole addr from vector<string>
  //addr.Return string result.
  string IntegrateVectorAddr(vector<string>&);
  
  vector<string> SplitStringAddrToVector(string);
  //GetValueFromGiveDataAreaAddr is get the actual value(not addr
  //name) from given data area. GetActualAddrFromVal is get the
  //actual addr from data area
  void GetValueFromGivenDataAreaAddr(string, int);
  
  DataStoreInfo GetAllInfoFromVal(string);

 private:
  DataAreaManage(); //prohibit create object

  static DataAreaManage* data_manage_instance; //single object

  //---
  //string initalAddr("3E00H"); //begin addr of data area
  //string endAddr("3FFFH");    //end addr of data area
  static long int begin_addr_dataArea;// = 15872;
  static long int end_addr_dataArea;// = 16384;
  
  //the total size of data area
  const int kTotalArea = 512;

  //available used data area addr
  static int available_data_area_addr;// = 512;

  //how many addr have been used
  static int have_used_data_area_addr;// = 0;

  /*data_addr_map stores the map between the virtual reg
   *like constant array and actual data area like 3E00H
   */
  static unordered_map<string, DataStoreInfo> data_area_addr_map;

   
};


#endif
