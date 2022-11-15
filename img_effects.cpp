#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

enum Effect : int
{
    Invert = 1,
    MirrorHorizontal = 2,
    MirrorVertical = 3,
    Brightness = 4,
    Rotate = 5
};

using namespace cv;
int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cerr << "Invalid usage. Usage: ./img_effects original copy\n";
        return 1;
    }

    std::string image_path = samples::findFile(argv[argc-2]);

    Mat img = imread(image_path, IMREAD_ANYCOLOR);
    if(img.empty())
    {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }

    int eff, bright, n;

    std::cout << "\nInput value for effect wanted:\n-> 1 for Invert\n-> 2 for Mirror Horizontally\n-> 3 for Mirror Vertically\n"
            << "-> 4 for Increse, Decrease brightness\n-> 5 for Rotations of 90 degrees\n\n"
            << "-> ";
    std::cin >> eff;

    Mat out(img.rows, img.cols, CV_8UC3);

    switch (eff){

        case Invert:   
            for(int i = 0; i < img.cols; i++)
            {
                for(int j = 0; j < img.rows; j++)
                {            
                    Vec3b p = img.at<Vec3b>(j, i);
                    uchar red = 255 - p[0];
                    uchar green = 255 - p[1];
                    uchar blue = 255 - p[2];

                    out.at<Vec3b>(j, i) = Vec3b(red, green, blue);
                }
            }
            break;

        case MirrorHorizontal: 
            for(int i = 0; i < img.cols; i++)
            {
                for(int j = 0; j < img.rows; j++)
                {            
                    out.at<Vec3b>(j, i) = img.at<Vec3b>(j, img.cols - i - 1);
                }
            }
            break;

        case MirrorVertical:
            for(int i = 0; i < img.cols; i++)
            {
                for(int j = 0; j < img.rows; j++)
                {            
                    out.at<Vec3b>(j, i) = img.at<Vec3b>(img.rows - j - 1, i);
                }
            }
            break;

        case Brightness:

            std::cout << "Value for increase/decrease: ";
            std::cin >> bright;

            for(int i = 0; i < img.cols; i++)
            {
                for(int j = 0; j < img.rows; j++)
                {   
                    for( int c = 0; c < img.channels(); c++ ) {
                    out.at<Vec3b>(j,i)[c] = saturate_cast<uchar>(img.at<Vec3b>(j, i)[c] + bright);

                    }         
                }
            }

            break;

        case Rotate:
            {
                std::cout << "Times to rotate: ";
                std::cin >> n;

                for(int r = 0; r < n; r++)
                {
                    out = Mat(img.cols, img.rows, CV_8UC3);

                    for(int j = 0; j < img.rows; j++)
                    {
                        for(int i = 0; i < img.cols; i++)
                        {            
                            out.at<Vec3b>(i, img.rows - j - 1) = img.at<Vec3b>(j, i);                            
                        }
                    }
                    
                    out.copyTo(img);
                }
            }
            break;

        default:
            std::cout << "Número inválido" << std::endl;
            break;
    }

    imwrite(argv[argc-1], out);
    return 0;
}