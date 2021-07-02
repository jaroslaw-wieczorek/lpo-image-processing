#include "hough_lines.h"
#include "bin_gradient.h"
#include "edge_laplacian.h"

#include "hough.h"
#include <iostream>
#include <Qt>
#include <QPainter>

HoughLines::HoughLines(PNM* img) :
    Transformation(img)
{
}

HoughLines::HoughLines(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}


PNM* HoughLines::transform()
{

    // Cut of value from the image;
    int  threshold      = getParameter("threshold").toInt();
    bool drawWholeLines = getParameter("draw_whole_lines").toBool();
    int theta_density = 3;

    PNM* newImage = new PNM(image->copy());

    EdgeLaplacian el(image);
    el.setParameter("size", 3);
    PNM* image_edge = el.transform();

    PNM* image_bin = BinarizationGradient(image_edge).transform();

    Hough h(image_edge);

    h.setParameter("theta_density", theta_density);
    h.setParameter("skip_edge_detection", true);
    PNM* image_hough = h.transform();

    int hough_width = image_hough->width();
    int hough_height = image_hough->height();

    int width = image_bin->width();
    int height = image_bin->height();

    qPainter = new QPainter(newImage);
    qPainter->setPen(Qt::black);

    for (int theta = 0; theta < hough_width; theta++)
    {
        for (int rho = 0; rho < hough_height; rho++)
        {
            int val = (int) qGray(image_hough->pixel(theta, rho));
            if (val > threshold)
            {
                double rtheta = ((double)theta / 3.0) * M_PI / 180.0;
                int rrho = rho - hough_height / 2;
                qPainter->drawLine(0, round(rrho / sin(rtheta)), width - 1, round((rrho - (width - 1) * cos(rtheta)) / sin(rtheta)));

            }
        }
     }

    int counter_pixels = 0;
    if (!drawWholeLines)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                QRgb pixel = image_bin->pixel(x, y);

                if (qGray(pixel) == 0)
                {
                    counter_pixels++;
                    newImage->setPixel(x, y, image->pixel(x, y));
                }
            }
        }
        std::cout << "counter " << counter_pixels << std::endl;
    }

    return newImage;
}
