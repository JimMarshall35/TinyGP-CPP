#include <iostream>
#include "TinyGP.h"

int main()
{
    TinyGP tgp("problem.dat", 24071995);
    tgp.evolve();
}