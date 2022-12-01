#include <stdio.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include <sndfile.hh>

#include "BitStream.h"
#include "GolombCoder.h"
#include "AudioPredictors.h"

#define VERBOSE(txt) if(verbose) std::cout << txt;

std::vector<int16_t> Quantize(const std::vector<int16_t>& vec, uint8_t bBits, uint64_t& totalDiff)
{
    std::vector<int16_t> results(vec.size());
    results.clear();

    totalDiff = 0;

    const double delta = pow(2, 16 - bBits);

    for(const auto r : vec)
    {
        double sample = static_cast<double>(r);

        double result = delta * (std::floor(sample / delta) + 0.5);

        int16_t res = static_cast<int16_t>(result) / delta;
        totalDiff += abs(res);

        results.push_back(res);
    }

    return results;
}

std::vector<int64_t> Dequantize(const std::vector<int64_t>& vec, uint8_t bBits)
{
    std::vector<int64_t> results(vec.size());
    results.clear();

    const double delta = pow(2, 16 - bBits);

    for(const auto r : vec)
    {   
        results.push_back(r * delta);
    }

    return results;
}

int main(int argc, char** argv)
{
    if(argc < 3) 
    {
		std::cerr << "Usage: wav_golomb [ -m [auto|value] (def. auto) ]\n";
        std::cerr << "                  [ -d (decode)]\n";
        std::cerr << "                  [ -l (lossy) nBits]\n";
        std::cerr << "                  [ -bs blockSize (def. 4096)]\n";
        std::cerr << "                  [ -range range div (def. 512 32)]\n";
		std::cerr << "                  fileIn fileOut\n";
		return 1;
	}

    bool encode = true;
    bool autoM = true;
    bool verbose = false;
    bool lossy = false;

    uint32_t nBits = 0;
    uint16_t m = 512;
    uint64_t blockSize = 4096;
    int64_t range = 512;
    int64_t div = 32;

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

    for(int n = 1 ; n < argc ; n++)
	{
        if(std::string(argv[n]) == "-bs") 
        {
			blockSize = atoi(argv[n+1]);
			break;
		}
    }

    for(int n = 1 ; n < argc ; n++)
	{
        if(std::string(argv[n]) == "-range") 
        {
			range = atoi(argv[n+1]);
            div = atoi(argv[n+2]);
			break;
		}
    }

    for(int n = 1 ; n < argc ; n++)
	{
        if(std::string(argv[n]) == "-l") 
        {
			lossy = true;
            nBits = atoi(argv[n+1]);
            if(nBits == 0 || nBits > 15) 
            {
                std::cerr << "Invalid number of bits\n";
                return EXIT_FAILURE;
            }
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
        std::cout << "3 - Third order polinomial predictor\n";
        std::cout << "4 - Inter Channel predictor\n\n";
        std::cout << "Predictor: ";
        std::cin >> predictor;

        if(lossy)
        {
            VERBOSE("Lossy compression selected\n");
            VERBOSE("nBits: " << nBits << "\n");
        }

        BitStream out(argv[argc-1], "w+");
        
        nChannels = sfhIn.channels();
        nFrames = sfhIn.frames();
        nSampleRate = sfhIn.samplerate();
        
        std::vector<int16_t> samples(blockSize * nChannels);
        uint64_t read = 0;

        assert(out.Write(blockSize));
        assert(out.Write(nBits));
        assert(out.Write(predictor));
        assert(out.Write(nChannels));
        assert(out.Write(nFrames));
        assert(out.Write(nSampleRate));

        uint64_t block = 0;
        while((read = sfhIn.readf(samples.data(), blockSize)) > 0)
        {
            samples.resize(read * nChannels);

            uint64_t writtenBits = 0;
            uint64_t totalDiff = 0;

            std::vector<int16_t> residuals;

            if(lossy)
            {
                samples = Quantize(samples, nBits, totalDiff);
            }

            residuals = AudioPredictors::Encode(samples, read, nChannels, totalDiff, predictor);

            if(autoM)
            {
                m = GolombCoder::EstimateM(residuals, range, div, true);

                VERBOSE("Estimated best m for block " << block++ << " is: " << m << "\n");
            }

            out.WriteAlign(false);
            assert(out.Write(m));

            for(const auto residual : residuals)
            {
                BitSet bs = GolombCoder::EncodeFold(residual, m);
                writtenBits += bs.size();

                assert(out.WriteNBits(bs));
            }

            VERBOSE("Written: " << writtenBits << " bits\n");

            samples.resize(blockSize * nChannels);
        }
        
    }
    else
    {       
        BitStream in(argv[argc-2], "r");

        assert(in.Read(blockSize));
        assert(in.Read(nBits));
        assert(in.Read(predictor));
        assert(in.Read(nChannels));
        assert(in.Read(nFrames));
        assert(in.Read(nSampleRate));
        assert(in.Read(m));

        VERBOSE("Predictor: " << predictor << "\n");

        if(nBits != 0)
        {
            VERBOSE("Lossy mode detected\n");
            VERBOSE("nBits: " << nBits << "\n");
        }

        SndfileHandle out { argv[argc-1], SFM_WRITE, SF_FORMAT_PCM_16 | SF_FORMAT_WAV, nChannels, (int)nSampleRate};

        std::vector<int64_t> residuals;

        while((residuals = GolombCoder::DecodeFold(in, m, blockSize * nChannels)).size() > 0)
        {
            uint64_t readFrames = residuals.size() / nChannels;
            
            in.ReadAlign();
            in.Read(m); // last read will fail but it's fine
            
            std::vector<int16_t> samples;

            if(nBits != 0)
            {
                residuals = Dequantize(residuals, nBits);
            }

            samples = AudioPredictors::Decode(residuals, readFrames, nChannels, predictor);
 
            out.writef(samples.data(), readFrames);
        }
    }

    VERBOSE("nChannels: " << nChannels << "\n");
    VERBOSE("nFrames: " << nFrames << "\n");
    VERBOSE("nSampleRate: " << nSampleRate << "\n");
    VERBOSE("blockSize: " << blockSize << "\n");

    return EXIT_SUCCESS;
}