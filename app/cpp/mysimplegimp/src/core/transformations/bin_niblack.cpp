#include "bin_niblack.h"
#include "transformation.h"

BinarizationNiblack::BinarizationNiblack(PNM* img) :
    Transformation(img)
{
}

BinarizationNiblack::BinarizationNiblack(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationNiblack::transform()
{
    int width = image->width();
    int height = image->height();

    int    r = getParameter("r").toInt();
    double a = getParameter("a").toDouble();

    double arithmetic_mean = 0;
    double variance = 0;
    double standard_deviation = 0;
    math::matrix<float> window;
    int T;

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    for (int x=0; x < width; x++)
    {
        for (int y=0; y < height; y++)
        {
            // Get current pixel
            QRgb pixel = image->pixel(x, y);

            //Get current window
            window = getWindow(x, y, r, LChannel, RepeatEdge);

            // Calculate arithmetic mean
            arithmetic_mean = window.sum() / pow(r, 2);


            // Calculate standard_deviation
            for (int i = 0; i < r+1; i++){
                for (int j = 0; i < r+1; i++){
                    variance += pow(window[i][j] - arithmetic_mean, 2);
                }
            }
            variance /= pow(r, 2);
            standard_deviation = sqrt(variance);

            // Calculate threshold T
            T = arithmetic_mean + a * standard_deviation;

            if(qGray(pixel) > T)
            {
                newImage->setPixel(x, y, Qt::color1);
            }
            else
            {
                newImage->setPixel(x, y, Qt::color0);
            }

        }
    }
    return newImage;
}

