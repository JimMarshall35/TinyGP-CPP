#include <iostream>
#include "TinyGP.h"

int main()
{
    TinyGP tgp("problem.dat", 42069);
    tgp.evolve();
}