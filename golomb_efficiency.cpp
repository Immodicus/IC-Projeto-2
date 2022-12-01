#include <stdio.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include <sndfile.hh>

#include "GolombCoder.h"
#include "BitStream.h"

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cerr << "Usage: golomb_efficiency wavFile\n";
        return EXIT_FAILURE;
    }
    
    SndfileHandle audioFile(argv[1]);
    BitStream bs_m1("test_m1.out", "w+");
    BitStream bs_m2("test_m2.out", "w+");

    std::vector<int16_t> samples(audioFile.frames() * audioFile.channels());

    audioFile.readf(samples.data(), audioFile.frames());

    uint64_t m1 = GolombCoder::EstimateMBrute(samples, 5000);

    uint64_t m2 = GolombCoder::EstimateMBrute(samples, 5000, true);

    std::cout << "m1: " << m1 << std::endl;
    std::cout << "m2: " << m2 << std::endl;

    uint64_t writtenBits_m1 = 0;
    uint64_t writtenBits_m2 = 0;

    for(const auto sample : samples)
    {
        BitSet b1 = GolombCoder::Encode(sample, m1);
        writtenBits_m1 += b1.size();

        assert(GolombCoder::Decode(b1, m1) == sample);

        BitSet b2 = GolombCoder::EncodeFold(sample, m2);
        writtenBits_m2 += b2.size();

        assert(GolombCoder::DecodeFold(b2, m2) == sample);

        assert(bs_m1.WriteNBits(b1));
        assert(bs_m2.WriteNBits(b2));
    }

    std::cout << "m1 entropy is: " << (double)writtenBits_m1 / (audioFile.frames() * audioFile.channels()) << " bits/samples\n";
    std::cout << "m2 entropy is: " << (double)writtenBits_m2 / (audioFile.frames() * audioFile.channels()) << " bits/samples\n";

    return EXIT_SUCCESS;
}