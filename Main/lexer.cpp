#include "lexer.hpp"

std::vector<std::string> valueArray;
std::vector<int> tokenArray;

std::regex pattern[patternSize] =
{
	std::regex("-?\\d+[^.]"),
	std::regex("-?\\d+\\.\\d+"),
	std::regex("정수|실수"),
	std::regex("정수형|실수형"),
	std::regex("만약"),
	std::regex("아니면"),
	std::regex("(더한다|뺀다|곱한다|나눈다|이다|더한|뺀|곱한|나눈)"),
	std::regex("반환한다"),
	std::regex("그만한다"),
	std::regex("한다"),
	std::regex("있다"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)"),
	std::regex("\\{|\\}|\\(|\\)|\\[|\\]|\\.|\\,")
};


std::regex removePattern[] =
{
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)([\\(])"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(은|는|이|가|을|를|와|과|에)"),
	std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*|-?\\d+\\.?\\d*)(이다\\.)"),
	std::regex("([\\(]|[\\)]|[\\,])"),
	std::regex("( )+"),
	std::regex("(더한|뺀|곱한|나눈)( )(값)"),
	std::regex("([^\\d])(\\.)"),
};

int StoreToken(std::ifstream &fileStream)	{
	std::string original;
	std::string temporary;
	std::string temp;
	std::stringstream stringStream;
	stringStream << fileStream.rdbuf();
	temporary = stringStream.str();

	temporary = std::regex_replace(temporary, removePattern[0], std::string("$1 $2"));
	temporary = std::regex_replace(temporary, removePattern[1], std::string("$1"));
    temporary = std::regex_replace(temporary, removePattern[2], std::string("$1 $2"));
	temporary = std::regex_replace(temporary, removePattern[3], std::string(" $1 "));
	temporary = std::regex_replace(temporary, removePattern[4], std::string(" "));
    temporary = std::regex_replace(temporary, removePattern[5], std::string("$1"));
	temporary = std::regex_replace(temporary, removePattern[6], std::string("$1 $2 "));

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

	for(int i = 0; i < valueArray.size(); ++i)	{
		std::cout << tokenArray[i] << " : " << valueArray[i] << "\n";
	}

	std::cout << "=====" << "\n";

	return 0;
}