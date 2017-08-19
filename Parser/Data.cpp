#include "Data.h"

Dataset::Dataset()
{

}


int Dataset::input(FILE* fp)
{
	wchar_t ch = 0;
	while (isspace(ch))
		ch = fgetc(fp);

	if (isId(ch))
	{
		do
		{
			buffer += ch;
		} while (isId(ch));
	}

}


bool Dataset::isId(wchar_t ch)
{
	if ('°¡' <= ch && ch <= 'ÆR')
	{
		return 1;
	}
	else if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z'))
	{
		return 1;
	}
	return 0;
}
