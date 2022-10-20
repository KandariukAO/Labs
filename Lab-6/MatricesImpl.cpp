#include "../Common/commondefs.h"
#include "List.h"

#include <cstdlib>

const int MATRIX_COUNT = 2;

typedef List<int> Row;

struct Matrix
{
    List<Row> Rows;

    int GetHeight() const
    {
        return Rows.Count();
    }

    int GetWidth() const
    {
        assert(GetHeight() != 0);
        return Rows[0].Count();
    }
};

bool ParseMatrix(FILE* input, Matrix* matrix, const char** pFailureReason)
{
    while (!feof(input))
    {
        Row row;
        int value;
        while (fscanf(input, "%d", &value) == 1)
        {
            row.Add(value);
        }

        if (row.Count() == 0)
        {
            *pFailureReason = "row width is zero / the row was not parseable";
            return false;
        }

        if ((matrix->GetHeight() != 0) && (row.Count() != matrix->GetWidth()))
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

        matrix->Rows.Add(std::move(row));
    }

    if (matrix->GetWidth() != matrix->GetHeight())
    {
        *pFailureReason = "width not equal to height";
        return false;
    }

    return true;
}

bool ConstructMatrix(int argc, char* argv[], int argIdx, Matrix* matrix, const char** pFailureReason)
{
    const char* inputFilePath = GetUserArg(argc, argv, argIdx);
    if (inputFilePath == nullptr)
    {
        *pFailureReason = "input file path not provided";
        return false;
    }

    FILE* inputFile = fopen(inputFilePath, "r");
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
    for (int y = 0; y < matrix->GetHeight(); y++)
    {
        for (int x = 0; x < matrix->GetWidth(); x++)
        {
            if (matrix->Rows[y][x] == 0)
            {
                return true;
            }
        }
    }

    return false;
}

double FindMean(const Row& row)
{
    int sum = 0;
    for (size_t x = 0; x < row.Count(); x++)
    {
        sum += row[x];
    }

    return (double)sum / row.Count();
}

int MatricesImpl(int argc, char* argv[])
{
    Matrix matrices[MATRIX_COUNT];

    for (int i = 0; i < MATRIX_COUNT; i++)
    {
        const char* failureReason = nullptr;
        if (!ConstructMatrix(argc, argv, i, &matrices[i], &failureReason))
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
            for (int y = 0; y < matrix->GetHeight(); y++)
            {
                double mean = FindMean(matrix->Rows[y]);
                printf(" Row %d: ", y);
                for (int x = 0; x < matrix->GetWidth(); x++)
                {
                    printf("%d, ", matrix->Rows[y][x]);
                }
                printf("mean - %.2lf\n", mean);
            }
        }
    }
}