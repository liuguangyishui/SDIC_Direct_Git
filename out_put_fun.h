//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* These funs is for output hint info.
 */

#include <iostream>
#include <set>
#include <map>


#ifndef _OUT_PUT_FUN_H_
#define _OUT_PUT_FUN_H_

using namespace::std;

extern map<string, string> special_reg_vec;

//when we out put the instruct. different instr will have different
//number parameter. ex: movlw 0;  movwf 0x11fc 1. 
//we can distin with no parameter, one parameter and two parameter
static set<string> instr_name_no_para {"addlw", "andlw", "bra", 
    "call", "clrwdt", "goto", "iorlw", "movlb", "movlw", "nop", 
    "rcall", "reset", "retfie", "retlw", "return", "sleep", 
    "sublw", "tblrd", "xorlw", "tblwr"};
static set<string> instr_name_one_para {"clrf", "cpfseq", "movwf", 
    "setf", "tstfsz"  };
static set<string> instr_name_two_para {"addwf", "addwfc", "andwf", 
    "bcf", "bsf", "btfsc", "btfss", "comf", "decf", "decfsz", 
    "dcfsnz", "incf", "incfsz", "infsnz", "iorwf", "movf", "rlcf", 
    "rlncf", "rrcf", "rrncf", "subwf", "subwfb", "xorwf"};
//output file name
static string out_put_file_name(" ");

void OpenOutPutFile(string);

void CloseOutPutFile();

//print instr without the last index number
void OutPut(string, string, string IR_name); 

//print instr with the last index number
void OutPut(string, string, int, int, string IR_name);

//this fun just output origin info
void OutPutPure(string);

//this fun direct output string
void OutPutDirect(string);

//print the paramter, without additonal content. like cout
void OutPutOrg(string, string);

//print instr that jump to target instr
void OutPutJump(string, string, string IR_name);

//print instr that jump to target instr, but no "label_"
void OutPutJumpPure(string, string, string);

//print label that use for jump statement
void OutPutLabel(string, string);


#endif
