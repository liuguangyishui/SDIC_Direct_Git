//Copyright 2019 Hust.
//Author: HYL
//Descriptor:
/* This file include important fundamental data.
 */

#include <unordered_map>
#include <vector>

#ifndef _KEY_DATA_H_
#define _KEY_DATA_H_

using namespace::std;

/* operateSet store the map between the instrName
 * and enum keyWord index
 */
static unordered_map<string,int> SDIC_operate_set \
  = {{"alloca", 1}, {"load", 2},
     {"store", 3},  {"ret",4},
     {"add",5},     {"sub", 6},
     {"fcmp",7},    {"icmp", 7},
     {"br", 8},     {"<label>", 9},
     {"global", 10},{"define", 11},
     {"call", 12},  {"and", 13},
     {"or", 14},    {"xor", 15},
     {"shl", 16},   {"ashr", 17},
     {"zext", 18},  {"phi", 19},
     {"constant", 20}, {"type", 21},
     {"getelementptr", 22},{"bitcast", 23},
     {"sext", 24}, {"trunc", 25},
     {"select", 26}};

/* keyWord mainly use for the switch statement in 
 * analysisIR.cpp which indicate which tranfrom fun
 * should be called.
 */
enum SDICKeyWord{
  knull,
  alloc,  //not use 'alloca',since it is a keyWord in c++
  load,
  store,
  ret,
  add,
  sub,
  
  fcmp,
  br,
  label,
  globa, //not use 'global',since it is a keyWord in c++
  defin, //not use 'define',since it is a keyWord in c++
  call,
  anda,  //not use 'and', since it is a keyWord in c++
  ora,   //not use 'or', since it is a keyWord in c++
  xora,  //not use 'xor', since it is a keyWord in c++

  shl,   //
  ashr,
  zext,  //expand 
  phi,
  constant,

  type, //for struct declare
  getelementptr,
  bitcast,
  sext, //extension the type from small to big 
  trunct, //not use 'trunc'. extension the type from big to small

  selectt,
};

/* splitWord stores all infomation of a single instructment
 * of IR.
 */
struct SplitWord{
  SDICKeyWord instrName = knull; //indicate which instr
  vector<string> opCol;      //store operator like %x
  vector<string> vaCol;      //store all word,splited by space
};


#endif
