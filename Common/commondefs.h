#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#define MAX_PATH 260
#define MAX_PATH_FMT "%260s"

#ifdef DEBUG
#define dbgf printf
#else
#define dbgf
#endif

const int CMD_USER_ARGS_OFFSET = 1;

inline const char* GetUserArg(int argc, char* argv[], int index)
{
	if (argc < (CMD_USER_ARGS_OFFSET + index))
	{
		return nullptr;
	}

    return argv[CMD_USER_ARGS_OFFSET + index];
}

template <typename T>
const char* GetPrintfFormat() { return ""; }

template <>
inline const char* GetPrintfFormat<int>() { return "%d"; }

template <typename T>
bool GetUserArg(int argc, char* argv[], int index, T* pValue)
{
	const char* argString = GetUserArg(argc, argv, index);
	if (argString == nullptr)
	{
		return false;
	}

	T value;
	if (sscanf(argString, GetPrintfFormat<T>(), &value) != 1)
	{
		return false;
	}

	*pValue = value;
	return true;
}
