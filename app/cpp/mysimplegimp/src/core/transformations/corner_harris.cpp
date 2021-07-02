#include "corner_harris.h"

#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"
#include <iostream>

CornerHarris::CornerHarris(PNM* img) :
    Convolution(img)
{
}

CornerHarris::CornerHarris(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* CornerHarris::transform()
{
    int    threshold    = getParameter("threshold").toInt();
    double sigma        = getParameter("sigma").toDouble();
    double sigma_weight = getParameter("sigma_weight").toDouble();
    double k_param      = getParameter("k").toDouble();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    math::matrix<float> Ixx(width, height);
    math::matrix<float> Iyy(width, height);
    math::matrix<float> Ixy(width, height);

    this->corner_candidates = new math::matrix<float>(width, height);
    this->corner_nonmax_suppress = new math::matrix<float>(width, height);

    ConversionGrayscale* conversion_grayscale = new ConversionGrayscale(image);
    PNM* gray_image = conversion_grayscale->transform();

    BlurGaussian* blur_gaussian = new BlurGaussian(gray_image);
    blur_gaussian->setParameter("size", 3);
    blur_gaussian->setParameter("sigma", 1.6);
    PNM* blur_gauss_image = blur_gaussian->transform();

    EdgeSobel* edge_sobel = new EdgeSobel(blur_gauss_image);
    math::matrix<float>* Gx = edge_sobel->rawHorizontalDetection();
    math::matrix<float>* Gy = edge_sobel->rawVerticalDetection();

    for (int i = 0;  i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            Ixx[i][j] = (*Gx)[i][j] * (*Gx)[i][j];
            Ixy[i][j] = (*Gx)[i][j] * (*Gy)[i][j];
            Iyy[i][j] = (*Gy)[i][j] * (*Gy)[i][j];

            (*corner_candidates)[i][j] = 0;
            (*corner_nonmax_suppress)[i][j] = 0;
        }
    }

    for (int i = 1; i < width - 1; i++)
    {
        for (int j = 1; j < height - 1; j++)
        {
            float Sxx = 0;
            float Syy = 0;
            float Sxy = 0;

            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    Sxx = Sxx + Ixx[i + k][j + l] * BlurGaussian::getGauss(k, l, sigma);
                    Syy = Syy + Iyy[i + k][j + l] * BlurGaussian::getGauss(k, l, sigma);
                    Sxy = Sxy + Ixy[i + k][j + l] * BlurGaussian::getGauss(k, l, sigma);
                }
            }

            Sxx = Sxx / sigma_weight;
            Sxy = Sxy / sigma_weight;
            Syy = Syy / sigma_weight;

            math::matrix<float> H(2,2);

            H(0,0) = Sxx;
            H(0,1) = Sxy;
            H(1,0) = Sxy;
            H(1,1) = Syy;

            float detH = H(0, 0) * H(1, 1) - H(0, 1) * H(1, 0); //determinant
            float trH = H(0, 0) + H(1, 1); //trace
            float r = detH - k_param * pow(trH, 2);

            if (r > threshold)
            {
                (*corner_candidates)[i][j] = r;
            }
        }
    }

    bool search = true;

    while(search)
    {
        search = false;
        for (int i = 1;  i < width - 1; i++)
        {
            for (int j = 1;  j <height - 1; j++)
            {
                float max = (*corner_candidates)[i][j];

                for (int k = -1; k <= 1; k++)
                {
                    for (int l = -1; l <= 1; l++)
                    {
                        if (max < (*corner_candidates)[i+k][j+l])
                        {
                            max = (*corner_candidates)[i+k][j+l];
                        }
                    }
                }

                if ((*corner_candidates)[i][j] == max)
                {
                    (*corner_nonmax_suppress)[i][j] = (*corner_candidates)[i][j];
                }
                else
                {
                    if ((*corner_candidates)[i][j] > 0)
                    {
                        search = true;
                    }

                    (*corner_nonmax_suppress)[i][j] = 0;
                }

            }
        }
        corner_candidates = corner_nonmax_suppress;
    }

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if ((*corner_candidates)[i][j] == 0)
            {

                newImage->setPixel(i, j, 0);
            }
            else
            {
                // std::cout << "White corner" << std::endl;
                newImage->setPixel(i, j, 1);
            }
        }
    }
    return newImage;
}


