#include "AudioCodec.hpp"
#include "Golomb.cpp"

#include <iostream>
#include <stdio.h>
#include <sndfile.h>
#include <math.h>
#include <vector>
#include "matplotlibcpp.h"
#include <map>

using namespace std;
namespace plt = matplotlibcpp;

AudioCodec::AudioCodec() {}

AudioCodec::AudioCodec(const char *filename)
{

    SNDFILE *infile;
    int readcount;
    short ch[2];

    if (!(infile = sf_open(filename, SFM_READ, &sfinfo)))
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

void AudioCodec::compress(const char *fileDst, int num, bool lossy, int shamt)
{

    ninput = num;

    if (lossy)
        preditorLossy(chs, shamt);
    else
        preditor(chs);

    cout << "start encoding..." << endl;

    // Golomb
    Golomb g(fileDst, 'e', 0);

    double m = 0;
    for (int i = 0; i < rn.size(); i++)
    {
        m += g.fold(rn[i]);
    }
    m = m / rn.size();

    m = (int)ceil(-1 / (log2(m / (m + 1))));

    g.setM(m);
    g.encodeM(m);
    g.encodeHeaderSound(sfinfo.frames, sfinfo.samplerate, sfinfo.channels, sfinfo.format, lossy);
    if (lossy)
        g.encondeShamt(shamt);

    for (int i = 0; i < rn.size(); i++)
    {
        g.encode(rn[i]);
    }
    g.close();
}

void AudioCodec::preditor(vector<short> vetSrc)
{
    vector<short> left;
    vector<short> right;
    for (int i = 0; i < chs.size() - 1; i += 2)
    {
        left.push_back(chs[i]);
        right.push_back(chs[i + 1]);
    }

    vector<short> xnl, xnr;

    if (ninput == 1)
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
            rn.push_back(left[i] - xnl[i]);
            rn.push_back(right[i] - xnr[i]);
        }
    }
    else if (ninput == 2)
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

void AudioCodec::preditorLossy(vector<short> vetSrc, int shamt)
{
    vector<short> left;
    vector<short> right;
    for (int i = 0; i < chs.size() - 1; i += 2)
    {
        left.push_back(chs[i]);
        right.push_back(chs[i + 1]);
    }

    vector<int> xnr, xnl;

    if (ninput == 1)
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
    else if (ninput == 2)
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

void AudioCodec::decompress(const char *fileSrc)
{
    cout << "start decoding..." << endl;

    Golomb g(fileSrc, 'd', 0);

    int m = g.decodeM();

    g.setM(m);

    int infoDeco[5];
    g.decodeHeaderSound(infoDeco);

    vector<short> resChs;
    vector<short> resl, resr;
    vector<short> resXl, resXr;

    // If lossy
    if (infoDeco[0] == 1)
    {
        int shamt = g.decodeShamt();
        for (int i = 0; i < infoDeco[1] * infoDeco[4]; i++)
            resChs.push_back(g.decode() << shamt);
    }
    else
    {
        for (int i = 0; i < infoDeco[1] * infoDeco[4]; i++)
            resChs.push_back(g.decode());
    }

    for (int i = 0; i < resChs.size() - 1; i += 2)
    {
        resl.push_back(resChs[i]);
        resr.push_back(resChs[i + 1]);
    }

    g.close();

    vector<short> resXN;
    vector<short> resHatXl, resHatXr;

    if (ninput == 1)
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
    else if (ninput == 2)
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

    SNDFILE *outfile = sf_open("out.wav", SFM_WRITE, &sfinfoOut);
    sf_count_t count = sf_write_short(outfile, &resXN[0], resXN.size());
    sf_write_sync(outfile);
    sf_close(outfile);
}

void AudioCodec::showHistEnt()
{

    // Calculate the entropy
    map<short, int> map_ori, map_afterPred;
    for (int i = 0; i < (int)chs.size(); i++)
    {
        map_ori[chs[i]]++;
        map_afterPred[rn[i]]++;
    }

    double entropy = 0;
    double p;
    for (auto i : map_ori)
    {
        p = (double)i.second / (double)chs.size();
        entropy += p * (-log2(p));
    }
    cout << "Entropy Original: " << entropy << endl;

    double entropy_pred = 0;
    for (auto i : map_afterPred)
    {
        p = (double)i.second / (double)rn.size();
        entropy_pred += p * (-log2(p));
    }
    cout << "\nEntropy of the residuals obtained after prediction: " << entropy_pred << endl;

    cout << "\nComparison between the 2 entropies: " << entropy_pred / entropy << endl;

    // show Histo
    map<short, int> map_pred;

    for (int i = 0; i < (int)rn.size() / 2; i += 2)
    {
        map_pred[rn[i]]++;
    }
    plt::figure(1);
    vector<int> samples;
    vector<int> occur;

    for (auto i : map_pred)
    {
        samples.push_back(i.first);
        occur.push_back(i.second);
    }
    plt::bar(samples, occur);
    plt::xlabel("Samples");
    plt::ylabel("Number of occurences");
    plt::title("Residuals obtained after prediction");
    plt::show();
}