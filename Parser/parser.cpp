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

		std::regex("([a-zA-Z_°¡-ÆR][a-zA-Z_°¡-ÆR0-9]*)(?=((Àº|´Â|ÀÌ|°¡|À»|¸¦|¿Í|¿¡|\\(\\w*\\))\\s))"),			// Id
		std::regex("[{}]"),																						// Default					
		std::regex("[()]"),																						// Default
		std::regex("Á¤¼ö"),																						// Keyword - INT
		std::regex("½Ç¼ö"),																						// Keyword - DOUBLE
		std::regex("ÀÖ"),																						// Keyword - DECLARATION
		std::regex("¹ÝÈ¯ÇÑ"),																					// Keyword - RETURN
		std::regex("¸¸¾à"),																						// Keyword - IF	
		std::regex("¾Æ´Ï¸é"),																					// Keyword - ELSE
		std::regex("ÇÑ"),																						// Keyword - WHILE
		std::regex("±×¸¸ÇÑ"),																					// Keyword - BREAK
		std::regex("(Àº|´Â|ÀÌ|°¡|À»|¸¦|¿Í|¿¡)"),																// Josa
		std::regex("^-?\\d*"),																					// Number - INT
		std::regex("^-?\\d*(\\.\\d+)?$"),																		// Number - DOUBLE
		std::regex("ÀÌ´Ù"),
		std::regex("´õÇÑ´Ù"),																					// Operator - ADD
		std::regex("»«´Ù"),																						// Operator - SUB
		std::regex("°öÇÑ´Ù"),																					// Operator - MUL
		std::regex("³ª´«"),																						// Operator - DIV
		
	};

	std::regex removePattern("([°¡-ÆR][°¡-ÆR]*)(´Ù.) |°ª");
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
