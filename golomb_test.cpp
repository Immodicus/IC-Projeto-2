#include "GolombCoder.h"
#include "BitStream.h"
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

    BitStream* out = new BitStream("test.col", "w+");

    int64_t a = 45-50;
    uint64_t b = 453;
    uint64_t c = 136453;
    uint64_t d = 5000000;

    uint64_t m = 256;

    out->WriteNBits(GolombCoder::Encode(a, m));
    out->WriteNBits(GolombCoder::Encode(b, m));
    out->WriteNBits(GolombCoder::Encode(c, m));
    out->WriteNBits(GolombCoder::Encode(d, m));

    delete out;

    BitStream in("test.col", "r");

    auto result = GolombCoder::Decode(in, m);

    for(auto d : result)
    {
        std::cout << d << "\n";;
    }

    return EXIT_SUCCESS;
}