#include "Lexer.hpp"

std::vector<std::string> valueArray;
std::vector<int> tokenArray;

int LexicalAnalysis(std::string &inputString)	{
	std::string forValue;
	std::string forToken;
	std::string temp;
	std::ofstream tStream;
	std::ofstream vStream;

	tStream.open("token.txt");
	vStream.open("value.txt");

    forToken = inputString;

	for(int i = 0; i < (int)removePattern.size(); ++i)	{
		if(i == (int)removePattern.size() - 2)	{
			continue;
		}
		forToken = std::regex_replace(forToken, removePattern[i], removedString[i]);
	}

	forValue = forToken;

	tStream << forToken;

	for (int i = 0; i < (int)pattern.size(); ++i)	{
		forToken = std::regex_replace(forToken, pattern[i], " " + std::to_string(i + 1) + " ");
	}

	forToken = std::regex_replace(forToken, pattern[(int)pattern.size() - 1], " " + std::to_string((int)pattern.size()) + " ");

	forValue = std::regex_replace(forValue, removePattern[(int)removePattern.size() - 2], removedString[(int)removePattern.size() - 2]);

	vStream << forValue;

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

	return 0;
}