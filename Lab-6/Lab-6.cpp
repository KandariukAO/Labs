// Лабораторная работа #6, вариант 10, Кандарюк Артем, А-16-21

#include "../Common/commondefs.h"
#include "Lab-6.h"

int main(int argc, char* argv[])
{
	const char* const ARRAYS_MODE = "arrays";
	const char* const MATRICES_MODE = "matrices";

    const char* mode = GetUserArg(argc, argv, 0);
	if ((mode == nullptr) || (!AreEqual(mode, ARRAYS_MODE) && !AreEqual(mode, MATRICES_MODE)))
	{
		fprintf(stderr, "Mode provided: '%s', expected: '%s' or '%s'", mode, ARRAYS_MODE, MATRICES_MODE);
		return -1;
	}

	argv++;
	argc--;
	if (AreEqual(mode, ARRAYS_MODE))
	{
		return ArraysImpl(argc, argv);
	}

	assert(AreEqual(mode, MATRICES_MODE));
	return MatricesImpl(argc, argv);
}