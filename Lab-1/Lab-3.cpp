#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include "../Common/commondefs.h"

typedef int value_t;
#define VALUE_FMT "%d"

const int MAX_INPUT_ARRAY_LENGTH = 100;
const int CMD_ARGS_OFFSET        = 2;
const int ARRAYS_COUNT           = 3;
const int INVALID_INDEX          = -1;

struct Array
{
    value_t Values[MAX_INPUT_ARRAY_LENGTH];
    int     Length;
};

struct Result
{
    int     ArrayIndex;
    value_t Value;
};

static bool ParseParameters(int argc, char* argv[], FILE** inputFile, value_t* lowerBound, value_t* upperBound);
static bool ParseArrays(FILE* input, Array* arrays);
static void CalculateMultiplicationResults(Array* arrays, Result* results, value_t lowerBound, value_t upperBound);
static void ResolveResults(Result* results, bool* outFoundValue, value_t* outMaxMulValue);
static void DisplayResults(Result* results, bool foundValue, value_t maxMulValue, value_t lowerBound, value_t upperBound);

int MainThree(int argc, char* argv[])
{
    bool error = false;

    FILE* input;
    value_t lowerBound;
    value_t upperBound;
    Array   arrays[ARRAYS_COUNT];
    if (!ParseParameters(argc, argv, &input, &lowerBound, &upperBound))
    {
        error = true;
    }
    else if (!ParseArrays(input, arrays))
    {
        error = true;
    }
    else
    {
        Result results[ARRAYS_COUNT];
        CalculateMultiplicationResults(arrays, results, lowerBound, upperBound);

        bool    foundValue;
        value_t maxMulValue;
        ResolveResults(results, &foundValue, &maxMulValue);

        DisplayResults(results, foundValue, maxMulValue, lowerBound, upperBound);
    }

    return error ? -1 : 0;
}

static bool ParseParameters(int argc, char* argv[], FILE** inputFile, value_t* lowerBound, value_t* upperBound)
{
    // What we want (offsets are in "user args"):
    // 0: source file path.
    // 1: lower bound.
    // 2: upper bound.

    char** userArgs      = argv + CMD_ARGS_OFFSET;
    int    userArgsCount = argc - CMD_ARGS_OFFSET;
    bool   success       = true;

    if (userArgsCount < 3)
    {
        success = false;
        fprintf(stderr, "Not enough arguments provided!\n");
    }
    else if ((*inputFile = fopen(userArgs[0], "r")) == nullptr)
    {
        success = false;
        perror("Error opening the input file");
    }
    else if (sscanf(userArgs[1], VALUE_FMT, lowerBound) != 1)
    {
        fprintf(stderr, "Failed to parse the lower bound!\n");
    }
    else if (sscanf(userArgs[2], VALUE_FMT, upperBound) != 1)
    {
        fprintf(stderr, "Failed to parse the upper bound!\n");
    }

    dbgf("Lower bound is " VALUE_FMT "\n", *lowerBound);
    dbgf("Upper bound is " VALUE_FMT "\n", *upperBound);

    return success;
}

static bool ParseArrays(FILE* input, Array* arrays)
{
    bool error = false;

    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        dbgf("Parsing line %d...\n", i);

        bool   lineParsed = false;
        int    length     = 0;
        while (!feof(input) && !lineParsed && !error)
        {
            value_t value;
            if (fscanf(input, VALUE_FMT "%*[\x20]", &value) == 1)
            {
                if (length >= MAX_INPUT_ARRAY_LENGTH)
                {
                    error = true;
                    fprintf(stderr, "Too many values in the input file!");
                }
                else
                {
                    arrays[i].Values[length++] = value;
                }
            }

            if (!error)
            {
                int nextChar = getc(input);
                if (nextChar == '\n')
                {
                    lineParsed = true;
                }
                else
                {
                    ungetc(nextChar, input);
                }
            }
        }

        if (length == 0)
        {
            error = true;
            fprintf(stderr, "Failed to find any values!\n");
        }

        if (!error)
        {
            arrays[i].Length = length;;
            dbgf("Successfully parsed %d values\n", length);
        }
    }

#ifdef DEBUG
    if (!error)
    {
        dbgf("Displaying arrays:\n");
        for (int i = 0; i < ARRAYS_COUNT; i++)
        {
            dbgf("Array %d: ", i);
            const Array& array = arrays[i];
            for (int j = 0; j < array.Length; j++)
            {
                dbgf(VALUE_FMT " ", array.Values[j]);
            }
            dbgf("\b\n");
        }
    }
#endif // DEBUG

    return !error;
}

static void CalculateMultiplicationResults(Array* arrays, Result* results, value_t lowerBound, value_t upperBound)
{
    // First, find what values our arrays have.
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        value_t      mulValue   = 1;
        const Array& array      = arrays[i];
        bool         foundValue = false;

        for (int j = 0; j < array.Length; j++)
        {
            value_t value = array.Values[j];
            if ((lowerBound <= value) && (value <= upperBound))
            {
                if (!foundValue)
                {
                    foundValue = true;
                }

                mulValue *= value;
            }
        }

        results[i].ArrayIndex = foundValue ? i : INVALID_INDEX;
        results[i].Value      = mulValue;
    }

#ifdef DEBUG
    dbgf("After first pass of assigning results:\n");
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        const Result& result = results[i];

        dbgf("Result for array %d: ", i);
        if (result.ArrayIndex != INVALID_INDEX)
        {
            dbgf("value is " VALUE_FMT "\n", result.Value);
        }
        else
        {
            dbgf("had no valid values\n");
        }
    }
#endif // DEBUG
}

static void ResolveResults(Result* results, bool* outFoundValue, value_t* outMaxMulValue)
{
    // We'll do two simple passes: first find the maximum value,
    // then invalidate the arrays that have values less than it.
    value_t maxMulValue;
    bool    foundValue = false;
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        const Result& result = results[i];

        if (result.ArrayIndex != INVALID_INDEX)
        {
            if (!foundValue)
            {
                foundValue  = true;
                maxMulValue = result.Value;
            }
            else
            {
                if (result.Value > maxMulValue)
                {
                    maxMulValue = result.Value;
                }
            }
        }
    }

    if (foundValue)
    {
        for (int i = 0; i < ARRAYS_COUNT; i++)
        {
            Result& result = results[i];

            if ((result.ArrayIndex != INVALID_INDEX) && (result.Value != maxMulValue))
            {
                result.ArrayIndex = INVALID_INDEX;
            }
        }
    }

    *outMaxMulValue = maxMulValue;
    *outFoundValue  = foundValue;
}

static void DisplayResults(Result* results, bool foundValue, value_t maxMulValue, value_t lowerBound, value_t upperBound)
{
    if (foundValue)
    {
        printf("The resulting maximum value from array(s) ");
        for (int i = 0; i < ARRAYS_COUNT; i++)
        {
            const Result& result = results[i];

            if (result.ArrayIndex != INVALID_INDEX)
            {
                printf("%d, ", result.ArrayIndex);
            }
        }
        printf("is " VALUE_FMT "\n", maxMulValue);
    }
    else
    {
        printf("Found no values in the supplied arrays that were between " VALUE_FMT " and " VALUE_FMT,
               lowerBound, upperBound);
    }
}