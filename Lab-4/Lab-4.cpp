// Лабораторная работа #4, вариант 10, Кандарюк Артем, А-16-21

#include "../Common/commondefs.h"

#include <cstring>
#include <cerrno>
#include <cassert>

bool IsWordChar(char c)
{
    return (('0' <= c) && (c <= '9')) || (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
}

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

void RemoveDuplicateChars(const char* inputLine, char* outputLine)
{
    assert((inputLine != nullptr) && (outputLine != nullptr));

    // The algorithm is simple: walk the string, keeping track of any words
    // encountered. Once the end of a word has been reached, process it.
    // Since we expect the words to be relatively short, a simple 0(n^2)
    // loop is ok. We could do better with a bitmap-based approach, or some
    // fancier alogrithm. For now, we defer.
    int outputIndex = 0;
    for (int i = 0, currentWordLength = 0; inputLine[i] != '\0'; i++)
    {
        char currentChar = inputLine[i];
        if (IsWordChar(currentChar))
        {
            currentWordLength++;
            continue;
        }

        // We have a word - backtrack. Note this will properly no-op in case "currentWordLength" is zero.
        int wordStartIndex = i - currentWordLength;
        for (int j = wordStartIndex; j < i; j++)
        {
            // Write this char out, but only if has not been duplicated.
            char currentWordChar = inputLine[j];
            bool isUnique        = true;
            for (int k = wordStartIndex; k < i; k++)
            {
                if ((k != j) && (inputLine[k] == currentWordChar))
                {
                    isUnique = false;
                    break;
                }
            }

            if (isUnique)
            {
                outputLine[outputIndex++] = currentWordChar;
            }
        }

        // Make sure to write out the non-word character as well.
        outputLine[outputIndex++] = currentChar;

        // Reset the counter.
        currentWordLength = 0;
    }

    // Null-terminate the result.
    outputLine[outputIndex] = '\0';
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
