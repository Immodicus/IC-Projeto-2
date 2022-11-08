#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <string.h>
#include "BitSet.h"


class GolombCoder
{
private:
    bool GetBit(int64_t c, int64_t pos)
    {
        return (c >> (63 - (pos % 64))) & 1;
    }

    static bool SGetBit(int64_t c, int64_t pos)
    {
        return (c >> (63 - (pos % 64))) & 1;
    }

    void SetBit(int64_t& c, int64_t pos, bool value) const
    {
        if (value)
        {
            c |= (1 << (63 - pos));
        }
        else
        {
            c &= ~(1 << (63 - pos));
        }
    }

    static void SSetBit(uint64_t& c, uint64_t pos, bool value)
    {
        if (value)
        {
            c |= (1 << (63 - pos));
        }
        else
        {
            c &= ~(1 << (63 - pos));
        }
    }


public:
    static BitSet Encode(int64_t i, uint64_t m)
    {
        int64_t q = i / m;
        uint64_t r = i % m;

        uint64_t b = std::floor(log2(static_cast<double>(m)));
        
        size_t s = q + 1;

        if(r < pow(2, b+1) - m) 
        {
            s += b;
        }
        else
        {
            s += (b + 1);
        }

        BitSet bs(s);

        for(size_t k = 0; k < q; k++)
        {
            bs.SetBit(k, false);
        }
        bs.SetBit(q, true);

        if(r < pow(2, b+1) - m) // code r in binary representation using b bits
        {
            for(size_t k = q+1, h = b-1; k < (q+1)+b; k++, h--)
            {
                bs.SetBit(k, SGetBit(r, 63-h));
            }
        }
        else // code the number r + 2 b + 1 − M {\displaystyle r+2^{b+1}-M} {\displaystyle r+2^{b+1}-M} in binary representation using b + 1 bits.
        {
            r = r + static_cast<uint64_t>(pow(2, b+1)) - m;

            for(size_t k = q+1, h = b; k < (q+1)+b+1; k++, h--)
            {
                bs.SetBit(k, SGetBit(r, 63-h));
            }
        }

        return bs;
    }

    static int64_t Decode(const BitSet& bs, uint64_t m)
    {
        uint64_t b = std::floor(log2(static_cast<double>(m)));
        
        uint64_t q = 0;

        for(; q < bs.size(); q++)
        {
            if(bs[q]) break;
        }

        uint64_t r = 0;

        for(size_t i = 0; i < b; i++)
        {
            SSetBit(r, 63-b+i+1, bs[q+1+i]);
        }

        if(r >= pow(2, b+1) - m)
        {
            r <<= 1;
            SSetBit(r, 63, bs[bs.size()-1]);
            r = r - pow(2, b+1) + m;
        }

        return q * m + r;
    }
};