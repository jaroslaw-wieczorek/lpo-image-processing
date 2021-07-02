#include "convolution.h"

/** Overloaded constructor */
Convolution::Convolution(PNM* img) :
    Transformation(img)
{
}

Convolution::Convolution(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

/** Returns a convoluted form of the image */
PNM* Convolution::transform()
{
    return convolute(getMask(3, Normalize), RepeatEdge);
}

/** Returns a sizeXsize matrix with the center point equal 1.0 */
math::matrix<float> Convolution::getMask(int size, Mode mode = Normalize)
{
    math::matrix<float> mask(size, size);

    // Get center of image
    int center = size/2;

    // Get mask
    for (int i=0; i < size; i++)
    {
        for (int j=0; j < size; j++)
        {
            if (i==j && i == center && j == center)
            {
                mask[i][j] = 1;
            }
            else
            {
                mask[i][j] = 0;
            }
        }
    }
    return mask;
}

/** Does the convolution process for all pixels using the given mask. */
PNM* Convolution::convolute(math::matrix<float> mask, Mode mode = RepeatEdge)
{
    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

    float weight = Convolution::sum(mask);
    if (weight == 0)
    {
        weight = 1.0;
    }

    math::matrix<float> reflection = Convolution::reflection(mask);
    for (int x=0; x < width ; x++)
    {
        for (int y=0; y < height; y++)
        {
             math::matrix<float> rAcc = Convolution::join(getWindow(x, y, mask.rowno(), Transformation::RChannel, mode), reflection);
             math::matrix<float> gAcc = Convolution::join(getWindow(x, y, mask.rowno(), Transformation::GChannel, mode), reflection);
             math::matrix<float> bAcc = Convolution::join(getWindow(x, y, mask.rowno(), Transformation::BChannel, mode), reflection);

             float rAccSum = Convolution::sum(rAcc);
             float gAccSum = Convolution::sum(gAcc);
             float bAccSum = Convolution::sum(bAcc);


             if (weight != 0)
             {
                 rAccSum = rAccSum / weight;
                 gAccSum = gAccSum / weight;
                 bAccSum = bAccSum / weight;
             }

             // Calculate Red Accumulate Sum
             if (rAccSum < 0)
             {
                 rAccSum = 0;
             }
             else if (rAccSum > 255)
             {
                 rAccSum = 255;
             }

             // Calculate Green Accumulate Sum
             if (gAccSum < 0)
             {
                 gAccSum = 0;
             }
             else if (gAccSum > 255)
             {
                 gAccSum = 255;
             }

             // Calculate Blue Accumulate Sum
             if (bAccSum < 0)
             {
                 bAccSum = 0;
             }
             else if(bAccSum > 255)
             {
                 bAccSum = 255;
             }

             // Create pixel
             QColor newPixel = QColor(rAccSum, gAccSum, bAccSum);

             // Set pixel
             newImage->setPixel(x,y, newPixel.rgb());
        }
    }
    return newImage;
}

/** Joins to matrices by multiplying the A[i,j] with B[i,j].
  * Warning! Both Matrices must be squares with the same size!
  */
const math::matrix<float> Convolution::join(math::matrix<float> A, math::matrix<float> B)
{
    int size = A.rowno();
    math::matrix<float> C(size, size);


    for (int i=0; i < size; i++)
    {
        for (int j=0; j < size; j++)
        {
           // Multiplication
           C[i][j] = A[i][j] * B[i][j];
        }
    }

    return C;
}

/** Sums all of the matrixes elements */
const float Convolution::sum(const math::matrix<float> A)
{
    float sum = 0.0;

    int size = A.rowno();

    for (int i=0; i<size; i++)
    {
        for (int j=0; j<size; j++)
        {
            // Summation
            sum = sum + A[i][j];
        }
    }
    return sum;
}


/** Returns reflected version of a matrix */
const math::matrix<float> Convolution::reflection(const math::matrix<float> A)
{
    int size = A.rowno();
    math::matrix<float> C(size, size);


    for (int i=0; i < size; i++)
    {
        for (int j=0; j < size; j++)
        {
            C[i][j] = A[size-i-1][size-j-1];
        }
    }

    return C;
}
