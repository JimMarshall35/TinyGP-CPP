# TinyGP-CPP
C++ translation / port of the TinyGP java source code from the book "A field guide to genetic programming" by Riccardo Poli, William B. Langdon and Nicholas F. McPhee.

Not changed from the original in any significant way except these (attempts at) optimizations.
- Memory pool implemented to store individuals - should eliminate possibilty of fragmentation and is faster than using new and delete. But uses a huge block of memory.
- Multi threaded fitness evaluation function - fitness cases split among threads in a thread pool. However this is slower than the single threaded version... see        threadedFitnessFunction in tinyGP.cpp for comment

Hasn't been rigourously tested.

Original Java code by Riccardo Poli (rpoli@essex.ac.uk)

![alt text](https://github.com/JimMarshall35/TinyGP-CPP/blob/main/fitness_2_44.png?raw=true)
