#include "GolombCoder.h"
#include "BitStream.h"
#include <stdio.h>

int main()
{
    for(size_t m = 2; m < 512; m++)
    {
        for(int64_t n = -(1000 * 128); n < 1000 * 128; n++)
        {
            BitSet b = GolombCoder::Encode(n, m);
            //std::cout << "i: " << n << "\n";
            assert(n == GolombCoder::Decode(b, m));
        }
        std::cout << "m: " << m << " OK\n";
    }

    BitStream* out = new BitStream("test.col", "w+");

    uint64_t a = 5;
    int64_t b = -5;
    uint64_t c = 453;
    int64_t d = -453;
    uint64_t e = 136453;
    int64_t f = -136453;
    uint64_t g = 5000000;
    int64_t h = -5000000;

    uint64_t m = 256;

    assert(out->WriteNBits(GolombCoder::Encode(a, m)));
    assert(out->WriteNBits(GolombCoder::Encode(b, m)));
    assert(out->WriteNBits(GolombCoder::Encode(c, m)));
    assert(out->WriteNBits(GolombCoder::Encode(d, m)));
    assert(out->WriteNBits(GolombCoder::Encode(e, m)));
    assert(out->WriteNBits(GolombCoder::Encode(f, m)));
    assert(out->WriteNBits(GolombCoder::Encode(g, m)));
    assert(out->WriteNBits(GolombCoder::Encode(h, m)));

    delete out;

    BitStream in("test.col", "r");

    auto result = GolombCoder::Decode(in, m);

    for(auto d : result)
    {
        std::cout << d << "\n";;
    }

    return EXIT_SUCCESS;
}