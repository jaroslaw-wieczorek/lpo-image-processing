#include "histogram.h"

#include <QDebug>
#include <QPainter>

#include <cmath>

Histogram::Histogram(QImage* image)
{
    R = new QHash<int, int>;
    G = new QHash<int, int>;
    B = new QHash<int, int>;
    L = new QHash<int, int>;
    generate(image);
}

Histogram::~Histogram()
{
    delete R;
    delete G;
    delete B;
    delete L;
}

void Histogram::generate(QImage* image)
{
    int width  = image->width();
    int height = image->height();

    for (int x = 0; x < width; x++) {

        for (int y = 0; y < height; y++) {

            QRgb pixel = image->pixel(x,y);

            // Get color values
            int r = Histogram::R->value(qRed(pixel));
            int g = Histogram::G->value(qGreen(pixel));
            int b = Histogram::B->value(qBlue(pixel));
            int l = Histogram::L->value(qGray(pixel));

            // Insert color values
            Histogram::R->insert(qRed(pixel), r+1);
            Histogram::G->insert(qGreen(pixel), g+1);
            Histogram::B->insert(qBlue(pixel), b+1);
            Histogram::L->insert(qGray(pixel), l+1);
        }
    }
}

/** Returns the maximal value of the histogram in the given channel */
int Histogram::maximumValue(Channel selectedChannel = RGB)
{
    // Create and set max value
    int max = 0;

    // Iterate over values
    for(int i = 0; i < R->size(); i++)
    {
        if(R->value(i) > max)
        {
            max = R->value(i);
        }
    }
    for(int i = 0; i < G->size(); i++)
    {
        if(G->value(i) > max)
        {
            max = G->value(i);
        }
    }
    for(int i = 0; i < B->size(); i++)
    {
        if(B->value(i) > max)
        {
            max = B->value(i);
        }
    }
    for(int i = 0; i < L->size(); i++)
    {
        if(L->value(i) > max)
        {
            max = L->value(i);
        }
    }

    return max;
}


/** Returns a pointer to the given channel QHash<int, int> */
QHash<int, int>* Histogram::get(Channel channel = LChannel)
{
    if (channel==LChannel) return L;
    if (channel==RChannel) return R;
    if (channel==GChannel) return G;
    if (channel==BChannel) return B;

    return 0;
}

/**
 *  Returns a 255 by 100 QImage containing a Histogram for the given channel.
 *  The background is transparent (Alpha 0, RGB=255) */
QImage Histogram::getImage(Channel channel = LChannel, QBrush pen = Qt::gray)
{
    // Create blank QImage and fill it with transparent background:
    QImage histImage(255, 100, QImage::Format_ARGB32);
    histImage.fill(0);
    QPainter painter(&histImage);
    painter.setBrush(Qt::transparent); 
    painter.setPen(Qt::transparent);
    painter.drawRect(0,0,255,100);

    // Calculate the aspect ratio using the maximal value of the color histograms
    int maximum = (channel == LChannel ? maximumValue(LChannel) :  maximumValue(RGB));
    float ratio = 100.0/float(maximum);

    // Preparing the painter:
    painter.setBrush(pen);
    painter.setPen(pen.color());

    int h;
    // Draw histogram
    QHash<int, int>* hist = get(channel);
    QHash<int, int>::const_iterator cit = hist->begin();

    while (cit != hist->end())
    {
        h = 100 - floor(ratio*cit.value());
        painter.drawLine(cit.key(), h, cit.key(), 100);
        ++cit;
    }

    return histImage;
}
