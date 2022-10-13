// Лабораторная работа #7, вариант 10, Кандарюк Артем, А-16-21
//
// Написать функцию, которая в числе x устанавливает 0 в n бит,
// находящихся справа от позиции p, остальные биты остаются без
// изменения.
//

#include "../Common/commondefs.h"

typedef int NumberType;
const int BITS_IN_NUMBER = sizeof(NumberType) * 8;
#define NUMBER_TYPE_FMT_DECIMAL "%d"
#define NUMBER_TYPE_FMT_HEX "0x%X"

int BitwiseImpl(NumberType x, int p, int n)
{
    // Create the mask:
    // [000..111..000]
    //      |   |
    //      p p - n
    //
    // Overshifting is UB so the special case of N == BITS_IN_NUMBER must be handled separately.
    //
    int mask = (p == BITS_IN_NUMBER) ? -1 : ((1 << p) - 1); // [000..111..111]
    mask >>= p - n;                                         // [000..000..111]
    mask <<= p - n;                                         // [000..111..000]

    return x & ~mask;
}

int ReferenceImpl(NumberType x, int p, int n)
{
    for (int i = p - n; i < p; i++)
    {
        x &= ~(1 << i);
    }

    return x;
}

bool VerifyInputs(int p, int n)
{
    if ((p < 0) || (p > BITS_IN_NUMBER))
    {
        fprintf(stderr, "P expected: [%d..%d], actual: %d", 0, BITS_IN_NUMBER, p);
        return false;
    }

    if ((n < 0) || (n > p))
    {
        fprintf(stderr, "N expected: [%d..%d], actual: %d", 0, p, n);
        return false;
    }

    return true;
}

void PrintBinaryNumber(NumberType x)
{
    printf("0b");
    for (int i = BITS_IN_NUMBER - 1; i >= 0; i--) // Technically UB...
    {
        printf(NUMBER_TYPE_FMT_DECIMAL, (x >> i) & 1);
    }
}

int main(int argc, char* argv[])
{
    int p, n;
    NumberType x;
    if (!GetUserArg(argc, argv, 0, &x) || !GetUserArg(argc, argv, 1, &p) || !GetUserArg<NumberType>(argc, argv, 2, &n))
    {
        fprintf(stderr, "X/P/N not provided\n");
        return -1;
    }

    if (!VerifyInputs(p, n))
    {
        return -2;
    }

    NumberType y = BitwiseImpl(x, p, n);
    NumberType z = ReferenceImpl(x, p, n);
    dbgf("Bitwise: " NUMBER_TYPE_FMT_HEX ", reference: " NUMBER_TYPE_FMT_HEX "\n", y, z);
    assert(y == z);

    printf(" (binary)  : ");
    PrintBinaryNumber(y);
    printf("\n");
    printf(" (decimal) : " NUMBER_TYPE_FMT_DECIMAL " \n", y);
    printf(" (hex)     : " NUMBER_TYPE_FMT_HEX " \n", y);
}