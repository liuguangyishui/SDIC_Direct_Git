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

//change hex number to dex number.
int ChangeHexToDec(string);

//add two string hex value, return string type hex value
string AddTwoStrHex(string, string);

//change string number to dec number.
long long int ChangeStrToDec(string); // == getNum(string)

#endif
