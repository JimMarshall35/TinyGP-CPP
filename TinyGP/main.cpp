#include <iostream>
#include "TinyGP.h"

int main()
{
    TinyGP tgp("problem.dat", 2);
    tgp.evolve();
}