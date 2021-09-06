#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef DEBUG
#define dbgf printf
#else
#define dbgf
#endif
