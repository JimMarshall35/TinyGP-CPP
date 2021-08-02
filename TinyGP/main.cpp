#include <iostream>
#include "TinyGP.h"

int main()
{
    TinyGP tgp("problem.dat", 1313131313);
    tgp.evolve();
}