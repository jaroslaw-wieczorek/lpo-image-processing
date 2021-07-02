#include "bin_manual.h"

BinarizationManual::BinarizationManual(PNM* img) :
    Transformation(img)
{
}

BinarizationManual::BinarizationManual(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationManual::transform()
{
    int threshold = getParameter("threshold").toInt();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    // Iterate over pixels
    for (int x=0; x < width; x++)
    {
            for (int y=0; y < height; y++)
            {
                // Get current pixel
                QRgb pixel = image->pixel(x, y);
                if(qGray(pixel) > threshold)
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




