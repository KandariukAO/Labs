#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cassert>
#include <cstring>

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

inline bool AreEqual(const char* left, const char* right)
{
    return !strcmp(left, right);
}

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
const char* GetPrintfFormat() { assert(!"Unexpected format"); return ""; }

template <>
inline const char* GetPrintfFormat<int>() { return "%d"; }

template <>
inline const char* GetPrintfFormat<double>() { return "%lf"; }

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
