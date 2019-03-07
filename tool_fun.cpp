//Copyright 2019 Hust
//Author: HYL
//Descriptor:

/*This file include some useful tool fun.
 */
#include <regex>
#include "tool_fun.h"

//Change dec number to hex number
string ChangeDecToHex(int dec_num){
  string res;
  while(dec_num > 15){
    int mod = dec_num % 16;
    dec_num /= 16;
    res = ox16_tool[mod] + res;
  }
  res = ox16_tool[dec_num] + res;
  return res;
}

//Change hex number to dec number
int ChangeHexToDec(string hex_str_val){
  int res = 0;
  
  for(int i = hex_str_val.size() - 1, j = 0; i >= 0; i--, j++){
    int val = hex_str_val[i] - '0';
    res += val * pow(10, j);
  }
  return res;

}

//add two string hex value, return string type hex value
string AddTwoStrHex(string str1, string str2){

  int maxLen = max(str1.size(), str2.size());
  if(str1.size() > str2.size()){
    int dif = maxLen - str2.size();
    for(int i = 0; i < dif; i++){
      str2 = "0" + str2;
    }
  } else if(str1.size() < str2.size()){
    int dif = maxLen - str1.size();
    for(int i = 0; i < dif; i++){
      str1 = "0" + str1;
    }
  }

  string res;
  int index = 0; //for addtional
  for(int i = maxLen - 1; i >= 0; i--){
    int num1 = ChangeHexToDec(to_string(str1[i]));
    int num2 = ChangeHexToDec(to_string(str2[i]));
    int add = num1 + num2 + index;
    index = 0;

    if(add > 15){
      index = 1;
      add -= 15;
    }
    res = ox16_tool[add] + res;
  }
  if(index != 0) res = ox16_tool[index] + res;
  return res;
}

//Change str value to dec value.
int ChangeStrToDec(string str_src){
  if(str_src.size() == 0 ) return 0;
  int src_num = str_src.size();
  int res = 0;
  regex reg_minus("^-.*");
  //the number is minus
  if(regex_match(str_src, reg_minus)){
    for(int i = src_num - 1, j = 0; i >= 1; i--, j++){
      int value = str_src[i] - '0';
      res += value * pow(10, j);
    }
    res = 0 - res;
  }
  //the number is positive
  else {
    for(int i = src_num - 1, j = 0; i >= 0; i--, j++){
      int value = str_src[i] - '0';
      res += value * pow(10, j);
    }
  }

  return res;

}
