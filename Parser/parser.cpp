#include "parser.hpp"

const int patternSize = 22;

int main()
{
	GetToken();
}

int GetToken()
{
	std::regex pattern[patternSize] =	{
		std::regex("^-?\\d+"),
		std::regex("^-?\\d+\\.\\d+?$"),
		std::regex("정수"),
		std::regex("실수"),
		std::regex("만약"),
		std::regex("아니면"),
		std::regex("있다"),
		std::regex("이다"),
		std::regex("더한다"),
		std::regex("뺀다"),
		std::regex("곱한다"),
		std::regex("나눈다"),
		std::regex("반환한다"),
		std::regex("한다"),
		std::regex("그만한다"),
		std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(?=((은|는|이|가|을|를|와|에|[(])))"),
		std::regex("(은|는|이|가|을|를|와|에)"),
		std::regex("[{}]"),
		std::regex("[()]")	
	};

	std::regex removePattern("([가-힣][가-힣]*다)(\\.)");
	std::ifstream file("../data.txt");
	std::string buffer;
	std::string original;
	std::string temporary;
	std::smatch Match;

	while (std::getline(file, temporary))	{
		temporary = std::regex_replace(temporary, removePattern, std::string("$1"));
		original = temporary;
		for (int i = 0; i < patternSize - 1; ++i)	{
			temporary = std::regex_replace(temporary, pattern[i], std::to_string(i + 1) + " ");
		}
		buffer += temporary;
	}

	std::cout << buffer << "\n";

	return 0;
}
