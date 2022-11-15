#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "BitStream.h"
#include "GolombCoder.h"

#define VERBOSE(txt) if(verbose) std::cout << txt;

using namespace cv;
int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cerr << "Invalid usage. Usage: img_codec fileIn fileOut\n";
        return 1;
    }

    bool encode = true;
    bool verbose = false;
    uint64_t m = 6;

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
        if(std::string(argv[n]) == "-v") 
        {
			verbose = true;
			break;
		}
    }

    if(encode)
    {
        Mat img = imread(argv[argc-2], IMREAD_ANYCOLOR);
        if(img.empty())
        {
            std::cout << "Could not read the image: " << argv[argc-2] << std::endl;
            return EXIT_FAILURE;
        }

        BitStream out(argv[argc-1], "w+");

        assert(out.Write(img.rows));
        assert(out.Write(img.cols));

        for(size_t row = 0; row < img.rows; row++)
        {            
            for(size_t col = 0; col < img.cols; col++)
            {
                uchar xr;
                uchar xg;
                uchar xb;

                Vec3b x = img.at<Vec3b>(row, col);

                if(row > 0 && col > 0)
                {
                    Vec3b c = img.at<Vec3b>(row-1, col-1);
                    Vec3b a = img.at<Vec3b>(row, col-1);
                    Vec3b b = img.at<Vec3b>(row-1, col);

                    if(c[0] >= std::max(a[0], b[0]))
                    {
                        xr = std::min(a[0], b[0]);
                    }
                    else if(c[0] <= std::min(a[0], b[0]))
                    {
                        xr = std::max(a[0], b[0]);
                    }
                    else
                    {
                        xr = a[0] + b[0] - c[0];
                    }

                    if(c[1] >= std::max(a[1], b[1]))
                    {
                        xg = std::min(a[1], b[1]);
                    }
                    else if(c[1] <= std::min(a[1], b[1]))
                    {
                        xg = std::max(a[1], b[1]);
                    }
                    else
                    {
                        xg = a[1] + b[1] - c[1];
                    }

                    if(c[2] >= std::max(a[2], b[2]))
                    {
                        xb = std::min(a[2], b[2]);
                    }
                    else if(c[2] <= std::min(a[2], b[2]))
                    {
                        xb = std::max(a[2], b[2]);
                    }
                    else
                    {
                        xb = a[2] + b[2] - c[2];
                    }

                    int16_t rDiff = x[0] - xr;
                    int16_t gDiff = x[1] - xg;
                    int16_t bDiff = x[2] - xb;

                    BitSet rBs = GolombCoder::Encode(rDiff, m);
                    BitSet gBs = GolombCoder::Encode(gDiff, m);
                    BitSet bBs = GolombCoder::Encode(bDiff, m);

                    assert(out.WriteNBits(rBs));
                    assert(out.WriteNBits(gBs));
                    assert(out.WriteNBits(bBs));
                }
                else
                {
                    xr = x[0];
                    xg = x[1];
                    xb = x[2];

                    BitSet rBs = GolombCoder::Encode(xr, m);
                    BitSet gBs = GolombCoder::Encode(xg, m);
                    BitSet bBs = GolombCoder::Encode(xb, m);

                    assert(out.WriteNBits(rBs));
                    assert(out.WriteNBits(gBs));
                    assert(out.WriteNBits(bBs));
                }
            }
        }
    }
    else
    {
        BitStream in(argv[argc-2], "r");

        int rows;
        int cols;
        assert(in.Read(rows));
        assert(in.Read(cols));

        Mat out(rows, cols, CV_8UC3);

        auto results = GolombCoder::Decode(in, m);

        size_t p = 0;
        for(size_t row = 0; row < rows; row++)
        {            
            for(size_t col = 0; col < cols; col++)
            {
                uchar xr;
                uchar xg;
                uchar xb;

                Vec3b x;
                
                if(row > 0 && col > 0)
                {
                    Vec3b c = out.at<Vec3b>(row-1, col-1);
                    Vec3b a = out.at<Vec3b>(row, col-1);
                    Vec3b b = out.at<Vec3b>(row-1, col);
                    
                    if(c[0] >= std::max(a[0], b[0]))
                    {
                        xr = std::min(a[0], b[0]);
                    }
                    else if(c[0] <= std::min(a[0], b[0]))
                    {
                        xr = std::max(a[0], b[0]);
                    }
                    else
                    {
                        xr = a[0] + b[0] - c[0];
                    }

                    if(c[1] >= std::max(a[1], b[1]))
                    {
                        xg = std::min(a[1], b[1]);
                    }
                    else if(c[1] <= std::min(a[1], b[1]))
                    {
                        xg = std::max(a[1], b[1]);
                    }
                    else
                    {
                        xg = a[1] + b[1] - c[1];
                    }

                    if(c[2] >= std::max(a[2], b[2]))
                    {
                        xb = std::min(a[2], b[2]);
                    }
                    else if(c[2] <= std::min(a[2], b[2]))
                    {
                        xb = std::max(a[2], b[2]);
                    }
                    else
                    {
                        xb = a[2] + b[2] - c[2];
                    }

                    x[0] = results[p++] + xr;
                    x[1] = results[p++] + xg;
                    x[2] = results[p++] + xb;
                }
                else
                {
                    x[0] = results[p++];
                    x[1] = results[p++];
                    x[2] = results[p++];
                }

                out.at<Vec3b>(row, col) = x;
            }
        }

        imwrite(argv[argc-1], out);
    }

    return EXIT_SUCCESS;
}