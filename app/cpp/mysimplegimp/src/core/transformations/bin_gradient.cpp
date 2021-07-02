#include "bin_gradient.h"
using namespace std;

BinarizationGradient::BinarizationGradient(PNM* img) :
    Transformation(img)
{
}

BinarizationGradient::BinarizationGradient(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationGradient::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);
    int numerator = 0;
    int denominator = 0;

    // Gx
    int Gx = 0;

    // Gy
    int Gy = 0;

    int G = 0;
    int T = 0;

    //PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Mode mode = Transformation::RepeatEdge;
            int I  = qGray(getPixel(x, y, mode));

            // Get Gx and Gy
            Gx = qGray(getPixel(x + 1, y, mode)) - qGray(getPixel(x - 1, y, mode));
            Gy = qGray(getPixel(x, y + 1, mode)) - qGray(getPixel(x, y - 1, mode));

            if (Gx > Gy)
            {
                G = Gx;
            }
            else
            {
                G = Gy;
            }
            numerator +=  I * G;
            denominator += G;
        }
    }

    T = numerator / denominator;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Mode mode = Transformation::RepeatEdge;
            QRgb pixel = getPixel(x, y, mode);

            int val = qGray(pixel);

            if (val < T)
            {
                newImage->setPixel(x, y, 0);
            }
            else
            {
                newImage->setPixel(x, y, 1);
            }
        }
    }

    return newImage;
}


