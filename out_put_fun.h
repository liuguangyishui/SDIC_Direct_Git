//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* These funs is for output hint info.
 */

#include <iostream>

#ifndef _OUT_PUT_FUN_H_
#define _OUT_PUT_FUN_H_

using namespace::std;

//output file name
static string out_put_file_name(" ");

void OpenOutPutFile(string);

void CloseOutPutFile();

//print instr without the last index number
void OutPut(string, string, string IR_name); 

//print instr with the last index number
void OutPut(string, string, int, int, string IR_name);

//print the paramter, without additonal content. like cout
void OutPutOrg(string, string);

//print instr that jump to target instr
void OutPutJump(string, string, string IR_name);

//print label that use for jump statement
void OutPutLabel(string, string);

#endif
