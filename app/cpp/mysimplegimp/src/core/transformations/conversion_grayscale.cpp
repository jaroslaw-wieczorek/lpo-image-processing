#include "conversion_grayscale.h"
#include <QDebug>
#include <iostream>
ConversionGrayscale::ConversionGrayscale(PNM* img) :
    Transformation(img)
{
}

ConversionGrayscale::ConversionGrayscale(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* ConversionGrayscale::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Grayscale8);

    if (image->format() == QImage::Format_Mono)
    {
        for (int x=0; x<width; x++){
            for (int y=0; y<height; y++)
            {
                QColor color = QColor::fromRgb(image->pixel(x,y)); // Getting the pixel(x,y) value
                newImage->setPixel(x,y, color == Qt::white ? PIXEL_VAL_MAX : PIXEL_VAL_MIN);
            }
        }

    }
    else // if (image->format() == QImage::Format_RGB32)
    {
        for (int x=0; x<width; x++)
        {
            for (int y=0; y<height; y++)
            {
                QRgb pixel = image->pixel(x,y); // Getting the pixel(x,y) value
                //int r = qRed(pixel);    // Get the 0-255 value of the R channel
                //int g = qGreen(pixel);  // Get the 0-255 value of the G channel
                //int b = qBlue(pixel);
                double r = qRed(pixel)*0.3;
                double g = qGreen(pixel)*0.6;
                double b = qBlue(pixel)*0.1;
                QColor newPixel = QColor(r+g+b,r+g+b,r+g+b);
                newImage->setPixel(x,y, newPixel.rgb());

            }
        }
    }

    return newImage;
}
