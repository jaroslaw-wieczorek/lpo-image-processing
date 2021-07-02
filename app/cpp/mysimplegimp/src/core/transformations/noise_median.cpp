#include "noise_median.h"

NoiseMedian::NoiseMedian(PNM* img) :
    Convolution(img)
{
}

NoiseMedian::NoiseMedian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseMedian::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());


    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (image->format() == QImage::Format_Indexed8)
            {
                // Get median value for LChannel and set as new pixel
                newImage->setPixel(i, j, getMedian(i, j, LChannel));
            }
            else
            {
                // Get median values for RGB channels
                int r_med = getMedian(i, j, RChannel);
                int g_med = getMedian(i, j, GChannel);
                int b_med = getMedian(i, j, BChannel);

                // Create new color
                QColor color = QColor(r_med, g_med, b_med);

                // Set new pixel with rgb color
                newImage->setPixel(i, j, color.rgb());
            }
        }
    }

    return newImage;
}

int NoiseMedian::getMedian(int x, int y, Channel channel)
{
    int radius = getParameter("radius").toInt();
    int window_size = 2 * radius + 1;

    math::matrix<int> window = getWindow(x, y, window_size, channel, RepeatEdge);
    std::vector<int> result; //list does not provide a subscript operator

    for(std::size_t i=0; i < window.rowno(); i++){
        for(std::size_t j=0; j < window.colno(); j++){
            result.push_back(window[i][j]);
        }
    }
    std::sort(result.begin(), result.end());
    int middle = result.size()/2;
    return result[middle];
}

/*
// Different solution
int NoiseMedian::getMedian(int x, int y, Channel channel)
{
    int radius = getParameter("radius").toInt();
    int window_size = 2 * radius + 1;

    int powSize = size*size;
    int window[powSize];

    int sr = 0;
    int sc = 0;
    int lp = 0;
    float value;

    for (sr = x-radius; sr <= x+radius; sr++)
    {
        for (sc = y-radius; sc <= y+radius; sc++)
        {
            QRgb color = getPixel(sr, sc, CyclicEdge);

            switch(channel)
            {
                case LChannel:
                    value = qGray (color);
                    break;
                case RChannel:
                    value = qRed  (color);
                     break;
                case GChannel:
                    value = qGreen(color);
                    break;
                case BChannel:
                    value = qBlue (color);
                    break;
            }
               window[lp++] = value;
        }
    }

    std::sort(window, window + powSize);
    int center = powSize / 2;
    int result =  window[center];
    return result;
}
*/
