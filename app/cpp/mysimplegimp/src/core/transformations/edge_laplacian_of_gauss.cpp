#include "edge_laplacian_of_gauss.h"

#include "blur_gaussian.h"

EdgeLaplaceOfGauss::EdgeLaplaceOfGauss(PNM* img) :
    Convolution(img)
{
}

EdgeLaplaceOfGauss::EdgeLaplaceOfGauss(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

math::matrix<float> EdgeLaplaceOfGauss::getMask()
{
    size = getParameter("size").toInt();
    double sigma = getParameter("sigma").toDouble();

    math::matrix<float> mask(size, size);

    int center = -size / 2;


    for (int x = 0, k = center; x < size; x++, k++)
    {
        for (int y = 0, l = center; y < size; y++, l++)
        {
            mask(x, y) = getLoG(k, l, sigma);
        }
    }

    return mask;
}

float EdgeLaplaceOfGauss::getLoG(int x, int y, float s)
{
    int px = pow(x, 2);
    int py = pow(y, 2);

    float ps = pow(s, 2);
    int numerator = px + py - 2;

    int gauss = BlurGaussian::getGauss(x, y, ps);

    int result = (numerator / ps) * gauss;

    return result;
}


int EdgeLaplaceOfGauss::getSize()
{
    return size;
}
