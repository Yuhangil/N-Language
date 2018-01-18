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

		std::regex("([a-zA-Z_��-�R][a-zA-Z_��-�R0-9]*)(?=((��|��|��|��|��|��|��|��|\\(\\w*\\))\\s))"),			// Id
		std::regex("[{}]"),																						// Default					
		std::regex("[()]"),																						// Default
		std::regex("����"),																						// Keyword - INT
		std::regex("�Ǽ�"),																						// Keyword - DOUBLE
		std::regex("��"),																						// Keyword - DECLARATION
		std::regex("��ȯ��"),																					// Keyword - RETURN
		std::regex("����"),																						// Keyword - IF	
		std::regex("�ƴϸ�"),																					// Keyword - ELSE
		std::regex("��"),																						// Keyword - WHILE
		std::regex("�׸���"),																					// Keyword - BREAK
		std::regex("(��|��|��|��|��|��|��|��)"),																// Josa
		std::regex("^-?\\d*"),																					// Number - INT
		std::regex("^-?\\d*(\\.\\d+)?$"),																		// Number - DOUBLE
		std::regex("�̴�"),
		std::regex("���Ѵ�"),																					// Operator - ADD
		std::regex("����"),																						// Operator - SUB
		std::regex("���Ѵ�"),																					// Operator - MUL
		std::regex("����"),																						// Operator - DIV
		
	};

	std::regex removePattern("([��-�R][��-�R]*)(��.) |��");
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
