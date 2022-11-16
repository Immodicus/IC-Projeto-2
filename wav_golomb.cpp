#include <stdio.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include <sndfile.hh>

#include "BitStream.h"
#include "GolombCoder.h"
#include "AudioPredictors.h"

#define VERBOSE(txt) if(verbose) std::cout << txt;

int main(int argc, char** argv)
{
    if(argc < 3) 
    {
		std::cerr << "Usage: wav_golomb [ -m [auto|value] (def. auto) ]\n";
        std::cerr << "                  [ -d (decode)]\n";
		std::cerr << "                  fileIn fileOut\n";
		return 1;
	}

    bool encode = true;
    bool autoM = true;
    bool verbose = false;

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

    for(int n = 1 ; n < argc ; n++)
	{
        if(std::string(argv[n]) == "-v") 
        {
			verbose = true;
			break;
		}
    }

    uint16_t predictor;

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

        std::cout << "Please select a predictor\n";
        std::cout << "1 - First order polinomial predictor\n";
        std::cout << "2 - Second order polinomial predictor\n";
        std::cout << "3 - Third order polinomial predictor\n\n";
        std::cout << "Predictor: ";
        std::cin >> predictor;

        BitStream out(argv[argc-1], "w+");
        
        nChannels = sfhIn.channels();
        nFrames = sfhIn.frames();
        nSampleRate = sfhIn.samplerate();
        
        std::vector<int16_t> samples(sfhIn.frames() * sfhIn.channels());
        sfhIn.readf(samples.data(), sfhIn.frames());

        uint64_t writtenBits = 0;
        uint64_t totalDiff = 0;

        std::vector<int16_t> residuals;
        if(predictor == 3)
        {
            residuals = AudioPredictors::ThirdOrderPolEnc(samples, nFrames, nChannels, totalDiff);
        }
        else if(predictor == 2)
        {
            residuals = AudioPredictors::SecondOrderPolEnc(samples, nFrames, nChannels, totalDiff);
        }
        else
        {
            residuals = AudioPredictors::FirstOrderPolEnc(samples, nFrames, nChannels, totalDiff);
        }

        if(autoM)
        {
            m = std::ceil((double)totalDiff / (double)samples.size());

            VERBOSE("Estimated best m is " << m << std::endl);
        }

        assert(out.Write(predictor));
        assert(out.Write(m));
        assert(out.Write(nChannels));
        assert(out.Write(nFrames));
        assert(out.Write(nSampleRate));

        for(const auto residual : residuals)
        {
            BitSet bs = GolombCoder::Encode(residual, m);
            writtenBits += bs.size();

            assert(out.WriteNBits(bs));
        }

        VERBOSE("Written: " << writtenBits << " bits\n");
    }
    else
    {       
        BitStream in(argv[argc-2], "r");

        assert(in.Read(predictor));
        assert(in.Read(m));
        assert(in.Read(nChannels));
        assert(in.Read(nFrames));
        assert(in.Read(nSampleRate));

        VERBOSE("Predictor: " << predictor << "\n");

        SndfileHandle out { argv[argc-1], SFM_WRITE, SF_FORMAT_PCM_16 | SF_FORMAT_WAV, nChannels, nSampleRate};

        auto residuals = GolombCoder::Decode(in, m);

        std::vector<int16_t> samples;

        if(predictor == 3)
        {
            samples = AudioPredictors::ThirdOrderPolDec(residuals, nFrames, nChannels);
        }
        else if(predictor == 2)
        {
            samples = AudioPredictors::SecondOrderPolDec(residuals, nFrames, nChannels);
        }
        else
        {
            samples = AudioPredictors::FirstOrderPolDec(residuals, nFrames, nChannels);            
        }

        out.writef(samples.data(), nFrames);
    }

    VERBOSE("m: " << m << "\n");
    VERBOSE("nChannels: " << nChannels << "\n");
    VERBOSE("nFrames: " << nFrames << "\n");
    VERBOSE("nSampleRate: " << nSampleRate << "\n");

    return EXIT_SUCCESS;
}