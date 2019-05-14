//Copyright 2019 Hust.
//Author: HYL
//Descriptor:

/*This file include some useful tool fun
 */


#ifndef _TOOL_FUN_H_
#define _TOOL_FUN_H_

#include <iostream>
#include <vector>
#include <cmath>
using namespace::std;


/*ox16_tool is some char of ox that use for change to ox
 */
static vector<string> ox16_tool = {"0", "1", "2", "3", 
				   "4", "5", "6", "7", 
				   "8", "9", "A", "B", 
				   "C", "D", "E", "F"};

//trance the dec number to hex number
string ChangeDecToHex(int);

//trance the dec number to hex number for data area
string ChangeDecToHexDataArea(int);

//change hex number to dex number.
int ChangeHexToDec(string);

//add two string hex value, return string type hex value
string AddTwoStrHex(string, string);

//change string number to dec number.
long long int ChangeStrToDec(string); // == getNum(string)

//deal with the float type
//store float 0x3FF19999A0000000, float* %3, align 4, !dbg !9
vector<string>& DealWithFloatTypeData(string, int);

//deal with the float type
//store float -2.334000e+08, float* %10, align 4, !dbg !16
vector<string>& DealWithDoubleTypeData(string, int);

#endif
