#ifndef AUDIOCODEC_H
#define AUDIOCODEC_H

#include <stdio.h>
#include <sndfile.h>
#include <vector>

/**
 * Class for encoding and decoding audio files.
 */

class AudioCodec
{
private:
    char *filename;
    SF_INFO sfinfo;
    int ninput;
    std::vector<short> chs = {};
    std::vector<short> rn = {};

public:
    AudioCodec();

    /**
     * AudioCodec Class Constructor
     * @param filename path to an audio file.
     */
    AudioCodec(const char *filename);

    /**
     * Compress audio file
     * @param filename path to a file were to store the enconded value.
     * @param num value to choose the order of the predictor that will be use.
     * Choosing 1, the predictor will only consider the previous value; 2 it will consider the 2 previous values; and choosing 3 will consider the 3 previous values.
     * @param lossy value to choose between lossless (0) and lossy (1) encoding.
     * @param shamt number of bits to be quantized in the predictor
     */
    void compress(const char *fileDst, int num, bool lossy, int shamt);

    /**
     * Lossless Preditor
     * @param vetSrc vector that contains all the samples of the audio file.
     */
    void preditor(std::vector<short> vetSrc);

    /**
     * Lossy Preditor
     * @param vetSrc vector that contains all the samples of the audio file.
     * @param shamt number of bits to be quantized in the predictor
     */
    void preditorLossy(std::vector<short> vetSrc, int shamt);

    /**
     * Decompress audio file
     * @param fileSrc path to a file were the stored enconded value is.
     */
    void decompress(const char *fileSrc);

    /**
     * Show the histograms of the residuals obtained after prediction, as well as the entropy value.
     * Compare the values with the entropy of the original data.
     */
    void showHistEnt();
};

#endif