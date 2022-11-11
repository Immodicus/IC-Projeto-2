#ifndef GOLOMB_H
#define GOLOMB_H

#include "BitStream.cpp"
#include <math.h>
#include <string>
using namespace std;

/**
 * Class for encoding and decoding values using golomb code.
 */
class Golomb
{
private:
    /* data */
    BitStream bitStream_r_w;
    int m; // the parameter that depend Golomb code - m > 0
    int b; // the binary value

public:
    /**
     * Golomb Class contructor
     * @param m_value represent the value of m that will be used in Golomb to encoding and decoding
     * @param filename reprenet the name of file used to store the encoded value or to read a decoded value
     * @param mode to represent the string "encode" for encoding value and "decode" for decoding value
     */
    Golomb();
    Golomb(const char *filename, char mode, int m_value);

    /**
     * encode method - can be positive or negative
     * @param n to reprent an integer number that can be positve/negative using Golomb code
     * @return the number os bits for encoded number
     */
    int encode(int n);

    /**
     * decode method - using Golomb code
     * Is given when the constructor of the class Golomb is initialized
     * @return the decoded value
     */
    int decode();

    /**
     * Convert an integer (positive or negative) into positive - what technique?
     * @param n integer number to be converted
     * @return converted value
     * */
    int fold(int n);

    /**
     * Convert an integer (positive or negative) into positive - what technique?
     * @param n integer number to be unconverted
     * @return unconverted value
     * */
    int unfold(int n);

    /**
     * Encode the golomb parameter m in binary (write it in the binary file)
     * @param n integer value that represents the m
     * */
    void encodeM(int n);

    /**
     * Read in binary the value of the golomb m
     * @return an integer that represents the m
     * */
    int decodeM();

    /**
     * Set value of m
     * @param mi integer that represents the m
     * */
    void setM(int mi);

    /**
     * Encode in binary the header of a soundfile
     * @param nFrames number of frames of the audio file
     * @param sampleRate of the audio file
     * @param Channels of the audiofile
     * @param format of the audiofile
     * @param lossy boolean that indicates if coding option is lossy or lossless
     **/
    void encodeHeaderSound(int nFrames, int sampleRate, int Channels, int format, bool lossy);

    /**
     * Encode quantization step in binary to save it in the header of encoded file
     * @param shamt quantization step
     **/
    void encondeShamt(int shamt);

    /**
     * Decode quantization step
     * @return quantization step
     **/
    int decodeShamt();

    /**
     * Decode the binary header of the soundfile
     * @param arr integer array of size 5 that will contain codecopetion(lossy, lossless), nFrames, sampleRate, numChannels and format
     *
     **/
    void decodeHeaderSound(int arr[]);

    /**
     * Encode mode image header
     * @param mode 0 -> lossless, 1 -> lossy
     * */
    void encodeMode(int mode);

    /**
     * Decode mode image header
     * @return mode  0 -> lossless, 1 -> lossy
     * */
    int decodeMode();

    /**
     * Close the golomb stream.
     * */
    void close();
};
#endif