#include "main.hpp"

int main(int argc, char** argv)  {
    if(argc < 2)        {
        std::cout << "error : no such input file" << "\n";
        exit(1);
    }

    std::ifstream inputFile(argv[1]);
    std::stringstream inputStream;
    std::string inputString;

	inputStream << inputFile.rdbuf();
    inputString = inputStream.str();

    LexicalAnalysis(inputString); 
}