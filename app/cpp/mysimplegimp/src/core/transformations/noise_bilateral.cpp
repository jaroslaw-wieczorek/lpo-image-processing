#include "noise_bilateral.h"

NoiseBilateral::NoiseBilateral(PNM* img) :
    Convolution(img)
{
}

NoiseBilateral::NoiseBilateral(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseBilateral::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    sigma_d = getParameter("sigma_d").toInt();
    sigma_r = getParameter("sigma_r").toInt();

    radius = sigma_d;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (image->format() == QImage::Format_Indexed8)
            {
                // Get calculated value for LChannel and set as new pixel
                newImage->setPixel(x, y, calcVal(x, y, LChannel));
            }
            else
            {
               // Get calculated values for RGB channels
               int r_calc = calcVal(x, y, RChannel);
               int g_calc = calcVal(x, y, GChannel);
               int b_calc = calcVal(x, y, BChannel);


               QColor color = QColor(r_calc, g_calc, b_calc);
               newImage->setPixel(x, y, color.rgb());
            }
        }
    }

    return newImage;
}

int NoiseBilateral::calcVal(int x, int y, Channel channel)
{
    // Set variables
    float top = 0;
    float bottom = 0;

    // Get window
    math::matrix<float> window = getWindow(x,y, radius, channel, RepeatEdge);

    // Get size of matrix
    int window_row_number = window.rowno();
    int window_col_number = window.colno();

    // Get central value
    float central = window[window_row_number / 2][window_col_number / 2];

    for (int i = 0; i < window_col_number; i++)
    {
        for (int j = 0; j < window_row_number; j++)
        {
            // Get Point in (i, j)
            QPoint p1(i,j);

            // Get second Point
            QPoint p2(window_row_number / 2, window_col_number / 2);

            // Calculate top value
            top = top + window[i][j] * colorCloseness(window[i][j], central) * spatialCloseness(p1, p2);

            // Calculate bottom value
            bottom = bottom + colorCloseness(window[i][j], central) * spatialCloseness(p1, p2);
        }
    }

   return top / bottom;
}

float NoiseBilateral::colorCloseness(int val1, int val2)
{
    float result = exp(-(pow(val1 - val2, 2) / (2 * sigma_r * sigma_r)));
    return result;
}

float NoiseBilateral::spatialCloseness(QPoint point1, QPoint point2)
{
    float result = exp(-(pow(point1.x() - point2.x(), 2) + pow(point1.y() - point2.y(), 2) / (2 * sigma_d * sigma_d)));
    return result;
}
