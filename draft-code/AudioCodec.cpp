#include "AudioCodec.hpp"
#include "Golomb.cpp"

#include <iostream>
#include <stdio.h>
#include <sndfile.h>
#include <math.h>
#include <vector>
#include <map>

using namespace std;

AudioCodec::AudioCodec() {}

AudioCodec::AudioCodec(const char *fname)
{
    SNDFILE *infile;
    int readcount;
    short ch[2];

    if (!(infile = sf_open(fname, SFM_READ, &sfinfo)))
    {
        cout << "File doesn't exists" << endl;
        exit(EXIT_FAILURE);
    }

    while ((readcount = (int)sf_readf_short(infile, ch, 1)) > 0)
    {
        chs.push_back(ch[0]);
        chs.push_back(ch[1]);
    }

    sf_close(infile);
}
void AudioCodec::compress(const char *fDst, int num, bool lossy, int shamt)
{
    num_in = num;
    if (lossy)
    {
        lossyPredictor(chs, shamt);
    }
    else
    {
        losslessPredictor(chs);
    }
    cout << "start encoding..." << endl;

    // now we use Golomb code
    Golomb gol(fDst, 'e', 0);

    double m = 0;
    for (int i = 0; i < rn.size(); i++)
    {
        m = m + gol.fold(rn[i]);
    }
    m = m / rn.size();
    m = (int)ceil(-1 / log2(m / (m + 1)));

    gol.setM(m);
    gol.encodeM(m);
    gol.encodeHeaderSound(sfinfo.frames, sfinfo.samplerate, sfinfo.channels, sfinfo.format, lossy);
    if (lossy)
    {
        gol.encondeShamt(shamt);

        for (int i = 0; i < rn.size(); i++)
        {
            gol.encode(rn[i]);
        }
        gol.close();
    }
}
void AudioCodec::decompress(const char *fSrc)
{
    cout << "start decoding..." << endl;

    Golomb gol(fSrc, 'd', 0);

    int m = gol.decodeM();

    gol.setM(m);

    int infoDeco[5];
    gol.decodeHeaderSound(infoDeco);

    vector<short> resChs;
    vector<short> resl, resr;
    vector<short> resXl, resXr;

    // If lossy
    if (infoDeco[0] == 1)
    {
        int shamt = gol.decodeShamt();
        for (int i = 0; i < infoDeco[1] * infoDeco[4]; i++)
            resChs.push_back(gol.decode() << shamt);
    }
    else
    {
        for (int i = 0; i < infoDeco[1] * infoDeco[4]; i++)
            resChs.push_back(gol.decode());
    }

    for (int i = 0; i < resChs.size() - 1; i += 2)
    {
        resl.push_back(resChs[i]);
        resr.push_back(resChs[i + 1]);
    }

    gol.close();

    vector<short> resXN;
    vector<short> resHatXl, resHatXr;

    if (num_in == 1)
    {
        resXl.push_back(resl[0]);
        resXr.push_back(resr[0]);
        resXN.push_back(resl[0]);
        resXN.push_back(resr[0]);
        for (int i = 1; i < resl.size(); i++)
        {
            resXl.push_back((short)resl[i] + resXl[i - 1]);
            resXr.push_back((short)resr[i] + resXr[i - 1]);
            resXN.push_back(resXl[i]);
            resXN.push_back(resXr[i]);
        }
    }
    else if (num_in == 2)
    {
        for (int i = 0; i < 2; i++)
        {
            resHatXl.push_back(0);
            resHatXr.push_back(0);
            resXl.push_back(resl[i]);
            resXr.push_back(resr[i]);
            resXN.push_back(resXl[i]);
            resXN.push_back(resXr[i]);
        }
        for (int i = 2; i < resl.size(); i++)
        {
            resHatXl.push_back((int)(2 * resXl[i - 1] - resXl[i - 2]));
            resHatXr.push_back((int)(2 * resXr[i - 1] - resXr[i - 2]));
            resXl.push_back((short)resl[i] + resHatXl[i]);
            resXr.push_back((short)resr[i] + resHatXr[i]);
            resXN.push_back(resXl[i]);
            resXN.push_back(resXr[i]);
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            resHatXl.push_back(0);
            resHatXr.push_back(0);
            resXl.push_back(resl[i]);
            resXr.push_back(resr[i]);
            resXN.push_back(resXl[i]);
            resXN.push_back(resXr[i]);
        }
        for (int i = 3; i < resl.size(); i++)
        {
            resHatXl.push_back((int)(3 * resXl[i - 1] - 3 * resXl[i - 2] + resXl[i - 3]));
            resHatXr.push_back((int)(3 * resXr[i - 1] - 3 * resXr[i - 2] + resXr[i - 3]));
            resXl.push_back((short)resl[i] + resHatXl[i]);
            resXr.push_back((short)resr[i] + resHatXr[i]);
            resXN.push_back(resXl[i]);
            resXN.push_back(resXr[i]);
        }
    }

    SF_INFO sfinfoOut;
    sfinfoOut.channels = infoDeco[4];
    sfinfoOut.samplerate = infoDeco[2];
    sfinfoOut.format = infoDeco[3];
    sfinfoOut.frames = infoDeco[1];

    SNDFILE *outfile = sf_open("fout.wav", SFM_WRITE, &sfinfoOut);
    sf_count_t count = sf_write_short(outfile, &resXN[0], resXN.size());
    sf_write_sync(outfile);
    sf_close(outfile);
}
void AudioCodec::losslessPredictor(vector<short> vectSample)
{
    vector<short> left;
    vector<short> right;
    vector<short> xnl, xnr;
    for (int i = 0; i < chs.size() - 1; i += 2)
    {
        left.push_back(chs[i]);
        right.push_back(chs[i + 1]);
    }
    if (num_in == 1)
    {
        for (int i = 0; i < left.size(); i++)
        {
            if (i == 0)
            {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else
            {
                xnl.push_back(left[i - 1]);
                xnr.push_back(right[i - 1]);
            }
            xnl.push_back(left[i] - xnl[i]);
            xnr.push_back(right[i] - xnr[i]);
        }
    }
    else if (num_in == 2)
    {
        for (int i = 0; i < left.size(); i++)
        {
            if (i == 0 || i == 1)
            {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else
            {
                xnl.push_back(2 * left[i - 1] - left[i - 2]);
                xnr.push_back(2 * right[i - 1] - right[i - 2]);
            }
            rn.push_back(left[i] - xnl[i]);
            rn.push_back(right[i] - xnr[i]);
        }
    }
    else
    {
        for (int i = 0; i < left.size(); i++)
        {
            if (i == 0 || i == 1 || i == 2)
            {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else
            {
                xnl.push_back(3 * left[i - 1] - 3 * left[i - 2] + left[i - 3]);
                xnr.push_back(3 * right[i - 1] - 3 * right[i - 2] + right[i - 3]);
            }
            rn.push_back(left[i] - xnl[i]);
            rn.push_back(right[i] - xnr[i]);
        }
    }
}
void AudioCodec::lossyPredictor(std::vector<short> vectSample, int shamt)
{
    vector<short> left;
    vector<short> right;
    vector<int> xnr, xnl;
    for (int i = 0; i < chs.size() - 1; i += 2)
    {
        left.push_back(chs[i]);
        right.push_back(chs[i + 1]);
    }

    if (num_in == 1)
    {
        for (int i = 0; i < left.size(); i++)
        {
            if (i == 0)
            {
                xnr.push_back(0);
                xnl.push_back(0);
            }
            else
            {
                xnl.push_back(left[i - 1]);
                xnr.push_back(right[i - 1]);
            }
            rn.push_back(((left[i] - xnl[i]) >> shamt));
            rn.push_back(((right[i] - xnr[i]) >> shamt));
            left[i] = (rn[2 * i] << shamt) + xnl[i];
            right[i] = (rn[2 * i + 1] << shamt) + xnr[i];
        }
    }
    else if (num_in == 2)
    {
        for (int i = 0; i < left.size(); i++)
        {
            if (i == 0 || i == 1)
            {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else
            {
                xnl.push_back(2 * left[i - 1] - left[i - 2]);
                xnr.push_back(2 * right[i - 1] - right[i - 2]);
            }
            rn.push_back(((left[i] - xnl[i]) >> shamt));
            rn.push_back(((right[i] - xnr[i]) >> shamt));
            left[i] = (rn[2 * i] << shamt) + xnl[i];
            right[i] = (rn[2 * i + 1] << shamt) + xnr[i];
        }
    }
    else
    {
        for (int i = 0; i < left.size(); i++)
        {
            if (i == 0 || i == 1 || i == 2)
            {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else
            {
                xnl.push_back(3 * left[i - 1] - 3 * left[i - 2] + left[i - 3]);
                xnr.push_back(3 * right[i - 1] - 3 * right[i - 2] + right[i - 3]);
            }
            rn.push_back(((left[i] - xnl[i]) >> shamt));
            rn.push_back(((right[i] - xnr[i]) >> shamt));
            left[i] = (rn[2 * i] << shamt) + xnl[i];
            right[i] = (rn[2 * i + 1] << shamt) + xnr[i];
        }
    }
}
