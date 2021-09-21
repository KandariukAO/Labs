#include <cstdio>

namespace Lab4
{
#define VALUE_FMT "%d"
    typedef int value_t;
    const int CMD_ARGS_OFFSET        = 2;
    const int ARRAYS_COUNT           = 3;
    const int INVALID_INDEX          = -1;

    struct Array
    {
        value_t* Values;
        int      Length;
    };

    struct Result
    {
        int     ArrayIndex;
        value_t Value;
    };

    bool ParseParameters(int argc, char* argv[], FILE** inputFile, value_t* lowerBound, value_t* upperBound);
    bool ParseArrays(FILE* input, Array* arrays);
    void CalculateMultiplicationResults(Array* arrays, Result* results, value_t lowerBound, value_t upperBound);
    void ResolveResults(Result* results, bool* outFoundValue, value_t* outMaxMulValue);
    void DisplayResults(Result* results, bool foundValue, value_t maxMulValue, value_t lowerBound, value_t upperBound);
}