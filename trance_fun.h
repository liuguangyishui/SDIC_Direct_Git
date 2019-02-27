//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* This file include all trance fun that dedicate how to trance instr
 */

#include <iostream>
#include "memory_manage_data.h"
#include "memory_manage_reg.h"
#include "key_data.h"

#ifndef _TRANCE_FUN_H_
#define _TRANCE_FUN_H_

using namespace::std;

/*During the transform of some statement like the 
 *control statement in tranceBr fun, we should use 
 *the last instr infomation
 */
static string lastInstrName(" ");

/*record how many parameter have been deliver.
 *for the fun getRegValue_funPara
 */
static int indexNumPara = 0;

/*record a fun have how many parameter
 */
static int numPara = 0;

/*firstFun is used for main fun*/
static bool firstFun = true;

//In IR instr there are same addr between two different IR_var_name
//EX: %4 and %3 have same actual addr.
//we record the message, that we just need to allocate
static unordered_map<string, string> IR_var_correspond_map;



//clear some special comma in the singleLine
void ChangeComma(string&); 

//the beginning of program, deal with the enter and inerrupt
void ProgramBegin();

//the interrupt of the program
void ProgramHint();

//core code of add, and, xor and other
void coreAdd_And(SplitWord, string, string);

//for load instr
void TranceLoad(SplitWord, string);   

//for store instr
void TranceStore(SplitWord, string);  

//for alloca instr
void TranceAlloca(SplitWord, string); 

//for add instr
void TranceAdd(SplitWord, string);    

//for sub instr
void TranceSub(SplitWord, string);    

//for cmp instr
void TranceFcmp(SplitWord, string);   

//for jump instr
void TranceBr(SplitWord, string);     

//for zext instr
void TranceZext(SplitWord, string);

//for label instr
void TranceLabel(SplitWord, string);  

//for global var
void TranceGlobal(SplitWord, string);

//for fun 
void TranceDefine(SplitWord, string);
 
//for call
void TranceCall(SplitWord, string);

//for and
void TranceAnd(SplitWord, string);

//for or
void TranceOr(SplitWord, string);

//for xor
void TranceXor(SplitWord, string);

//for shl(不带进位的向左移位)
void TranceShl(SplitWord, string);

//for ashr(不带进位的向右移位)
void TranceAshr(SplitWord, string);

//for phi
void TrancePhi(SplitWord, string);

//for constant
void TranceConstant(SplitWord, string);


//fro struct
void TranceType(SplitWord, string);

void TranceGetelementptr(SplitWord, string);

void TranceBitcast(SplitWord, string);

void getDataFromInstr(vector<string> &, SplitWord);

//get array elem num
int GetArrayElemNum(SplitWord);


string GetArrayElemType(SplitWord);

//change string to int 
int getNum(string);

//get struct value from IR instr
vector<string> GetStructValueFromIRInstr(SplitWord);

//for ret instr
void TranceRet(SplitWord, string);

#endif
