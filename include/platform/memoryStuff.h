#pragma once
#include <Windows.h>
#include <string>
#include <vector>

struct OppenedQuery
{
	HANDLE queriedProcess = 0;
	char *baseQueriedPtr = (char *)0x0;
	bool oppened()
	{
		return queriedProcess != 0;
	}
};

std::string getLastErrorString();

OppenedQuery initVirtualQuery(HANDLE process);

bool getNextQuery(OppenedQuery &query, void *&low, void *&hi, int &flags);

enum
{
	memQueryFlags_ = 0,
	memQueryFlags_None = 0,
	memQueryFlags_Read = 0b0001,
	memQueryFlags_Write = 0b0010,
	memQueryFlags_Execute = 0b0100,

};

bool writeMemory(HANDLE process, void *ptr, void *data, size_t size);

bool readMemory(HANDLE process, void *start, size_t size, void *buff);

std::vector<void *> findBytePatternInProcessMemory(HANDLE process, void *pattern, size_t patternLen);
