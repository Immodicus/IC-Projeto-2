#include <stdio.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include <sndfile.hh>

#include "BitStream.h"
#include "GolombCoder.h"

int main(int argc, char** argv)
{
    if(argc < 3) 
    {
		std::cerr << "Usage: wav_golomb [ -m [auto|value] (def. auto) ]\n";
        std::cerr << "                  [ -d (decode)]\n";
		std::cerr << "                  wavFileIn wavFileOut\n";
		return 1;
	}

    bool encode = true;
    bool autoM = true;

    uint64_t m = 512;

    for(int n = 1 ; n < argc ; n++)
	{
        if(std::string(argv[n]) == "-d") 
        {
			encode = false;
			break;
		}
    }

    for(int n = 1 ; n < argc ; n++)
	{
        if(std::string(argv[n]) == "-m") 
        {
			autoM = false;
            m = atoi(argv[n+1]);
			break;
		}
    }

    uint16_t nChannels;
    uint64_t nFrames;
    uint32_t nSampleRate;

	if(encode)
    {
        SndfileHandle sfhIn { argv[argc-2] };
        if(sfhIn.error()) 
        {
            std::cerr << "Error: invalid input file\n";
            return 1;
        }

        if((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) 
        {
            std::cerr << "Error: file is not in WAV format\n";
            return 1;
        }

        if((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) 
        {
            std::cerr << "Error: file is not in PCM_16 format\n";
            return 1;
        }

        BitStream out(argv[argc-1], "w+");
        
        nChannels = sfhIn.channels();
        nFrames = sfhIn.frames();
        nSampleRate = sfhIn.samplerate();
        
        std::vector<int16_t> samples(sfhIn.frames() * sfhIn.channels());
        sfhIn.readf(samples.data(), sfhIn.frames());

        int16_t lastSample = 0;
        uint64_t writtenBits = 0;
        uint64_t totalDiff = 0;

        if(autoM)
        {
            for(const auto sample : samples)
            {
                int16_t diff = sample - lastSample;
                lastSample = sample;

                totalDiff += abs(diff);
            }

            m = std::ceil((double)totalDiff / (double)samples.size());

            std::cout << "Estimated best m is " << m << std::endl;
        }

        assert(out.Write(m));
        assert(out.Write(nChannels));
        assert(out.Write(nFrames));
        assert(out.Write(nSampleRate));

        lastSample = 0;
        for(const auto sample : samples)
        {
            int16_t diff = sample - lastSample;

            lastSample = sample;

            BitSet bs = GolombCoder::Encode(diff, m);
            writtenBits += bs.size();

            assert(out.WriteNBits(bs));
        }
    }
    else
    {       
        BitStream in(argv[argc-2], "r");

        assert(in.Read(m));
        assert(in.Read(nChannels));
        assert(in.Read(nFrames));
        assert(in.Read(nSampleRate));

        std::cout << "m: " << m << "\n";
        std::cout << "nChannels: " << nChannels << "\n";
        std::cout << "nFrames: " << nFrames << "\n";
        std::cout << "nSampleRate: " << nSampleRate << "\n";

        SndfileHandle out { argv[argc-1], SFM_WRITE, SF_FORMAT_PCM_16 | SF_FORMAT_WAV, nChannels, nSampleRate};

        auto results = GolombCoder::Decode(in, m);

        std::vector<int16_t> samples(nChannels * nFrames);
        samples.clear();

        int16_t lastSample = 0;
        for(const auto diff : results)
        {
            int16_t sample = lastSample + diff;
            lastSample = sample;
            
            samples.push_back(sample);
        }

        out.writef(samples.data(), nFrames);
    }
}