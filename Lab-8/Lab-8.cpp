// Лабораторная работа #8, вариант 10, Кандарюк Артем, А-16-21

#include "../Common/commondefs.h"

// #define LAB8DLL_IMPORTS
#include "../Lab-8-Common/Lab-8-Impl.h"

#include <cerrno>

FILE* OpenInput(int argc, char* argv[])
{
    const char* inputFilePath = GetUserArg(argc, argv, 0);

    FILE* input;
    if (inputFilePath != nullptr)
    {
        input = fopen(inputFilePath, "r");

        if (input == nullptr)
        {
            fprintf(stderr, "Error opening the input file '%s': %s\n", inputFilePath, strerror(errno));
        }
    }
    else
    {
        input = stdin;
    }

    return input;
}

const char* ReadInputLine(char* inputLineBuffer, int inputLineBufferLength, FILE* input)
{
    char* inputLine = fgets(inputLineBuffer, inputLineBufferLength, input);
    if (inputLine == nullptr)
    {
        fprintf(stderr, "Error reading the input line; most likely the limit of %d chars has been exceeded\n", inputLineBufferLength);
        return nullptr;
    }

    // Remove the newline character from the input to make output look nicer.
    inputLine[strlen(inputLine) - 1] = '\0';

    return inputLine;
}

int Exit(int exitCode, FILE* input)
{
    if (input != nullptr)
    {
        fclose(input);
    }

    return exitCode;
}

int main(int argc, char* argv[])
{
    FILE* input = OpenInput(argc, argv);
    if (input == nullptr)
    {
        return Exit(-1, nullptr);
    }

    static const int MAX_LINE_LENGTH = 1024;
    char        inputLineBuffer[MAX_LINE_LENGTH + 1];
    const char* inputLine = ReadInputLine(inputLineBuffer, sizeof(inputLineBuffer), input);
    if (inputLine == nullptr)
    {
        return Exit(-2, input);
    }

    char outputLine[MAX_LINE_LENGTH + 1];
    RemoveDuplicateChars(inputLine, outputLine);

    printf("Input : '%s'\n", inputLine);
    printf("Output: '%s'\n", outputLine);

    return Exit(0, input);
}

