#include "GolombCoder.h"
#include <stdio.h>

int main()
{
    for(size_t m = 2; m < 512; m++)
    {
        for(int64_t n = 0; n < 1000 * 128; n++)
        {
            BitSet b = GolombCoder::Encode(n, m);
            //std::cout << "i: " << n << "\n";
            assert(n == GolombCoder::Decode(b, m));
        }
        std::cout << "m: " << m << " OK\n";
    }

    return EXIT_SUCCESS;
}