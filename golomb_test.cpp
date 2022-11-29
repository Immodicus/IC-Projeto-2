#include "GolombCoder.h"
#include "BitStream.h"
#include <stdio.h>

int main()
{
    for(size_t m = 2; m < 512; m++)
    {
        uint64_t rBits = 0;
        uint64_t fBits = 0;
        uint64_t cnt = 0;

        for(int64_t n = -(1000 * 128); n < 1000 * 128; n++, cnt++)
        {
            BitSet b = GolombCoder::Encode(n, m);
            BitSet b2 = GolombCoder::EncodeFold(n, m);

            if(n == 25 && m == 8)
            {
                std::cout << b;
                std::cout << b2;
                std::cout << "----------------------\n";
            }

            rBits += b.size();
            fBits += b2.size();

            assert(n == GolombCoder::Decode(b, m));
            assert(n == GolombCoder::DecodeFold(b2, m));
        }

        std::cout << "m: " << m << " OK. Avg bits regular: " << (double)rBits/cnt 
                  << " Avg bits fold: " << (double)fBits/cnt << "\n";
    }

    BitStream* out = new BitStream("test.col", "w+");
    BitStream* out2 = new BitStream("test2.col", "w+");

    std::vector<int64_t> v(0);

    for(size_t i = 0; i < 1000000; i++)
    {
        int32_t j = rand() % 32768;

        BitSet b = GolombCoder::Encode(j, 2000);
        out->WriteNBits(b);

        BitSet b2 = GolombCoder::EncodeFold(j, 2000);
        out2->WriteNBits(b2);

        v.push_back(j);
    }

    delete out;
    delete out2;

    BitStream in("test.col", "r");
    BitStream in2("test2.col", "r");

    auto result = GolombCoder::Decode(in, 2000);
    auto result2 = GolombCoder::DecodeFold(in2, 2000);

    for(size_t i = 0; i < result.size(); i++)
    {
        assert(result[i] == v[i]);
        assert(result2[i] == v[i]);
    }

    return EXIT_SUCCESS;
}