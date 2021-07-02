#include "map_horizon.h"

#include "map_height.h"

MapHorizon::MapHorizon(PNM* img) :
    Transformation(img)
{
}

MapHorizon::MapHorizon(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* MapHorizon::transform()
{
    int width  = image->width(),
        height = image->height();

    double scale     = getParameter("scale").toDouble();
    int    sun_alpha = getParameter("alpha").toInt();
    int dx, dy;

    switch (getParameter("direction").toInt())
    {
    case NORTH: dx = 0; dy = -1; break;
    case SOUTH: dx = 0; dy = 1; break;
    case EAST:  dx = 1; dy = 0; break;
    case WEST:  dx = -1; dy = 0; break;
    default:
        qWarning() << "Unknown direction!";
        dx =  0;
        dy = -1;
    }

    PNM* newImage = new PNM(width, height, image->format());

    MapHeight* mapHeight = new MapHeight(image);

    image = mapHeight->transform();

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            double alpha = 0;
            int current_h = qGray(image->pixel(i, j));

            for (int k = i + dx, l = j + dy; k < width && l < height && k >= 0 && l >= 0; k = k + dx, l = l + dy)
            {
                int ray_h = qGray(image->pixel(k, l));

                if (current_h < ray_h)
                {
                    double dist = sqrt(pow(k - i, 2) + pow(l - j, 2)) * scale;
                    double ray_alpha = atan((ray_h - current_h) / dist);

                    if (ray_alpha > alpha)
                    {
                        alpha = ray_alpha;
                    }
                }

            }

            double delta = alpha - sun_alpha * M_PI / 180;

            if (delta > 0)
            {
                double val = cos(delta) * 255;
                QColor color = QColor(val, val, val);
                newImage->setPixel(i, j, color.rgb());
            }
            else
            {
                QColor color = QColor(255, 255, 255);
                newImage->setPixel(i, j, color.rgb());
            }

        }
    }
    return newImage;
}
