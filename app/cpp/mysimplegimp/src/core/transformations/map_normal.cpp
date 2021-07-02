#include "map_normal.h"

#include "edge_sobel.h"
#include "map_height.h"
#include <iostream>

MapNormal::MapNormal(PNM* img) :
    Convolution(img)
{
}

MapNormal::MapNormal(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* MapNormal::transform()
{
    int width  = image->width();
    int height = image->height();

    // The power constant of the generated normal map.
    double strength = getParameter("strength").toDouble();

    PNM* newImage = new PNM(width, height, image->format());

    MapHeight* mh = new MapHeight(image);
    PNM* image = mh->transform();

    EdgeSobel* es = new EdgeSobel(image);
    math::matrix<float>* Gx = es->rawHorizontalDetection();
    math::matrix<float>* Gy = es->rawVerticalDetection();

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            // For each pixel (i,j), determine the coordinates of the vector d
            float dx = (*Gx)(i, j) / 255;
            float dy = (*Gy)(i, j) / 255;
            float dz = 1.0 / strength;

            // Normalise the vector d
            double dw = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

            // Scale these values to the interval
            dx = dx / dw;
            dy = dy / dw;
            dz = dz / dw;

            dx = (dx + 1.0) * (255 / strength);
            dy = (dy + 1.0) * (255 / strength);
            dz = (dz + 1.0) * (255 / strength);

            QColor newPixel = QColor(dx, dy, dz);
            newImage->setPixel(i, j, newPixel.rgb());
        }
    }

    return newImage;
}
