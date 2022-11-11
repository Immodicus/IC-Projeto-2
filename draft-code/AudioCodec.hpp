#ifndef AUDIOCODEC_H
#define AUDIOCODEC_H

#include <stdio.h>
#include <sndfile.h>
#include <vector>

class AudioCodec
{
private:
    /* data */
    char *fname;
    SF_INFO sfinfo;
    int num_in;
    std::vector<short> chs = {};
    std::vector<short> rn = {};

public:
    AudioCodec();
    /**
     * @brief Construct a new Audio Codec object
     *
     * @param fname, represent a path store filename
     */
    AudioCodec(const char *fname);

    /**
     *Compress Audio File
     * @brief
     * @param fDst repperesent the path store the filename destination
     * @param num to choose the order of the predictor
     * @param lossy to choose between lossless(0) and lossy(1) encoding
     * @param shamt represent the number of bits to be quantized in the predictor
     */
    void compress(const char *fDst, int num, bool lossy, int shamt);

    /**
     *Decompress Audio File
     * @brief
     * @param fSrc repperesent the path store the filename source
     */
    void decompress(const char *fSrc);

    /**
     * Lossless Predictor
     * @brief
     * @param vectSample is the vector that contains all samples
     */
    void losslessPredictor(std::vector<short> vectSample);

    /**
     * Lossy Predictor
     * @brief
     * @param vectSample is the vector that contains all samples
     * @param shamt describe the number of bits to be quantized in the predictor
     */
    void lossyPredictor(std::vector<short> vectSample, int shamt);
};

#endif