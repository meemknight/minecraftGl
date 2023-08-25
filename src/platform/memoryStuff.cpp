#include <memoryStuff.h>
#include <vector>
#include <Windows.h>
#include <iostream>

//https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
std::string getLastErrorString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
	{
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

OppenedQuery initVirtualQuery(HANDLE process)
{
	OppenedQuery q = {};

	q.queriedProcess = process;
	q.baseQueriedPtr = 0;
	return q;
}

//hi is exclusive
bool getNextQuery(OppenedQuery &query, void *&low, void *&hi, int &flags)
{

	if (query.queriedProcess == 0) { return false; }

	flags = memQueryFlags_None;
	low = nullptr;
	hi = nullptr;

	MEMORY_BASIC_INFORMATION memInfo;

	bool rez = 0;
	while (true)
	{
		rez = VirtualQueryEx(query.queriedProcess, (void *)query.baseQueriedPtr, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

		if (!rez)
		{
			query = {};
			return false;
		}

		query.baseQueriedPtr = (char *)memInfo.BaseAddress + memInfo.RegionSize;

		if (memInfo.State == MEM_COMMIT)
		{
			//good page

			//if (flags)
			{
				if (memInfo.Protect & PAGE_READONLY)
				{
					flags |= memQueryFlags_Read;
				}
				
				if (memInfo.Protect & PAGE_READWRITE)
				{
					flags |= (memQueryFlags_Read | memQueryFlags_Write);
				}
				
				if (memInfo.Protect & PAGE_EXECUTE)
				{
					flags |= memQueryFlags_Execute;
				}
				
				if (memInfo.Protect & PAGE_EXECUTE_READ)
				{
					flags |= (memQueryFlags_Execute | memQueryFlags_Read);
				}
				
				if (memInfo.Protect & PAGE_EXECUTE_READWRITE)
				{
					flags |= (memQueryFlags_Execute | memQueryFlags_Read | memQueryFlags_Write);
				}

				if (memInfo.Protect & PAGE_EXECUTE_WRITECOPY)
				{
					flags |= (memQueryFlags_Execute | memQueryFlags_Read);
				}

				if (memInfo.Protect & PAGE_WRITECOPY)
				{
					flags |= memQueryFlags_Read;
				}

				

				//todo there are some other flags that I should look into
			}

			low = memInfo.BaseAddress;
			hi = (char *)memInfo.BaseAddress + memInfo.RegionSize;
			return true;
		}

	}
}

bool writeMemory(HANDLE process, void *ptr, void *data, size_t size)
{
	BOOL writeSucceeded = WriteProcessMemory(
		process,
		ptr,
		data,
		size,
		NULL);

	if (!writeSucceeded)
	{
		return 0;
		std::cout << getLastErrorString().c_str() << "\n";
	}

}

bool readMemory(HANDLE process, void *start, size_t size, void *buff)
{
	SIZE_T readSize = 0;
	return ReadProcessMemory(process, start, buff, size, &readSize);
}




//http://kylehalladay.com/blog/2020/05/20/Rendering-With-Notepad.html
std::vector<void *> findBytePatternInProcessMemory(HANDLE process, void *pattern, size_t patternLen)
{
	if (patternLen == 0) { return {}; }

	std::vector<void *> returnVec;
	returnVec.reserve(1000);

	auto query = initVirtualQuery(process);

	if (!query.oppened())
		return {};

	void *low = nullptr;
	void *hi = nullptr;
	int flags = memQueryFlags_None;

	while (getNextQuery(query, low, hi, flags))
	{
		if ((flags | memQueryFlags_Read) && (flags | memQueryFlags_Write))
		{
			//search for our byte patern
			size_t size = (char *)hi - (char *)low;
			char *localCopyContents = new char[size];
			if (
				readMemory(process, low, size, localCopyContents)
				)
			{
				char *cur = localCopyContents;
				size_t curPos = 0;
				while (curPos < size - patternLen + 1)
				{
					if (memcmp(cur, pattern, patternLen) == 0)
					{
						returnVec.push_back((char *)low + curPos);
					}
					curPos++;
					cur++;
				}
			}
			delete[] localCopyContents;
		}
	}

	return returnVec;
}
