#define _CRT_SECURE_NO_WARNINGS

#include <cstring>
#include "../Common/commondefs.h"
#include "Lab-4.h"

int MainFour(int argc, char* argv[])
{
    using namespace Lab4;

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

        fclose(input);
        for (const Array& array : arrays)
        {
            delete[] array.Values;
        }
    }

    return error ? -1 : 0;
}

namespace Lab4
{
    bool ParseParameters(int argc, char* argv[], FILE** inputFile, value_t* lowerBound, value_t* upperBound)
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

    bool ParseArrays(FILE* input, Array* arrays)
    {
        bool error = false;

        for (int i = 0; i < ARRAYS_COUNT; i++)
        {
            dbgf("Parsing line %d...\n", i);

            int    length     = 0;
            bool   lineParsed = false;
            Array& array      = arrays[i];

            // Let's start with a conservative estimate that we'll need 10 value_t's.
            array.Length = 10;
            array.Values = new value_t[array.Length];

            while (!feof(input) && !lineParsed && !error)
            {
                value_t value;
                if (fscanf(input, VALUE_FMT "%*[\x20]", &value) == 1)
                {
                    if (length >= array.Length)
                    {
                        // Rellocate at twice the capacity.
                        // Technically could use "realloc" here.
                        int      oldLength = array.Length;
                        int      newLength = oldLength * 2;
                        value_t* oldValues = array.Values;
                        value_t* newValues = new value_t[newLength];
                        memcpy(newValues, oldValues, oldLength * sizeof(value_t));

                        array.Length = newLength;
                        array.Values = newValues;
                        delete[] oldValues;
                    }
                    else
                    {
                        array.Values[length++] = value;
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
                // We will most likely have overallocated,
                // so reset the length to a precise value.
                array.Length = length;;
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

    void CalculateMultiplicationResults(Array* arrays, Result* results, value_t lowerBound, value_t upperBound)
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

    void ResolveResults(Result* results, bool* outFoundValue, value_t* outMaxMulValue)
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

    void DisplayResults(Result* results, bool foundValue, value_t maxMulValue, value_t lowerBound, value_t upperBound)
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
}