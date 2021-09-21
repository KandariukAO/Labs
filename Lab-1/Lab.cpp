// Лабораторная работа #1, вариант 9, Кандарюк Артем, А-16-20

#include <cstdlib>
#include <cstdio>
#include "variants.h"

int main(int argc, char* argv[])
{
    int variant = atoi(argv[1]);

    switch (variant)
    {
        case 1:
            return MainOne(argc, argv);
        case 2:
            return MainTwo(argc, argv);
        case 3:
            return MainThree(argc, argv);;
        case 4:
            return MainFour(argc, argv);;
        default:
            fprintf(stderr, "Wrong variant '%d' supplied! Valid values: 1, 2, 3, 4", variant);
    }
}