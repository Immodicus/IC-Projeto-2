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

        VERBOSE("Beginning encoding process...\n");

        BitStream out(argv[argc-1], "w+");

        std::vector<int16_t> predictions(img.rows * img.cols * 3);
        predictions.clear();

        uint64_t totalDiff = 0;
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

                    totalDiff += abs(rDiff);
                    totalDiff += abs(gDiff);
                    totalDiff += abs(bDiff);

                    predictions.push_back(rDiff);
                    predictions.push_back(gDiff);
                    predictions.push_back(bDiff);
                }
                else
                {
                    xr = x[0];
                    xg = x[1];
                    xb = x[2];

                    totalDiff += abs(xr);
                    totalDiff += abs(xg);
                    totalDiff += abs(xb);

                    predictions.push_back(xr);
                    predictions.push_back(xg);
                    predictions.push_back(xb);
                }
            }
        }

        m = std::ceil((double)totalDiff / (double)(img.rows * img.cols * 3));

        VERBOSE("Image is: " << img.rows << " x " << img.cols << "\n");
        VERBOSE("Auto determined m is: " << m << "\n");
        
        assert(out.Write(img.rows));
        assert(out.Write(img.cols));
        assert(out.Write(m));

        uint64 p = 0;
        for(size_t row = 0; row < img.rows; row++)
        {
            for(size_t col = 0; col < img.cols; col++)
            {
                out.WriteNBits(GolombCoder::Encode(predictions[p++], m));
                out.WriteNBits(GolombCoder::Encode(predictions[p++], m));
                out.WriteNBits(GolombCoder::Encode(predictions[p++], m));
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
        assert(in.Read(m));

        VERBOSE("Beginning decoding process...\n");
        VERBOSE("Image is: " << rows << " x " << cols << "\n");
        VERBOSE("m is: " << m << "\n");

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

    VERBOSE("Done\n");

    return EXIT_SUCCESS;
}