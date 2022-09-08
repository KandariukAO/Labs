// Лабораторная работа #2, вариант 10, Кандарюк Артем, А-16-21

#include "../Common/commondefs.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

const int CMD_ARGS_OFFSET = 1;
const int MAX_WIDTH       = 20;
const int MAX_HEIGHT      = 20;
const int MATRIX_COUNT    = 2;

struct Matrix
{
    int Height;
    int Width;
    int Data[MAX_HEIGHT][MAX_WIDTH];
};

bool ParseMatrix(FILE* input, Matrix* matrix, const char** pFailureReason)
{
    int width  = -1;
    int height = 0;

    while (!feof(input))
    {
        int rowWidth = 0;

        int value;
        while (fscanf(input, "%d", &value) == 1)
        {
            matrix->Data[height][rowWidth++] = value;
        }

        if (rowWidth == 0)
        {
            *pFailureReason = "row width is zero / the row was not parseable";
            return false;
        }

        if (rowWidth >= MAX_WIDTH)
        {
            *pFailureReason = "row width is too large";
            return false;
        }

        if (width == -1)
        {
            width = rowWidth;
        }

        if (width != rowWidth)
        {
            *pFailureReason = "rows do not all have the same width";
            return false;
        }

        char semicolon;
        if ((fscanf(input, " %c ", &semicolon) != 1) || (semicolon != ';'))
        {
            *pFailureReason = "row does not end with a semicolon";
            return false;
        }

        height++;
    }

    if (width != height)
    {
        *pFailureReason = "width not equal to height";
        return false;
    }

    matrix->Height = height;
    matrix->Width  = width;

    return true;
}

bool ConstructMatrix(int argc, char* argv[], int argIdx, Matrix* matrix, const char** pFailureReason)
{
    if (argIdx > (argc - 1))
    {
        *pFailureReason = "argument not provided";
        return false;
    }

    FILE* inputFile = fopen(argv[argIdx], "r");
    if (inputFile == nullptr)
    {
        *pFailureReason = strerror(errno);
        return false;
    }

    if (!ParseMatrix(inputFile, matrix, pFailureReason))
    {
        return false;
    }

    return true;
}

bool MatrixHasZeroElements(Matrix* matrix)
{
    for (int y = 0; y < matrix->Height; y++)
    {
        for (int x = 0; x < matrix->Width; x++)
        {
            if (matrix->Data[y][x] == 0)
            {
                return true;
            }
        }
    }

    return false;
}

double FindMean(int row[], int width)
{
    int sum = 0;
    for (int x = 0; x < width; x++)
    {
        sum += row[x];
    }

    return (double)sum / width;
}

int main(int argc, char* argv[])
{
    Matrix matrices[MATRIX_COUNT];

    for (int i = 0; i < MATRIX_COUNT; i++)
    {
        const char* failureReason = nullptr;
        if (!ConstructMatrix(argc, argv, CMD_ARGS_OFFSET + i, &matrices[i], &failureReason))
        {
            fprintf(stderr, "Failed to construct matrix %d: %s\n", i, failureReason);
            return -1;
        }
    }

    for (int i = 0; i < MATRIX_COUNT; i++)
    {
        Matrix* matrix = &matrices[i];
        if (MatrixHasZeroElements(matrix))
        {
            printf("Matrix %d has zero elements...\n", i);
            for (int y = 0; y < matrix->Height; y++)
            {
                double mean = FindMean(matrix->Data[y], matrix->Width);
                printf(" Row %d: ", y);
                for (int x = 0; x < matrix->Width; x++)
                {
                    printf("%d, ", matrix->Data[y][x]);
                }
                printf("mean - %.2lf\n", mean);
            }
        }
    }
}