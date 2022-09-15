// Лабораторная работа #3, вариант 10, Кандарюк Артем, А-16-21

#include "../Common/commondefs.h"

#include <cerrno>
#include <cstring>

typedef double RecordFieldType;
#define RECORD_FIELD_FMT "%lf"
const int RECORD_FIELD_COUNT = 3;

struct Record
{
    RecordFieldType Fields[RECORD_FIELD_COUNT];
};

const int INPUT_TEXT_FILE_ARG_IDX    = 0;
const int OUTPUT_BINARY_FILE_ARG_IDX = 1;
const int RECORD_ONE_IDX_ARG_IDX     = 2;
const int RECORD_TWO_IDX_ARG_IDX     = 3;

FILE* OpenTextInput(int argc, char* argv[])
{
    const char* filePath = GetUserArg(argc, argv, INPUT_TEXT_FILE_ARG_IDX);
    if (filePath == nullptr)
    {
        fprintf(stderr, "No input file provided\n");
        return nullptr;
    }

    FILE* file = fopen(filePath, "r");
    if (file == nullptr)
    {
        fprintf(stderr, "Error reading the input file: %s", strerror(errno));
        return nullptr;
    }

    return file;
}

FILE* OpenBinaryOutput(int argc, char* argv[])
{
    const char* filePath = GetUserArg(argc, argv, OUTPUT_BINARY_FILE_ARG_IDX);
    if (filePath == nullptr)
    {
        fprintf(stderr, "No output file provided\n");
        return nullptr;
    }

    FILE* file = fopen(filePath, "w+b");
    if (file == nullptr)
    {
        fprintf(stderr, "Error opening the output file: %s\n", strerror(errno));
        return nullptr;
    }

    return file;

}

bool ConvertTextToBinary(FILE* textInput, FILE* binaryOutput, int idxOne, int idxTwo)
{
    int             fieldCount = 0;
    RecordFieldType value;
    while (fscanf(textInput, RECORD_FIELD_FMT " ", &value) == 1)
    {
        fwrite(&value, sizeof(value), 1, binaryOutput);
        fieldCount++;
    }

    if (!feof(textInput) || ((fieldCount % RECORD_FIELD_COUNT) != 0))
    {
        fprintf(stderr, "The input file had incorrect format\n");
        return false;
    }

    int recordCount = fieldCount / RECORD_FIELD_COUNT;
    int maxIdx      = idxTwo > idxOne ? idxTwo : idxOne;
    if (recordCount <= maxIdx)
    {
        fprintf(stderr, "The input file had too few records; expected: %d, actual: %d\n", maxIdx, recordCount);
        return false;
    }

    return true;
}

void SwapRecords(FILE* binaryOutput, int idxOne, int idxTwo)
{    
    int byteIdxOne = idxOne * sizeof(Record);
    int byteIdxTwo = idxTwo * sizeof(Record);

    // Read the two records. Note we don't harden this code against
    // random file errors (e. g. someone pulling the disk out).
    Record recordOne;
    fseek(binaryOutput, byteIdxOne, SEEK_SET);
    fread(&recordOne, sizeof(recordOne), 1, binaryOutput);

    Record recordTwo;
    fseek(binaryOutput, byteIdxTwo, SEEK_SET);
    fread(&recordTwo, sizeof(recordTwo), 1, binaryOutput);

    // Write the two records. We could "optimize" this by reading and
    // writing one record at the same time; we presume seeking is cheap.
    fseek(binaryOutput, byteIdxOne, SEEK_SET);
    fwrite(&recordTwo, sizeof(recordTwo), 1, binaryOutput);

    fseek(binaryOutput, byteIdxTwo, SEEK_SET);
    fwrite(&recordOne, sizeof(recordOne), 1, binaryOutput);
}

void PrintOutput(FILE* binaryOutput)
{
    fseek(binaryOutput, 0, SEEK_SET);
    printf(" { ");

    int             count = 0;
    RecordFieldType value;
    while (fread(&value, sizeof(value), 1, binaryOutput) == 1)
    {
        if ((count != 0) && (count % RECORD_FIELD_COUNT) == 0)
        {
            printf("}\n { ");
        }

        printf(RECORD_FIELD_FMT " ", value);
        count++;
    }

    printf("}\n");
}

int Exit(int exitCode, FILE* inputFile, FILE* outputFile)
{
    if (inputFile != nullptr)
    {
        fclose(inputFile);
    }
    if (outputFile != nullptr)
    {
        fclose(outputFile);
    }

    return exitCode;
}

int main(int argc, char* argv[])
{
    FILE* textInput = OpenTextInput(argc, argv);
    if (textInput == nullptr)
    {
        return Exit(-1, textInput, nullptr);
    }

    FILE* binaryOutput = OpenBinaryOutput(argc, argv);
    if (binaryOutput == nullptr)
    {
        return Exit(-2, textInput, binaryOutput);
    }

    int idxOne;
    int idxTwo;
    if (!GetUserArg(argc, argv, RECORD_ONE_IDX_ARG_IDX, &idxOne) ||
        !GetUserArg(argc, argv, RECORD_TWO_IDX_ARG_IDX, &idxTwo))
    {
        fprintf(stderr, "No record indices provided\n");
        return Exit(-3, textInput, binaryOutput);
    }

    if (!ConvertTextToBinary(textInput, binaryOutput, idxOne, idxTwo))
    {
        return Exit(-4, textInput, binaryOutput);
    }

    printf("Before swapping the records %d and %d:\n", idxOne, idxTwo);
    PrintOutput(binaryOutput);
    printf("\n");

    SwapRecords(binaryOutput, idxOne, idxTwo);

    printf("After swapping the records %d and %d:\n", idxOne, idxTwo);
    PrintOutput(binaryOutput);
    printf("\n");

    return Exit(0, textInput, binaryOutput);
}
