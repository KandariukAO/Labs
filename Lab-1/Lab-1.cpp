// Лабораторная работа #1, вариант 6, Кандарюк Артем, А-16-20

#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cerrno>
#include <ctype.h>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "../Common/commondefs.h"

typedef int value_t;
#define VALUE_FMT "%d"

const int MAX_INPUT_ARRAY_LENGTH = 100;
const int CMD_ARGS_OFFSET = 1;
const int ARRAYS_COUNT = 3;

struct Array
{
    value_t Values[MAX_INPUT_ARRAY_LENGTH];
    int     Length;
};

// Windows passes us the path to our executable as the first argument. We ignore it.
int ActualCmdArgIndex(int userIndex)
{
    return userIndex + 1;
}

void AnalyzeArrays(Array arrays[], value_t upperBound);
void ParseArrays(FILE* inputFile, Array arrays[]);
void ParseLine(FILE* inputFile, Array* array, const char** errorMessage);

int main(int argc, char* argv[])
{
    int maxArgIndex = argc - 1;

    if (maxArgIndex < ActualCmdArgIndex(0))
    {
        fprintf(stderr, "Path to the input file must be provided as the first argument!");
        return -1;
    }

    if (maxArgIndex < ActualCmdArgIndex(1))
    {
        fprintf(stderr, "The upper bound to compare against must be provided as the second argument!");
        return -2;
    }

    value_t upperBound;
    if (sscanf(argv[ActualCmdArgIndex(1)], VALUE_FMT, &upperBound) != 1)
    {
        fprintf(stderr, "Failed to parse the upper bound from '%s'!", argv[ActualCmdArgIndex(1)]);
        return -3;
    }
    dbgf("The upper bound is: " VALUE_FMT "\n", upperBound);

    const char* inputFilePath = argv[ActualCmdArgIndex(0)];
    FILE* inputFile = fopen(inputFilePath, "r");

    if (inputFile == nullptr)
    {
        perror("Error opening the input file");
        return errno;
    }

    Array arrays[ARRAYS_COUNT];
    ParseArrays(inputFile, arrays);
    fclose(inputFile);

    AnalyzeArrays(arrays, upperBound);
}

void AnalyzeArrays(Array arrays[], value_t upperBound)
{
    value_t upperMaximums[ARRAYS_COUNT];
    value_t sentinel = upperBound;

    dbgf("Displaying arrays:\n");
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        dbgf("Array %d: ", i);
        Array   array = arrays[i];
        value_t upperMax = sentinel;
        for (int j = 0; j < array.Length; j++)
        {
            value_t value = array.Values[j];

            if ((value < upperBound) && ((value > upperMax) || (upperMax == sentinel)))
            {
                upperMax = value;
            }

            dbgf(VALUE_FMT " ", value);
        }

        upperMaximums[i] = upperMax;

        dbgf("\n");
    }

    struct Result
    {
        int     ArrayIndex;
        value_t Value;
    };

    int    resultCount = 0;
    Result results[ARRAYS_COUNT];
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        value_t value = upperMaximums[i];
        if (value != sentinel)
        {
            if (resultCount > 0)
            {
                value_t previousValue = results[resultCount - 1].Value;
                if (value < previousValue)
                {
                    results[resultCount] = {i, value};
                }
                else if (value == previousValue)
                {
                    results[resultCount++] = {i, value};
                }
            }
            else
            {
                results[resultCount++] = {i, value};
            }
        }
    }

    if (resultCount != 0)
    {
        for (int i = 0; i < resultCount; i++)
        {
            Result result = results[i];
            printf(VALUE_FMT " is the maximal value less than " VALUE_FMT " in array %d\n",
                result.Value, upperBound, result.ArrayIndex);
        }
    }
    else
    {
        printf("Found no values less than " VALUE_FMT " in the supplied arrays :(\n", upperBound);
    }
}

void ParseArrays(FILE* inputFile, Array arrays[])
{
    const char* errorMessage = nullptr;
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        dbgf("Parsing line %d...\n", i);

        ParseLine(inputFile, &arrays[i], &errorMessage);
        if (errorMessage != nullptr)
        {
            fprintf(stderr, "%s\n", errorMessage);
            break;
        }

        dbgf("Successfully parsed %d values\n", arrays[i].Length);
    }
}

void ParseLine(FILE* inputFile, Array* array, const char** errorMessage)
{
    int length = 0;
    while (!feof(inputFile))
    {
        value_t value;
        int     parse = fscanf(inputFile, VALUE_FMT "%*[\x20]", &value);

        if (parse == 1)
        {
            if (length >= MAX_INPUT_ARRAY_LENGTH)
            {
                *errorMessage = "Too many values in the input file!";
                break;
            }

            array->Values[length++] = value;
        }

        int nextChar = getc(inputFile);
        if ((nextChar == '\n'))
        {
            break;
        }
        else
        {
            ungetc(nextChar, inputFile);
        }
    }

    if (length == 0)
    {
        *errorMessage = "Failed to find any values!";
    }

    array->Length = length;;
}