#ifndef __DATA__
#define __DATA__
#include <cstdio>
#include <iostream>
#include <regex>
#include <string>
#include <locale>
#include <fstream>
#include <vector>

using namespace std;

enum token
{
	
};

class Dataset
{
private:
	regex pattern[10];
	wstring buffer;
	smatch match;
	wstring String;
public:
	Dataset();
	void input(char* filename);
	void solve(wstring Buffer);
};


#endif