#include "Data.h"

Dataset::Dataset()
{
	pattern[0] = "([a-zA-Z_°¡-ÆR][a-zA-Z_°¡-ÆR0-9]*)(?=((Àº|´Â|ÀÌ|°¡|À»|¸¦|¿Í|¿¡|\\(\\w*\\))\\s))";
	pattern[1] = "[{}]";
	pattern[2] = "[()]";
	pattern[3] = "(Á¤¼ö|½Ç¼ö)";
	pattern[4] = "(Àº|´Â|ÀÌ|°¡|À»|¸¦|¿Í|¿¡)";
	pattern[5] = "(´õÇÑ|»«|°öÇÑ|³ª´«)";
}

void Dataset::input(char* filename)
{
	wifstream ifs(filename);
	while (getline(ifs, String))
		buffer += String;
	ifs.close();
}

void Dataset::solve(wstring Buffer)
{
	
}