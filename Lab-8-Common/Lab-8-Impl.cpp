#include <cassert>
#include "Lab-8-Impl.h"

bool IsWordChar(char c)
{
    return (('0' <= c) && (c <= '9')) || (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
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