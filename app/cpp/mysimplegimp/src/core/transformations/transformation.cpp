#include "transformation.h"

Transformation::Transformation(PNM* image) :
    QThread(0)
{
    parameters = new QHash<QString, QVariant>;
    this->image = image;
    this->supervisor = 0;
}

Transformation::Transformation(PNM* image, ImageViewer* iv) :
    QThread(0)
{
    parameters = new QHash<QString, QVariant>;
    this->image = image;
    this->supervisor = iv;

    if (iv)
    {
        connect(this, SIGNAL(started()), iv, SLOT(transformationStarted()));
        connect(this, SIGNAL(finished()), iv, SLOT(transformationFinished()));
        connect(this, SIGNAL(message(QString)), iv, SLOT(toolMessage(QString)));
        connect(this, SIGNAL(progress(int)), iv, SLOT(toolProgress(int)));
        connect(this, SIGNAL(resultReady(PNM*)), iv, SLOT(updateImage(PNM*)));
        connect(this, SIGNAL(finished()), iv, SLOT(toolFinished()));
    }
}

Transformation::~Transformation()
{
    delete parameters;
    if (supervisor)
    {
        disconnect(this, SIGNAL(started()));
        disconnect(this, SIGNAL(message(QString)));
        disconnect(this, SIGNAL(progress(int)));
        disconnect(this, SIGNAL(resultReady(PNM*)));
        disconnect(this, SIGNAL(finished()));
    }
}

/** Sets a parameter param of the transformation
* with the value.
* If the parameter was already set it is overridden
* Returns the pointer to the Transformation object so it can be used as a factory.
*/
Transformation* Transformation::setParameter(QString param, QVariant value)
{
    parameters->insert(param, value);
    return this;
}

/** Returns the value of the given param.
* If the param isn't set returns a default QVariant()
* which is an invalid variant
* @see QVariant::QVariant()
*/
QVariant Transformation::getParameter(QString param)
{
    return parameters->value(param, QVariant());
}

void Transformation::run()
{
    PNM* image = this->transform();
    emit resultReady(image);
}

/**
* /virtual, abstract!/
* Virtual method that do some transformations (based on parameter Hash)
* in the Transformation class it returns a pointer to
* the loaded image or a new null PNM image
*/
PNM* Transformation::transform()
{
    if (image)
        return image;
    else
        return new PNM();
}

/** Returns a pointer to the image stored in the class */
PNM* Transformation::getImage()
{
    return image;
}

/** Returns a pixel value at given coordinates using different modes */
QRgb Transformation::getPixel(int x, int y, Mode mode)
{
    switch (mode) {
    case CyclicEdge:    return getPixelCyclic(x,y);
    case NullEdge:      return getPixelNull(x,y);
    case RepeatEdge:    return getPixelRepeat(x,y);
    default:            return image->pixel(x,y);
    }

}

/** Returns a pixel using the Cyclic mode:
 *  pixel(x,y) = pixel(x%width, y%width);
 */
QRgb Transformation::getPixelCyclic(int x, int y)
{
    int width = image->width();
    int height = image->height();

    // Return a pixel by looping the image
    x = x % width;
    y = y % height;

    // Return pixel
    return image->pixel(x,y);
}

/**
  * Returns a given pixel
  * If the pixel is out of image boundaries Black is returned;
  */
QRgb Transformation::getPixelNull(int x, int y)
{
    int width  = image->width();
    int height = image->height();

    // Return black as a pixel when out of range
    if (x > width || x < 0 || y > height || y < 0){
        return Qt::black;
    }

    // Return pixel
    return image->pixel(x,y);
}

/**
  * Returns given pixel.
  * If the pixel is out of image boundaries
  * the nearest edge pixel is given
  */
QRgb Transformation::getPixelRepeat(int x, int y)
{
    int width  = image->width();
    int height = image->height();

    // Copy nearest edge pixel
    if (x<0 && y<0) return image->pixel(0,0);
    else if (x>=width && y<0) return image->pixel(width-1,0);
    else if (x>=width && y>=height) return image->pixel(width-1,height-1);
    else if (x<0 && y>=height) return image->pixel(0,height-1);
    else if (x<0) return image->pixel(0,y);
    else if (y<0) return image->pixel(x,0);
    else if (x>=width) return image->pixel(width-1,y);
    else if (y>=height) return image->pixel(x,height-1);
    else return image->pixel(x,y);

    return image->pixel(x,y);
}

/** Returns a size x size part of the image centered around (x,y) */
math::matrix<float> Transformation::getWindow(int x, int y, int size, Channel channel, Mode mode = RepeatEdge)
{
    math::matrix<float> window(size,size);

    // Calculate center
    int center = size/2;

    // Get window
    for (int i=0; i < size; i++)
    {
        for (int j=0; j < size; j++)
        {
            QRgb pixelVal = getPixel(x-center+i, y-center+j, mode);
            if (channel == RChannel)
            {
                window[i][j] = qRed(pixelVal);
            }
            else if (channel == GChannel)
            {
                window[i][j] = qGreen(pixelVal);
            }
            else if (channel == BChannel)
            {
                window[i][j] = qBlue(pixelVal);
            }
            else if (channel == LChannel)
            {
                window[i][j] = qGray(pixelVal);
            }
        }
    }
    return window;
}

ImageViewer* Transformation::getSupervisor()
{
    return supervisor;
}
