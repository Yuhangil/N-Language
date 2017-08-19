#include "Data.h"

Dataset::Dataset()
{
	pattern[0] = "([a-zA-Z_��-�R][a-zA-Z_��-�R0-9]*)(?=((��|��|��|��|��|��|��|��|\\(\\w*\\))\\s))";
	pattern[1] = "[{}]";
	pattern[2] = "[()]";
	pattern[3] = "(����|�Ǽ�)";
	pattern[4] = "(��|��|��|��|��|��|��|��)";
	pattern[5] = "(����|��|����|����)";
}

void Dataset::input(char* filename)
{
	wifstream ifs(filename);
	while (getline(ifs, String))
		buffer += String;
	ifs.close();
}

void Dataset::solve(wstring Buffer)
{
	
}