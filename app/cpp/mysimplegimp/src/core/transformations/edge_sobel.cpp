#include "edge_sobel.h"
#include <iostream>

EdgeSobel::EdgeSobel(PNM* img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

EdgeSobel::EdgeSobel(PNM* img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

void EdgeSobel::prepareMatrices()
{
    g_x =  math::matrix<float>(3, 3, {-1, 0, 1, -2, 0, 2, -1, 0, 1});
    g_y =  math::matrix<float>(3, 3, {-1, -2, -1, 0, 0, 0, 1, 2, 1});
}

math::matrix<float>* EdgeSobel::rawHorizontalDetection()
{
    int width = image->width();
    int height = image->height();

    math::matrix<float>* x_gradient = new math::matrix<float>(width, height);

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
           (*x_gradient)(i, j) = sum(join(g_x, getWindow(i, j, 3, LChannel, RepeatEdge)));
        }
    }
    return x_gradient;
}

math::matrix<float>* EdgeSobel::rawVerticalDetection()
{
    int width = image->width();
    int height = image->height();

    math::matrix<float>* y_gradient = new math::matrix<float>(width, height);
    math::matrix<float> window;

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
          (*y_gradient)(i, j) = sum(join(g_y, getWindow(i, j, 3, LChannel, RepeatEdge)));
        }
    }
    return y_gradient;
}
