#include "Data.h"

Dataset::Dataset()
{
	pattern[0] = "([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(?=((은|는|이|가|을|를|와|에|\\(\\w*\\))\\s))";
	pattern[1] = "[{}]";
	pattern[2] = "[()]";
	pattern[3] = "(정수형|실수)";
	pattern[4] = "(은|는|이|가|을|를|와|에)";
	pattern[5] = "(더한|뺀|곱한|나눈)";
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
