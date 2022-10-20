#if defined(LAB8DLL_EXPORTS)
#define LAB8_API __declspec(dllexport)
#elif defined(LAB8DLL_IMPORTS)
#define LAB8_API __declspec(dllimport)
#else
#define LAB8_API
#endif

void LAB8_API RemoveDuplicateChars(const char* inputLine, char* outputLine);