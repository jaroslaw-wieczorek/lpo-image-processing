#include "bin_iterbimodal.h"
#include "bin_manual.h"
#include "conversion_grayscale.h"
#include "histogram_equalization.h"
#include "../histogram.h"


BinarizationIterBimodal::BinarizationIterBimodal(PNM* img) :
    Transformation(img)
{
}

BinarizationIterBimodal::BinarizationIterBimodal(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationIterBimodal::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    // Create variables
    HistogramEqualization* equalized_histogram = new HistogramEqualization(image);
    BinarizationManual* binarization_manual;

    int iter = 1;
    
    // Threshold
    int T = 128;

    // New threshold
    int T_new;

    image = equalized_histogram->transform();

    // Histogram table
    hg = image->getHistogram()->get(Histogram::LChannel);

    // Calculate new threshold
    T_new = (avg_saturation(0, T-1) + avg_saturation(T, 255)) / 2;

    while (T_new != T)
    {
        T = T_new;

        T_new = (avg_saturation(0, T-1) + avg_saturation(T, 255)) / 2;
        iter ++;
    }

    // Use manual binarization to binaryze image
    binarization_manual = new BinarizationManual(image);
    binarization_manual->setParameter("threshold", T);

    // Return binary image
    newImage = binarization_manual->transform();

    return newImage;

}


int BinarizationIterBimodal::avg_saturation(int start, int end)
{
    int i;
    int value;
    int sum;
    int counter;
    int result;

    sum = 1;
    counter = 1;
    for (i = start; i <= end; i++)
    {
        // Get value
        value = hg->value(i);
        sum += value;
        counter += value * i;
    }
    result = counter / sum;

    return result;
}



