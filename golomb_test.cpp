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

    std::vector<int64_t> v(0);

    for(size_t i =0; i < 1000000; i++)
    {
        int32_t j = rand() % 32768;

        BitSet b = GolombCoder::Encode(j, 2000);
        out->WriteNBits(b);

        v.push_back(j);
    }

    delete out;

    BitStream in("test.col", "r");

    auto result = GolombCoder::Decode(in, 2000);

    size_t i =0;
    for(auto d : result)
    {
        assert(d == v[i++]);
    }

    return EXIT_SUCCESS;
}