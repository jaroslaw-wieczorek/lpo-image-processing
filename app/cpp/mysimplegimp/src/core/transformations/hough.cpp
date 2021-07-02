#include "hough.h"
#include "iostream"
#include "conversion_grayscale.h"
#include "edge_laplacian.h"

Hough::Hough(PNM* img) :
    Transformation(img)
{
}

Hough::Hough(PNM* img, ImageViewer* super) :
    Transformation(img, super)
{
}

PNM* Hough::transform()
{   
    int width = image->width();
    int height = image->height();

    int theta_density = getParameter("theta_density").toInt();
    bool skip_edge = getParameter("skip_edge_detection").toBool();

    std::cout << "skip_edge: " << skip_edge << std::endl;
    int theta_size = 180 * theta_density;
    double max_ro = sqrt(pow(width, 2) + pow(height, 2));

    ConversionGrayscale* gray_scale = new ConversionGrayscale(image);
    image = gray_scale->transform();

    PNM* newImage = new PNM(theta_size, max_ro * 2 + 1, QImage::Format_Grayscale8);

    int new_image_width = newImage->width();
    int new_image_height = newImage->height();

    math::matrix<float> hough(new_image_width, new_image_height);



    if (skip_edge == false)
    {
         EdgeLaplacian *el = new EdgeLaplacian(image);
         el->setParameter("size", 7);
         image = el->transform();
    }

    for (int x = 0; x < new_image_width; x++)
    {
        for (int y = 0; y < new_image_height; y++)
        {
            hough[x][y] = 0;
        }
    }


    double theta = 0.0;
    double ro = 0;
    float max = 0;
    double max_z = 0;

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (qGray(image->pixel(i, j)) > 0)
            {
                for (int k = 0; k < theta_size; k++)
                {
                    theta = (k * M_PI) / (theta_density * 180);
                    ro = i * cos(theta) + j * sin(theta);
                    hough[k][ro + max_ro]++;
                }
            }
        }
    }

    max = hough.max();

    int z;

    for (int x = 0; x < new_image_width; x++)
    {
        for (int y = 0; y < new_image_height; y++)
        {
            z = hough[x][y] * (max / 255);

            newImage->setPixel(x, y, QColor(z, z, z).rgb());
        }
    }
    std::cout << "max_z: " << max_z << std::endl;

    return newImage;
}
