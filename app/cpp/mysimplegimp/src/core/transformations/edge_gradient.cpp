#include "edge_gradient.h"

EdgeGradient::EdgeGradient(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

EdgeGradient::EdgeGradient(PNM* img) :
    Convolution(img)
{
}

PNM* EdgeGradient::verticalDetection()
{
    return convolute(g_y, RepeatEdge);
}

PNM* EdgeGradient::horizontalDetection()
{
    return convolute(g_x, RepeatEdge);
}


int EdgeGradient::calcValue(int i, int j, Channel channel, PNM* img_x, PNM* img_y)
{
    int value_x, value_y;
    double power_x, power_y;
    int value_xy;
    int result;

    QRgb pixel_x = img_x->pixel(i, j);
    QRgb pixel_y = img_y->pixel(i, j);

    switch(channel)
    {

        case RChannel:
            value_x = qRed(pixel_x);
            value_y = qRed(pixel_y);
        break;
        case GChannel:
            value_x = qGreen(pixel_x);
            value_y = qGreen(pixel_y);
        break;
        case BChannel:
            value_x = qBlue(pixel_x);
            value_y = qBlue(pixel_y);
        break;
        case LChannel:
            value_x = qGray(pixel_x);
            value_y = qGray(pixel_y);
        break;
    }

    power_x = pow(value_x, 2);
    power_y = pow(value_y, 2);

    value_xy = (int) sqrt(power_x + power_y);

    result = std::max(0, std::min(255, value_xy));

    return result;

}


PNM* EdgeGradient::transform()
{
    PNM* newImage = new PNM(image->width(), image->height(), image->format());

    int width = image->width();
    int height = image->height();

    // Gradient image by x
    PNM* img_x = horizontalDetection();

    // Gradient image by y
    PNM* img_y = verticalDetection();


    for (int i = 0; i < width; i++)
    {
       for (int j = 0; j < height; j++)
       {
           if (image->format() == QImage::Format_Indexed8)
           {
               // Calculate gray pixel value
               int l = calcValue(i, j, LChannel, img_x, img_y);
               newImage->setPixel(i, j, l);
           }
           else
           {
               // Calculate RGB values for pixel
               int r = calcValue(i, j, RChannel, img_x, img_y);
               int g = calcValue(i, j, GChannel, img_x, img_y);
               int b = calcValue(i, j, BChannel, img_x, img_y);

               QColor color = QColor(r, g, b);
               newImage->setPixel(i, j, color.rgb());
           }
       }
    }
    return newImage;
}

