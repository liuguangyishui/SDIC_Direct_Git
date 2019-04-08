#include <vector>
#include <map>

#ifndef _ANALYSISIR_H_
#define _ANALYSISIR_H_

using namespace::std;

//This fun get ram and rom info from cfg file
//parameter string is the file path
//parameter vector<string>& and vector<string>& is the ram and rom info vec
//parameter string is the core name 
void GetRamAndRomInfo(string, vector<string>&, vector<string>&, string&);

//This fun deal with the ram and rom info by using class memory_manage_data
//and memory_manage_reg
//parameter vecotr<string>& and vector<string>& is the ram and rom info vec
//parameter string& is the core name
void DealWithRamAndRomInfo(vector<string>&, vector<string>&, string&);

//This fun deal with the special reg. we will use map to store the 
//special reg actual name and the virtual name
//map<string, string>& store the map
//string& is the special reg path name
//string& is the core name
void DealWithSpecialRegInfo(map<string, string>&, string&, string&);
//parameter string& is the name of .ll file
void OpenFileAndDeal(string &);

#endif
