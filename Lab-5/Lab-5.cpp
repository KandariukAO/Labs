// Лабораторная работа #5, вариант 10, Кандарюк Артем, А-16-21

#include "../Common/commondefs.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

typedef double value_t;
#define VALUE_FMT "%lf"

typedef value_t(*Selector)(value_t);

const int MAX_INPUT_ARRAY_LENGTH = 100;
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
    value_t MulValue;
};

value_t Identity(value_t value)
{
    return value;
}

value_t MultiplyByTwo(value_t value)
{
    return value * 2;
}

bool ReadArgs(int argc, char* argv[], value_t* pLowerBound, value_t* pUpperBound, const char** pInputFilePath)
{
    const int LOWER_BOUND_ARG_IDX = 0;
    const int UPPER_BOUND_ARG_IDX = 1;
    const int INPUT_FILE_ARG_IDX  = 2;

    if (!GetUserArg<value_t>(argc, argv, LOWER_BOUND_ARG_IDX, pLowerBound) ||
        !GetUserArg<value_t>(argc, argv, UPPER_BOUND_ARG_IDX, pUpperBound))
    {
        fprintf(stderr, "Lower/upper bounds not provided or invalid\n");
        return false;
    }

    const char* inputFilePath = GetUserArg(argc, argv, INPUT_FILE_ARG_IDX);
    if (inputFilePath == nullptr)
    {
        fprintf(stderr, "Input file path not provided\n");
        return false;
    }

    *pInputFilePath = inputFilePath;
    return true;
}

FILE* OpenInputFile(const char* inputFilePath)
{
    FILE* input = fopen(inputFilePath, "r");
    if (input == nullptr)
    {
        fprintf(stderr, "Failed to open the input file: %s\n", strerror(errno));
    }

    return input;
}

bool ParseArrays(FILE* input, Array* arrays)
{
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        Array*  array = &arrays[i];
        value_t value;
        while (fscanf(input, VALUE_FMT, &value) == 1)
        {
            array->Values[array->Length++] = value;
        }

        if (array->Length == 0)
        {
            fprintf(stderr, "Failed to find any values for array %d!\n", i);
            return false;
        }

        if (array->Length >= MAX_INPUT_ARRAY_LENGTH)
        {
            fprintf(stderr, "Too many values in array %d!\n", i);
            return false;
        }

        if (fscanf(input, "\r") != 0)
        {
            fprintf(stderr, "Array %d is not present in the input file!\n", i);
            return false;
        }

        char comma;
        if ((i != (ARRAYS_COUNT - 1)) && ((fscanf(input, " %c", &comma) != 1) || (comma != ',')))
        {
            fprintf(stderr, "Unexpected delimiter after array %d!\n", i);
            return false;
        }
    }

    return true;
}

void ComputeResults(Array* srcArrays,
                    Selector selector,
                    value_t  lowerBound,
                    value_t  upperBound,
                    Result* results,
                    value_t* pMinMulValue)
{
    Array arrays[ARRAYS_COUNT]{};
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        Array* srcArray = &srcArrays[i];
        Array* array    = &arrays[i];

        array->Length = srcArray->Length;
        for (int j = 0; j < srcArray->Length; j++)
        {
            array->Values[j] = selector(srcArray->Values[j]);
        }
    }

#ifdef DEBUG
    dbgf("\nSource arrays:\n");
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        Array* srcArray = &srcArrays[i];

        dbgf(" ");
        for (int j = 0; j < srcArray->Length; j++)
        {
            dbgf(VALUE_FMT " ", srcArray->Values[j]);
        }
        dbgf("\n");
    }

    dbgf("Converted arrays:\n");
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        Array* array = &arrays[i];

        dbgf(" ");
        for (int j = 0; j < array->Length; j++)
        {
            dbgf(VALUE_FMT " ", array->Values[j]);
        }
        dbgf("\n");
    }
#endif // DEBUG

    // First, find what values our arrays have.    
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        value_t mulValue   = 1;
        Array* array      = &arrays[i];
        bool    foundValue = false;

        for (int j = 0; j < array->Length; j++)
        {
            value_t value = array->Values[j];
            if ((lowerBound <= value) && (value <= upperBound))
            {
                foundValue = true;
                mulValue *= value;
            }
        }

        results[i].ArrayIndex = foundValue ? i : INVALID_INDEX;
        results[i].MulValue   = mulValue;
    }

    // We'll do two simple passes: first find the minimum value,
    // then invalidate the arrays that have values greater than it.
    value_t minMulValue = 0;
    bool    foundValue  = false;
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        Result* result = &results[i];
        if (result->ArrayIndex != INVALID_INDEX)
        {
            if (!foundValue)
            {
                foundValue  = true;
                minMulValue = result->MulValue;
            }
            else
            {
                if (result->MulValue < minMulValue)
                {
                    minMulValue = result->MulValue;
                }
            }
        }
    }

    if (foundValue)
    {
        for (int i = 0; i < ARRAYS_COUNT; i++)
        {
            Result* result = &results[i];
            if ((result->ArrayIndex != INVALID_INDEX) && (result->MulValue != minMulValue))
            {
                result->ArrayIndex = INVALID_INDEX;
                continue;
            }
        }
    }

    *pMinMulValue = minMulValue;
}

void PrintResults(Result* results, const char* selectorName, value_t minMulValue, value_t lowerBound, value_t upperBound)
{
    int resultCount = 0;
    for (int i = 0; i < ARRAYS_COUNT; i++)
    {
        if (results[i].ArrayIndex != INVALID_INDEX)
        {
            resultCount++;
        }
    }

    printf("Using selector '%s':\n", selectorName);

    if (resultCount != 0)
    {
        printf(" - result: " VALUE_FMT ", array%s: ", minMulValue, resultCount > 1 ? "s" : "");
        for (int i = 0; i < ARRAYS_COUNT; i++)
        {
            Result* result = &results[i];
            if (result->ArrayIndex != INVALID_INDEX)
            {
                printf("%d%s ", result->ArrayIndex, (resultCount-- != 1) ? "," : "");
            }
        }
        printf("\n");
    }
    else
    {
        // Just say that we don't have anything...
        printf(" - found no values in the supplied arrays that were between " VALUE_FMT " and " VALUE_FMT "\n",
               lowerBound, upperBound);
    }
}

int Exit(int errorCode, FILE* input)
{
    fclose(input);
    return errorCode;
}

int main(int argc, char* argv[])
{
    value_t lowerBound;
    value_t upperBound;
    const char* inputFilePath;
    if (!ReadArgs(argc, argv, &lowerBound, &upperBound, &inputFilePath))
    {
        return -1;
    }

    dbgf("Lower bound is " VALUE_FMT "\n", lowerBound);
    dbgf("Upper bound is " VALUE_FMT "\n", upperBound);
    dbgf("Input file path is: '%s'\n", inputFilePath);

    FILE* input = OpenInputFile(inputFilePath);
    if (input == nullptr)
    {
        return -2;
    }

    Array arrays[ARRAYS_COUNT]{};
    if (!ParseArrays(input, arrays))
    {
        return Exit(-3, input);
    }

    struct SelectorDsc
    {
        Selector    Func;
        const char* Name;
    };
#define SELECTOR(s) {s, #s}

    SelectorDsc selectors[] = { SELECTOR(sin), SELECTOR(Identity), SELECTOR(MultiplyByTwo) };
    for (SelectorDsc selector : selectors)
    {
        Result  results[ARRAYS_COUNT]{};
        value_t minMulValue;
        ComputeResults(arrays, selector.Func, lowerBound, upperBound, results, &minMulValue);

        PrintResults(results, selector.Name, minMulValue, lowerBound, upperBound);
    }

    return Exit(0, input);
}
