#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cerr << "Invalid usage. Usage: img_cpy original copy\n";
        return 1;
    }

    std::string image_path = samples::findFile(argv[argc-2]);

    Mat img = imread(image_path, IMREAD_ANYCOLOR);
    if(img.empty())
    {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    Mat out(img.rows, img.cols, CV_8UC3);

    for(size_t i = 0; i < img.cols; i++)
    {
        for(size_t j = 0; j < img.rows; j++)
        {            
            out.at<Vec3b>(j, i) = img.at<Vec3b>(j, i);
        }
    }

    imwrite(argv[argc-1], out);
    return 0;
}