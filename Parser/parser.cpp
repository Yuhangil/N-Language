#include "parser.hpp"

const int patternSize = 19;

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
		std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)"),
		std::regex("[{}]"),
		std::regex("[()]")	
	};

	std::regex removePattern[5] =	{
		std::regex("([가-힣][가-힣]*다)(\\.)"),
		std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)([(])"),
		std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(은|는|이|가|을|를|와|에)"),
		std::regex("([()])"),
		std::regex("( )+")
	};
	std::ifstream file("data.txt");
	std::ofstream output("output.txt");
	std::string buffer;
	std::string original;
	std::string temporary;
	std::stringstream stringStream;
	stringStream << file.rdbuf();
	temporary = stringStream.str();
	std::smatch Match;

	temporary = std::regex_replace(temporary, removePattern[0], std::string("$1"));
	temporary = std::regex_replace(temporary, removePattern[1], std::string("$1 $2"));
	temporary = std::regex_replace(temporary, removePattern[2], std::string("$1"));
	temporary = std::regex_replace(temporary, removePattern[3], std::string(" $1 "));
	temporary = std::regex_replace(temporary, removePattern[4], std::string(" "));

	original = temporary;

	for (int i = 0; i < patternSize; ++i)	{
		temporary = std::regex_replace(temporary, pattern[i], std::to_string(i + 1) + " ");
	}

	std::cout << temporary << "\n";
	std::cout << original << "\n";

	output << temporary << "\n";
	output << original << "\n";

	return 0;
}