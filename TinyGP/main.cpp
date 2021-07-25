#include <iostream>
#include "TinyGP.h"

int main()
{
    TinyGP tgp("problem.dat", -1);
    tgp.evolve();
}