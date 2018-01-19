#include <iostream>
#include <fstream>
#include <Windows.h>

#include "util.h"

bool ReadFile(const char* pFileName, string& outFile)
{
	ifstream f(pFileName);

	bool ret = false;

	if (f.is_open())
	{
		string line;
		while (getline(f, line))
		{
			outFile.append(line);
			outFile.append("\n");
		}

		f.close();

		ret = true;
	}

	return ret;
}


long long GetCurrentTimeMillis()
{
	return GetTickCount();
}
