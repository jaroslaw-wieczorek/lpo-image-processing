#include "map_height.h"
#include "conversion_grayscale.h"

MapHeight::MapHeight(PNM* img) :
    Transformation(img)
{
}

MapHeight::MapHeight(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* MapHeight::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());
    ConversionGrayscale* grayscale = new ConversionGrayscale(image);

    newImage = grayscale->transform();
    return newImage;
}
