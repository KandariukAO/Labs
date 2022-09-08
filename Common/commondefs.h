#define _CRT_SECURE_NO_WARNINGS

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
