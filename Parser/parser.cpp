#include "parser.hpp"

const int PatternSize = 20;

int main() 
{
	GetToken();
}

int GetToken() 
{
	std::regex pattern[PatternSize] = 
	{

		std::regex("([a-zA-Z_가-힣][a-zA-Z_가-힣0-9]*)(?=((은|는|이|가|을|를|와|에|\\(\\w*\\))\\s))"),			// Id
		std::regex("[{}]"),																						// Default					
		std::regex("[()]"),																						// Default
		std::regex("정수"),																						// Keyword - INT
		std::regex("실수"),																						// Keyword - DOUBLE
		std::regex("있"),																						// Keyword - DECLARATION
		std::regex("반환한"),																					// Keyword - RETURN
		std::regex("만약"),																						// Keyword - IF	
		std::regex("아니면"),																					// Keyword - ELSE
		std::regex("한"),																						// Keyword - WHILE
		std::regex("그만한"),																					// Keyword - BREAK
		std::regex("(은|는|이|가|을|를|와|에)"),																// Josa
		std::regex("^-?\\d*"),																					// Number - INT
		std::regex("^-?\\d*(\\.\\d+)?$"),																		// Number - DOUBLE
		std::regex("이다"),
		std::regex("더한다"),																					// Operator - ADD
		std::regex("뺀다"),																						// Operator - SUB
		std::regex("곱한다"),																					// Operator - MUL
		std::regex("나눈"),																						// Operator - DIV
		
	};

	std::regex removePattern("([가-힣][가-힣]*)(다.) |값");
	std::ifstream file("data.txt");
	std::string buffer;
	std::string temporary;
	std::smatch Match;

	while (std::getline(file, temporary))
	{
		temporary = std::regex_replace(temporary, removePattern, std::string("$1"));
		std::cout << temporary << "\n";
		for (int i = 0; i < PatternSize; ++i)
		{
			temporary = std::regex_replace(temporary, pattern[i], std::to_string(i + 1) + " ");
		}
		buffer += temporary;
	}

	std::cout << buffer << "\n";

	return 0;
}
