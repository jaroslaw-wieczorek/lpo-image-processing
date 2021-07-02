#include "blur_gaussian.h"

BlurGaussian::BlurGaussian(PNM* img) :
    Convolution(img)
{
}

BlurGaussian::BlurGaussian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* BlurGaussian::transform()
{
    emit message("Blurring...");

    int size = getParameter("size").toInt();
    radius = (size/2)+1;
    sigma = getParameter("sigma").toDouble();

    return convolute(getMask(size, Normalize), RepeatEdge);
}

math::matrix<float> BlurGaussian::getMask(int size, Mode)
{
    math::matrix<float> mask(size, size);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            mask(i,j) = getGauss(i-radius, j-radius, sigma);
        }
    }


    return mask;
}

float BlurGaussian::getGauss(int x, int y, float sigma)
{    
    // Calculate a two-dimensional Gaussian function:
    return (1 / (2 * M_PI * pow(sigma, 2))) * exp(-(pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)));

}

