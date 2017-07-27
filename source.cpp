#include <cstdio>
#include <iostream>
#include <regex>
#include <string>

using namespace std;


int main(int argc, char* argv[])
{
	FILE* fp;
	string str;
	regex pattern("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(?=((은|는|이|가|을|를|와|에|\\(\\w*\\))\\s))");
	smatch match;

	if (argc < 2)	{
		return 0;
	}
	fp = fopen(argv[1], "r");
	while (!feof(fp))	{
		str += fgetc(fp);
	}
	while (regex_search(str, match, pattern)) {
        cout << match[0] << "\n";
		str = match.suffix().str();
    }

	return 0;
}