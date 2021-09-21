// Лабораторная работа #1, вариант 9, Кандарюк Артем, А-16-20

#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include "variants.h"

int main(int argc, char* argv[])
{
    int variant;
    if ((argc < 2) || (sscanf(argv[1], "%d", &variant) != 1) || (variant < 1) || (variant > 4))
    {
        fprintf(stderr, "The variant: 1, 2, 3 or 4, must be specified as the first argument!");
        return -1;
    }

    switch (variant)
    {
        case 1:
            return MainOne(argc, argv);
        case 2:
            return MainTwo(argc, argv);
        case 3:
            return MainThree(argc, argv);
        case 4:
            return MainFour(argc, argv);
        default:
            assert(!"Unreached");
            return -1;
    }
}