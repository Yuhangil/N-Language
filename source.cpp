#include <cstdio>
#include <iostream>
#include <regex>
#include <string>

using namespace std;


int main(int argc, char* argv[])
{
	size_t filesize;
	FILE* fp;
	string fuck;
	regex pattern("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(?=((은|는|이|가|을|를|와|에|\\(\\w*\\))\\s))");
	smatch m;
	if (argc < 2)
	{
		return 0;
	}
	fp = fopen(argv[1], "r");
	while (!feof(fp))
	{
		fuck += fgetc(fp);
	}
	if (regex_search(fuck, m, pattern))
	{
		for (auto& sm : m)
		{
			printf("%s", sm);
		}
	}

	return 0;
}