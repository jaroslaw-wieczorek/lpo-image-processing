#include "histogram_stretching.h"

#include "../histogram.h"

HistogramStretching::HistogramStretching(PNM* img) :
    Transformation(img)
{
}

HistogramStretching::HistogramStretching(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramStretching::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    // Create min values
    int minR = 0;
    int minG = 0;
    int minB = 0;
    int minL = 0;

    // Create max values
    int maxR = 255;
    int maxG = 255;
    int maxB = 255;
    int maxL = 255;

    // Create histogram
    Histogram* histogram = image->getHistogram();

    // Create pointers QHash<int, int>
    QHash<int, int>* R = histogram->get(histogram->RChannel);
    QHash<int, int>* G = histogram->get(histogram->GChannel);
    QHash<int, int>* B = histogram->get(histogram->BChannel);
    QHash<int, int>* L = histogram->get(histogram->LChannel);

    // Increment max values
    for (int i=0; i<256; i++)
    {
        if (R->value(minR) == 0) minR++;
        if (G->value(minG) == 0) minG++;
        if (B->value(minB) == 0) minB++;
        if (L->value(minL) == 0) minL++;
    }
    // Decrement max values
    for (int i=256; i>0; i--)
    {
        if (R->value(maxR) == 0) maxR--;
        if (G->value(maxG) == 0) maxG--;
        if (B->value(maxB) == 0) maxB--;
        if (L->value(maxL) == 0) maxL--;
    }

    // Iterate over pixels
    for (int x=0; x<width; x++)
    {
        for (int y=0; y<height; y++)
        {
            QRgb pixel = image->pixel(x, y);

            // Calculate new values
            int r = (255/(maxR-minR))*(qRed(pixel)-minR);
            int g = (255/(maxG-minG))*(qGreen(pixel)-minG);
            int b = (255/(maxB-minB))*(qBlue(pixel)-minB);
            int l = (255/(maxL-minL))*(qGray(pixel)-minL);

            if (image->format() == QImage::Format_RGB32)
            {
                // Set pixel in RGB32 model
                newImage->setPixel(x, y, QColor(r,g,b).rgb());
            }
            else
            {
                // Set pixel
                newImage->setPixel(x, y, l);
            }
        }
    }
    return newImage;
}


