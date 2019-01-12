#include "lexer.hpp"

std::vector<std::string> valueArray;
std::vector<int> tokenArray;

int StoreToken(std::ifstream &fileStream)	{
	std::string forValue;
	std::string forToken;
	std::string temp;
	std::stringstream stringStream;
	std::ofstream testStream;

	testStream.open("token.txt");
	stringStream << fileStream.rdbuf();
	forToken = stringStream.str();

	// 입력물을 변형
	for(int i = 0; i < (int)removePattern.size() - 1; ++i)	{
		forToken = std::regex_replace(forToken, removePattern[i], removedString[i]);
	}

	// 조사를 없애자(한번에 안 잡힘)
	forToken = std::regex_replace(forToken, removePattern[1], removedString[1]);

	forValue = forToken;

	for (int i = 0; i < (int)pattern.size(); ++i)	{
		forToken = std::regex_replace(forToken, pattern[i], " " + std::to_string(i + 1) + " ");
	}

	forToken = std::regex_replace(forToken, pattern[(int)pattern.size() - 1], " " + std::to_string((int)pattern.size()) + " ");

	forValue = std::regex_replace(forValue, removePattern[8], removedString[8]);

	// 혹시 나중에 최적화할 때 쓰세요

	for(int i = 0; i < (int)forValue.length(); ++i)	{
		while(forValue[i] != ' ' && forValue[i] != '\n' && forValue[i] != '\0')	{
			temp += forValue[i];
			++i;
		}

		if(!temp.empty())	{
			valueArray.push_back(temp);
		}
		temp.clear();
	}

	temp.clear();

	for(int i = 0; i < (int)forToken.length(); ++i)	{
		while(forToken[i] != ' ' && forToken[i] != '\n' && forToken[i] != '\0')	{
			temp += forToken[i];
			++i;
		}

		if(!temp.empty())	{
			tokenArray.push_back(stoi(temp));
		}
		temp.clear();
	}

	// 여기까지

	return 0;
}