#include "bin_otsu.h"

#include "histogram_equalization.h"
#include "../histogram.h"

BinarizationOtsu::BinarizationOtsu(PNM* img) :
    Transformation(img)
{
}

BinarizationOtsu::BinarizationOtsu(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationOtsu::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);
    HistogramEqualization* equalized_histogram = new HistogramEqualization(image);
    QHash<int, int> * hg;

    // Number od pixels in image
    int N = width * height;

    image = equalized_histogram->transform();

    // Histogram table
    hg = image->getHistogram()->get(Histogram::LChannel);


    int value;
    double bcv[256];

    int sum_1 = 0;
    int sum2 = 0;

    double u_b;
    double u_f;
    double W_b;
    double W_f;

    for (int t=0; t < 255; t++)
    {

        for (int j=0; j < t; j++)
        {
            value = image->getHistogram()->get(Histogram::LChannel)->value(j);
            sum_1 += value;
        }

        W_b = sum_1 / pow(255, 2);

        for (int j=0; j < t; j++)
        {
            value = image->getHistogram()->get(Histogram::LChannel)->value(j);
            sum2 += value * j;
		}

        if (sum_1 == 0)
        {
            u_b = 0;
        }
        else
        {
            u_b = sum2 / sum_1;
        }

        sum_1 = 0;
        sum2 = 0;

        for (int j=t; j < 255; j++)
        {
            value = image->getHistogram()->get(Histogram::LChannel)->value(j);
            sum_1 += value;
        }

        W_f = sum_1 / pow(255, 2);

        for (int j=t; j < 255; j++)
        {
            value = image->getHistogram()->get(Histogram::LChannel)->value(j);
            sum2 += value * j;
        }

        if (sum_1 == 0)
        {
            u_f = 0;
        }
		else
		{
			u_f = sum2 / sum_1;
		}

		bcv[t]= W_b * W_f * pow(u_b - u_f, 2);
	}

    T = 0;
    for (int j=0; j< 255;j++)
    {
        if (bcv[j] > bcv[T])
        {
            T=j;
        }
    }

    // Iterate over pixels
    for (int x=0; x<width; x++)
    {
        for (int y=0; y < height; y++)
         {
            // Get current pixel
            QRgb pixel = image->pixel(x, y);
            int l = qGray(pixel);

            newImage->setPixel(x,y, l < T ? Qt::color0 : Qt::color1);
        }
    }

    return newImage;
}

