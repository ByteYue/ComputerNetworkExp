#ifndef GLOBAL_H
#define GLOBAL_H

#include "Tool.h"
#include "NetworkService.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#define GBN_WINDOW_SIZE 4                   //窗口长度为4
#define SEQ_BIT 3                           //需要000-100来指示序号,比特数为3
#define MAX_SEQ (1 << SEQ_BIT)              //所有涉及序号的运算必须使用模2^3运算
extern  Tool *pUtils;						//指向唯一的工具类实例，只在main函数结束前delete
extern  NetworkService *pns;				//指向唯一的模拟网络环境类实例，只在main函数结束前delete

#endif