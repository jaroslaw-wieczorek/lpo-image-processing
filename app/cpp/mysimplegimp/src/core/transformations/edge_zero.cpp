#include "edge_zero.h"

#include "edge_laplacian_of_gauss.h"

EdgeZeroCrossing::EdgeZeroCrossing(PNM* img) :
    Convolution(img)
{
}

EdgeZeroCrossing::EdgeZeroCrossing(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* EdgeZeroCrossing::transform()
{
    int    size  = getParameter("size").toInt();
    double sigma = getParameter("sigma").toDouble();
    int    t     = getParameter("threshold").toInt();

    int width = image->width();
    int height = image->height();

    int t_width;
    int t_height;

    PNM* newImage = new PNM(width, height, QImage::Format_Grayscale8);

    EdgeLaplaceOfGauss *laplace_gauss = new EdgeLaplaceOfGauss(image);

    laplace_gauss->setParameter("size", size);
    laplace_gauss->setParameter("sigma", sigma);

    PNM* transformed_image = laplace_gauss->transform();

    t_width = transformed_image->width();
    t_height = transformed_image->height();

    for (int i = 0; i < t_width; i++)
    {
        for (int j = 0; j < t_height; j++)
        {
            math::matrix<float> window = laplace_gauss->getWindow(i, j, size, LChannel, RepeatEdge);

            if (window.min() < (128 - t) && window.max() > (128 + t))
            {
                QRgb pixel = transformed_image->pixel(i, j);
                newImage->setPixel(i, j, QColor(pixel).rgb());
            }
            else
            {
                newImage->setPixel(i, j, QColor(0,0,0).rgb());
            }
        }
    }

    return newImage;
}

