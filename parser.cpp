#include "parser.hpp"

std::vector<std::string> valueArray;
std::vector<int> tokenArray;

int main()	{
	GetToken();
}

int GetToken()	{
	std::ifstream file("data.txt");
	std::string original;
	std::string temporary;
	std::string temp;
	std::stringstream stringStream;
	stringStream << file.rdbuf();
	temporary = stringStream.str();

	temporary = std::regex_replace(temporary, removePattern[0], std::string("$1"));
	temporary = std::regex_replace(temporary, removePattern[1], std::string("$1 $2"));
	temporary = std::regex_replace(temporary, removePattern[2], std::string("$1"));
	temporary = std::regex_replace(temporary, removePattern[3], std::string(" $1 "));
	temporary = std::regex_replace(temporary, removePattern[4], std::string(" "));

	original = temporary;

	for (int i = 0; i < patternSize; ++i)	{
		temporary = std::regex_replace(temporary, pattern[i], std::to_string(i + 1) + " ");
	}

	for(int i = 0; i < original.length(); ++i)	{
		while(original[i] != ' ' && original[i] != '\n' && original[i] != '\0')	{
			temp += original[i];
			++i;
		}

		if(!temp.empty())	{
			valueArray.push_back(temp);
		}
		temp.clear();
	}

	for(int i = 0; i < temporary.length(); ++i)	{
		while(temporary[i] != ' ' && temporary[i] != '\n' && temporary[i] != '\0')	{
			temp += temporary[i];
			++i;
		}

		if(!temp.empty())	{
			tokenArray.push_back(stoi(temp));
		}
		temp.clear();
	}

	return 0;
}