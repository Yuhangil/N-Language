#ifndef __DATA__
#define __DATA__
#include <cstdio>
#include <cmath>
#include <vector>
#include <set>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include <regex>

using namespace std;

enum token
{
	eof = -1,
	def = -2,
	id = -3,
	number = -4,

};

class Dataset
{
private:
	regex pattern[4];
	wstring buffer;

public:
	Dataset();
	//void print();
	//void getTable();
	bool isId(wchar_t ch);
	int input(FILE* fp);
};


#endif