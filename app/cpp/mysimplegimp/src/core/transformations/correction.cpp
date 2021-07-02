#include "correction.h"

Correction::Correction(PNM* img) :
    Transformation(img)
{
}

Correction::Correction(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* Correction::transform()
{
    float shift  = getParameter("shift").toFloat();
    float factor = getParameter("factor").toFloat();
    float gamma  = getParameter("gamma").toFloat();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    for(unsigned int i=0; i<sizeof(LUT)/sizeof(LUT[0]); i++) {

          // Adjusted Value
          float adjustedValue = 0;

          // shift value
          adjustedValue = i + shift;

          // multiply by factor
          adjustedValue = adjustedValue * factor;

          // power by gamma
          adjustedValue = pow(adjustedValue, gamma);

          // if adjusted value is bigger than 255, will be set to 255
          if (adjustedValue > PIXEL_VAL_MAX) adjustedValue = PIXEL_VAL_MAX;
          // if adjusted value is smaller than 0, will be set to 0
          if (adjustedValue < PIXEL_VAL_MIN) adjustedValue = PIXEL_VAL_MIN;

          //Fill LUT table
          LUT[i] = adjustedValue;
      }

      // Set new pixels
      for (int x = 0; x < width; x++) {
          for (int y = 0; y < height; y++) {
              QRgb pixel = image->pixel(x,y);
              int r = qRed(pixel);
              int g = qGreen(pixel);
              int b = qBlue(pixel);
              QColor newPixel = QColor(LUT[r], LUT[g], LUT[b]);
              newImage->setPixel(x, y, newPixel.rgb());
          }
      }

    return newImage;
}
