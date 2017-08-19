#include <cstdio>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

vector<wstring> Buffer;

int main(int argc, char* argv[])	{
	FILE* fp;
	wstring String;
	size_t FileSize;
	regex idPattern("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(?=((은|는|이|가|을|를|와|에|\\(\\w*\\))\\s))");
	regex bracePattern("[{}]");
	regex bracketPattern("[()]");
	regex NumberPattern("(정수|실수)");
	regex WordingPattern("(은|는|이|가|을|를|와|에)");
	regex OperatorPattern("(더한|뺀|곱한|나눈)");
	smatch Match;

	if (argc < 2)	{
		return 0;
	}
	
	fp = fopen(argv[1], "r");
	fseek(fp, 0L, SEEK_END);
	FileSize = ftell(fp);
	fgets(String, FileSize, fp);
	fseek(fp, 0L, SEEK_SET);

		while (regex_search(str, Match, idPattern)) {
        	cout << Match[0] << "\n";
			Buffer.push_back(Match.suffix().str());
		}

	return 0;
}