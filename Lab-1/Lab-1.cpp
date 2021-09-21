#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include "../Common/commondefs.h"

typedef int value_t;
#define VALUE_FMT "%d"

const int MAX_INPUT_ARRAY_LENGTH = 100;
const int CMD_ARGS_OFFSET = 2;
const int ARRAYS_COUNT = 3;

struct Array
{
    value_t Values[MAX_INPUT_ARRAY_LENGTH];
    int     Length;
};

int MainOne(int argc, char* argv[])
{
    FILE* input = stdin;
    Array arrays[ARRAYS_COUNT]{};
    bool  error = false;

    value_t lowerBound;
    value_t upperBound;
    if ((argc < (CMD_ARGS_OFFSET + 2)) ||
        (sscanf(argv[CMD_ARGS_OFFSET], VALUE_FMT, &lowerBound) != 1) ||
        (sscanf(argv[CMD_ARGS_OFFSET + 1], VALUE_FMT, &upperBound) != 1) ||
        (lowerBound > upperBound))
    {
        error = true;
        fprintf(stderr, "Lower/upper bounds are invalid or not parseable!");
    }
    else
    {
        dbgf("Lower bound is " VALUE_FMT "\n", lowerBound);
        dbgf("Upper bound is " VALUE_FMT "\n", upperBound);

        for (int i = 0; i < ARRAYS_COUNT; i++)
        {
            dbgf("Parsing line %d...\n", i);

            Array* array  = &arrays[i];
            int    length = 0;
            while (!feof(input) && !error)
            {
                value_t value;
                int     parse = fscanf(input, VALUE_FMT "%*[\x20]", &value);

                if (parse == 1)
                {
                    if (length >= MAX_INPUT_ARRAY_LENGTH)
                    {
                        error = true;
                        fprintf(stderr, "Too many values in the input file!");
                    }
                    else
                    {
                        array->Values[length++] = value;
                    }
                }

                if (!error)
                {
                    int nextChar = getc(input);
                    if ((nextChar == '\n'))
                    {
                        break;
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
                array->Length = length;;
                dbgf("Successfully parsed %d values\n", length);
            }
        }

        if (!error)
        {
#ifdef DEBUG
            dbgf("Displaying arrays:\n");
            for (int i = 0; i < ARRAYS_COUNT; i++)
            {
                dbgf("Array %d: ", i);
                Array* array = &arrays[i];
                for (int j = 0; j < array->Length; j++)
                {
                    dbgf(VALUE_FMT " ", array->Values[j]);
                }
                dbgf("\b\n");
            }
#endif // DEBUG

            struct Result
            {
                int     ArrayIndex;
                value_t Value;
            };

            // First, find what values our arrays have.
            const int INVALID_INDEX = -1;
            Result    results[ARRAYS_COUNT]{};
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

            // We'll do two simple passes: first find the maximum value,
            // then invalidate the arrays that have values less than it.
            value_t maxMulValue;
            bool    foundValue = false;
            for (const Result& result : results)
            {
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
                for (Result& result : results)
                {
                    if ((result.ArrayIndex != INVALID_INDEX) && (result.Value != maxMulValue))
                    {
                        result.ArrayIndex = INVALID_INDEX;
                    }
                }

                // Finally, display the results.
                printf("The resulting maximum value from array(s) ");
                for (const Result& result : results)
                {
                    if (result.ArrayIndex != INVALID_INDEX)
                    {
                        printf("%d, ", result.ArrayIndex);
                    }
                }
                printf("is " VALUE_FMT, maxMulValue);
            }
            else
            {
                // Just say that we don't have anything...
                printf("Found no values in the supplied arrays that were between " VALUE_FMT " and " VALUE_FMT,
                       lowerBound, upperBound);
            }
        }
    }

    return error ? -1 : 0;
}
