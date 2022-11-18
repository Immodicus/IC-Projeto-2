#pragma once

#include "BitSet.h"
#include "BitStream.h"

class AudioPredictors
{
public:
    static std::vector<int16_t> ThirdOrderPolEnc(const std::vector<int16_t>& samples, uint64_t nFrames, int nChannels, uint64_t& totalDiff)
    {
        std::vector<int16_t> residuals(nFrames * nChannels);
        residuals.clear();

        totalDiff = 0;

        for(size_t f = 0; f < nFrames; f++)
        {
            int16_t lsample = samples[f * 2];
            int16_t rsample = samples[f * 2 + 1];

            int16_t ldiff;
            int16_t rdiff;

            if(f > 2)
            {
                ldiff = lsample - (3 * samples[(f-1) * 2] - 3 * samples[(f-2) * 2] + samples[(f-3) * 2]);
                rdiff = rsample - (3 * samples[(f-1) * 2 + 1] - 3 * samples[(f-2) * 2 + 1] + samples[(f-3) * 2 + 1]);
            }
            else
            {
                ldiff = lsample;
                rdiff = rsample;
            }

            residuals.push_back(ldiff);
            residuals.push_back(rdiff);

            totalDiff += abs(ldiff);
            totalDiff += abs(rdiff);
        }

        return residuals;
    }

    static std::vector<int16_t> ThirdOrderPolDec(const std::vector<int64_t>& residuals, uint64_t nFrames, int nChannels)
    {
        std::vector<int16_t> samples(nChannels * nFrames);
        samples.clear();

        for(size_t f = 0; f < nFrames; f++)
        {
            int16_t ldiff = residuals[f * 2];
            int16_t rdiff = residuals[f * 2 + 1];

            int16_t lsample;
            int16_t rsample;

            if(f > 2)
            {
                lsample = ldiff + (3 * samples[(f-1) * 2] - 3 * samples[(f-2) * 2] + samples[(f-3) * 2]);
                rsample = rdiff + (3 * samples[(f-1) * 2 + 1] - 3 * samples[(f-2) * 2 + 1] + samples[(f-3) * 2 + 1]);
            }
            else
            {
                lsample = ldiff;
                rsample = rdiff;
            }
            
            samples.push_back(lsample);
            samples.push_back(rsample);
        }

        return samples;
    }

    static std::vector<int16_t> SecondOrderPolEnc(const std::vector<int16_t>& samples, uint64_t nFrames, int nChannels, uint64_t& totalDiff)
    {
        std::vector<int16_t> residuals(nFrames * nChannels);
        residuals.clear();

        totalDiff = 0;

        for(size_t f = 0; f < nFrames; f++)
        {
            int16_t lsample = samples[f * 2];
            int16_t rsample = samples[f * 2 + 1];

            int16_t ldiff;
            int16_t rdiff;

            if(f > 1)
            {
                ldiff = lsample - (2 * samples[(f-1) * 2] - samples[(f-2) * 2]);
                rdiff = rsample - (2 * samples[(f-1) * 2 + 1] - samples[(f-2) * 2 + 1]);
            }
            else
            {
                ldiff = lsample;
                rdiff = rsample;
            }

            residuals.push_back(ldiff);
            residuals.push_back(rdiff);

            totalDiff += abs(ldiff);
            totalDiff += abs(rdiff);
        }

        return residuals;
    }

    static std::vector<int16_t> SecondOrderPolDec(const std::vector<int64_t>& residuals, uint64_t nFrames, int nChannels)
    {
        std::vector<int16_t> samples(nChannels * nFrames);
        samples.clear();

        for(size_t f = 0; f < nFrames; f++)
        {
            int16_t ldiff = residuals[f * 2];
            int16_t rdiff = residuals[f * 2 + 1];

            int16_t lsample;
            int16_t rsample;

            if(f > 1)
            {
                lsample = ldiff + (2 * samples[(f-1) * 2] - samples[(f-2) * 2]);
                rsample = rdiff + (2 * samples[(f-1) * 2 + 1] - samples[(f-2) * 2 + 1]);
            }
            else
            {
                lsample = ldiff;
                rsample = rdiff;
            }
            
            samples.push_back(lsample);
            samples.push_back(rsample);
        }

        return samples;
    }

    static std::vector<int16_t> FirstOrderPolEnc(const std::vector<int16_t>& samples, uint64_t nFrames, int nChannels, uint64_t& totalDiff)
    {
        std::vector<int16_t> residuals(nFrames * nChannels);
        residuals.clear();

        totalDiff = 0;

        for(size_t f = 0; f < nFrames; f++)
        {
            int16_t lsample = samples[f * 2];
            int16_t rsample = samples[f * 2 + 1];

            int16_t ldiff;
            int16_t rdiff;

            if(f > 0)
            {
                ldiff = lsample - (samples[(f-1) * 2]);
                rdiff = rsample - (samples[(f-1) * 2 + 1]);
            }
            else
            {
                ldiff = lsample;
                rdiff = rsample;
            }

            residuals.push_back(ldiff);
            residuals.push_back(rdiff);

            totalDiff += abs(ldiff);
            totalDiff += abs(rdiff);
        }

        return residuals;
    }

    static std::vector<int16_t> FirstOrderPolDec(const std::vector<int64_t>& residuals, uint64_t nFrames, int nChannels)
    {
        std::vector<int16_t> samples(nChannels * nFrames);
        samples.clear();

        for(size_t f = 0; f < nFrames; f++)
        {
            int16_t ldiff = residuals[f * 2];
            int16_t rdiff = residuals[f * 2 + 1];

            int16_t lsample;
            int16_t rsample;

            if(f > 0)
            {
                lsample = ldiff + (samples[(f-1) * 2]);
                rsample = rdiff + (samples[(f-1) * 2 + 1]);
            }
            else
            {
                lsample = ldiff;
                rsample = rdiff;
            }
            
            samples.push_back(lsample);
            samples.push_back(rsample);
        }

        return samples;
    }

    static std::vector<int16_t> InterChannelEnc(const std::vector<int16_t>& samples, uint64_t nFrames, int nChannels, uint64_t& totalDiff)
    {
        std::vector<int16_t> residuals(nFrames * nChannels);
        residuals.clear();

        totalDiff = 0;

        for(size_t f = 0; f < nFrames; f++)
        {
            int16_t lsample = samples[f * 2];
            int16_t rsample = samples[f * 2 + 1];

            residuals.push_back(lsample);
            residuals.push_back(lsample - rsample);

            totalDiff += abs(lsample);
            totalDiff += abs(lsample - rsample);
        }

        return residuals;
    }

    static std::vector<int16_t> InterChannelDec(const std::vector<int64_t>& residuals, uint64_t nFrames, int nChannels)
    {
        std::vector<int16_t> samples(nChannels * nFrames);
        samples.clear();

        for(size_t f = 0; f < nFrames; f++)
        {
            int16_t lsample = residuals[f * 2];
            int16_t rdiff = residuals[f * 2 + 1];
            
            samples.push_back(lsample);
            samples.push_back(lsample - rdiff);
        }

        return samples;
    }
};