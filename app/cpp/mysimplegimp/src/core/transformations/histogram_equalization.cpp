#include "histogram_equalization.h"

#include "../histogram.h"

HistogramEqualization::HistogramEqualization(PNM* img) :
    Transformation(img)
{
}

HistogramEqualization::HistogramEqualization(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramEqualization::transform()
{
    float width = image->width();
    float height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    // Create histogram
    Histogram* histogram = image->getHistogram();

    // Create pointers QHash<int, int>
    QHash<int, int>* R = histogram->get(Histogram::RChannel);
    QHash<int, int>* G = histogram->get(Histogram::GChannel);
    QHash<int, int>* B = histogram->get(Histogram::BChannel);
    QHash<int, int>* L = histogram->get(Histogram::LChannel);

    // Setup Channel p
    float Rp[255];
    float Gp[255];
    float Bp[255];
    float Lp[255];

    // Setup Channel D
    float RD[255];
    float GD[255];
    float BD[255];
    float LD[255];

    // Get size
    float size = width * height;

    // Create template values
    float tempSumR=0;
    float tempSumG=0;
    float tempSumB=0;
    float tempSumL=0;


    for (int i=0; i<256; i++)
    {
        // Set Xp[i] values
        Rp[i] = (R->value(i))/(size);
        Gp[i] = (G->value(i))/(size);
        Bp[i] = (B->value(i))/(size);
        Lp[i] = (L->value(i))/(size);
    }


    for(int i=0; i<255; i++)
    {
        // Red
        tempSumR=Rp[i]+tempSumR;
        RD[i]=tempSumR;

        // Green
        tempSumG=Gp[i]+tempSumG;
        GD[i]=tempSumG;

        // Blue
        tempSumB=Bp[i]+tempSumB;
        BD[i]=tempSumB;

        // L
        tempSumL=Lp[i]+tempSumL;
        LD[i]=tempSumL;
    }


    // For each pixel
    for (int x=0; x<width; x++)
    {
        for (int y=0; y<height; y++)
        {
            // Get pixel
            QRgb pixel = image->pixel(x, y);

            // Set new values
            int r = RD[qRed(pixel)] * 255;
            int g = GD[qGreen(pixel)] * 255;
            int b = BD[qBlue(pixel)] * 255;
            int l = LD[qGray(pixel)] * 255;

            if (image->format() == QImage::Format_RGB32)
            {
                newImage->setPixel(x, y, QColor(r,g,b).rgb());
            }
            else
            {
                newImage->setPixel(x, y, l);
            }
        }
    }
    return newImage;
}
