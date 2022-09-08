// Лабораторная работа #1, вариант 10, Кандарюк Артем, А-16-21

#include "../Common/commondefs.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

typedef int value_t;
#define VALUE_FMT "%d"

const int MAX_INPUT_ARRAY_LENGTH = 100;
const int ARRAYS_COUNT           = 3;
const int CMD_ARGS_OFFSET        = 1;

struct Array
{
    value_t Values[MAX_INPUT_ARRAY_LENGTH];
    int     Length;
};

int main(int argc, char* argv[])
{
    const int LOWER_BOUND_ARG_IDX = CMD_ARGS_OFFSET;
    const int UPPER_BOUND_ARG_IDX = CMD_ARGS_OFFSET + 1;
    const int INPUT_FILE_ARG_IDX  = CMD_ARGS_OFFSET + 2;

    value_t lowerBound;
    value_t upperBound;
    if ((argc < (UPPER_BOUND_ARG_IDX + 1)) ||
        (sscanf(argv[LOWER_BOUND_ARG_IDX], VALUE_FMT, &lowerBound) != 1) ||
        (sscanf(argv[UPPER_BOUND_ARG_IDX], VALUE_FMT, &upperBound) != 1) ||
        (lowerBound > upperBound))
    {
        fprintf(stderr, "Lower/upper bounds are invalid or not parseable!\n");
        return -1;
    }

    FILE* input;
    char inputFilePath[MAX_PATH]{};
    if ((argc >= (INPUT_FILE_ARG_IDX + 1) && (sscanf(argv[INPUT_FILE_ARG_IDX], MAX_PATH_FMT, inputFilePath) == 1)))
    {
        input = fopen(inputFilePath, "r");

        if (input == nullptr)
        {
            fprintf(stderr, "Failed to open the input file: %s\n", strerror(errno));
            return errno;
        }
    }
    else
    {
        printf("No input file provided, using stdin\n");
        input = stdin;
    }

    dbgf("Lower bound is " VALUE_FMT "\n", lowerBound);
    dbgf("Upper bound is " VALUE_FMT "\n", upperBound);

    Array arrays[ARRAYS_COUNT]{};

    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        dbgf("Parsing array %d...\n", i);

        Array* array = &arrays[i];

        value_t value;
        while (fscanf(input, VALUE_FMT, &value) == 1)
        {
            dbgf("Parsed " VALUE_FMT "\n", value);
            array->Values[array->Length++] = value;
        }

        if (array->Length == 0)
        {
            fprintf(stderr, "Failed to find any values for array %d!\n", i);
            return -2;
        }

        if (array->Length >= MAX_INPUT_ARRAY_LENGTH)
        {
            fprintf(stderr, "Too many values in array %d!\n", i);
            return -3;
        }

        if (fscanf(input, "\r") != 0)
        {
            fprintf(stderr, "Array %d is not present in the input file!\n", i);
            return -4;
        }

        char comma;
        if ((i != (ARRAYS_COUNT - 1)) && ((fscanf(input, " %c", &comma) != 1) || (comma != ',')))
        {
            fprintf(stderr, "Unexpected delimiter after array %d!\n", i);
            return -5;
        }
    }

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
        value_t MulValue;
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
                foundValue = true;
                mulValue *= value;
            }
        }

        results[i].ArrayIndex = foundValue ? i : INVALID_INDEX;
        results[i].MulValue      = mulValue;
    }

#ifdef DEBUG
    dbgf("After first pass of assigning results:\n");
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        const Result& result = results[i];

        dbgf("Result for array %d: ", i);
        if (result.ArrayIndex != INVALID_INDEX)
        {
            dbgf("value is " VALUE_FMT "\n", result.MulValue);
        }
        else
        {
            dbgf("had no valid values\n");
        }
    }
#endif // DEBUG

    // We'll do two simple passes: first find the minimum value,
    // then invalidate the arrays that have values greater than it.
    value_t minMulValue;
    bool    foundValue = false;
    for (const Result& result : results)
    {
        if (result.ArrayIndex != INVALID_INDEX)
        {
            if (!foundValue)
            {
                foundValue  = true;
                minMulValue = result.MulValue;
            }
            else
            {
                if (result.MulValue < minMulValue)
                {
                    minMulValue = result.MulValue;
                }
            }
        }
    }

    if (foundValue)
    {
        int resultCount = 0;
        for (Result& result : results)
        {
            if ((result.ArrayIndex != INVALID_INDEX) && (result.MulValue != minMulValue))
            {
                result.ArrayIndex = INVALID_INDEX;
                continue;
            }

            resultCount++;
        }

        // Finally, display the results.
        printf("Result: " VALUE_FMT ", array%s: ", minMulValue, resultCount > 1 ? "s" : "");
        for (const Result& result : results)
        {
            if (result.ArrayIndex != INVALID_INDEX)
            {
                printf("%d%s ", result.ArrayIndex, (result.ArrayIndex != (ARRAYS_COUNT - 1)) ? "," : "");
            }
        }
        printf("\n");
    }
    else
    {
        // Just say that we don't have anything...
        printf("Found no values in the supplied arrays that were between " VALUE_FMT " and " VALUE_FMT "\n",
               lowerBound, upperBound);
    }

    return 0;
}
