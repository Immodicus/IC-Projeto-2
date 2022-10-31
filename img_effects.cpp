#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

typedef enum Effect
{
    Invert,
    Rotate,
    MirrorHorizontal,
    MirrorVertical,
    Brightness
} Effect;

using namespace cv;
int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cerr << "Invalid usage. Usage: img_cpy original copy\n";
        return 1;
    }

    Effect effect = MirrorVertical;

    std::string image_path = samples::findFile(argv[argc-2]);

    Mat img = imread(image_path, IMREAD_ANYCOLOR);
    if(img.empty())
    {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    Mat out(img.rows, img.cols, CV_8UC3);

    if(effect == Invert)
    {
        for(size_t i = 0; i < img.cols; i++)
        {
            for(size_t j = 0; j < img.rows; j++)
            {            
                Vec3b p = img.at<Vec3b>(j, i);
                uchar red = 255 - p[0];
                uchar green = 255 - p[1];
                uchar blue = 255 - p[2];

                out.at<Vec3b>(j, i) = Vec3b(red, green, blue);
            }
        }
    }
    else if(effect == MirrorHorizontal)
    {
        for(size_t i = 0; i < img.cols; i++)
        {
            for(size_t j = 0; j < img.rows; j++)
            {            
                out.at<Vec3b>(j, i) = img.at<Vec3b>(j, img.cols - i - 1);
            }
        }
    }
    else if(effect == MirrorVertical)
    {
        for(size_t i = 0; i < img.cols; i++)
        {
            for(size_t j = 0; j < img.rows; j++)
            {            
                out.at<Vec3b>(j, i) = img.at<Vec3b>(img.rows - j - 1, i);
            }
        }
    }

    imwrite(argv[argc-1], out);
    return 0;
}