#include "edge_canny.h"

#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

EdgeCanny::EdgeCanny(PNM* img) :
    Convolution(img)
{
}

EdgeCanny::EdgeCanny(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}


void EdgeCanny::Calculate_XY(int i, int j, enum directions dir, int* x1, int* y1, int* x2, int* y2)
{
    switch (dir)
    {

        case TOP_MIDDLE_BOTTOM_MIDDLE:
        {
            *x1 = i - 1;
            *y1 = j;
            *x2 = i + 1;
            *y2 = j;
            break;
        }

        case LEFT_MIDDLE_RIGHT_MIDDLE:
        {
            *x1 = i;
            *y1 = j - 1;
            *x2 = i;
            *y2 = j + 1;
            break;
        }

        case TOP_RIGHT_BOTTOM_LEFT:
        {
            *x1 = i - 1;
            *y1 = j - 1;
            *x2 = i + 1;
            *y2 = j + 1;
            break;
        }

        case TOP_LEFT_BOTTOM_RIGHT:
        {
            *x1 = i - 1;
            *y1 = j + 1;
            *x2 = i + 1;
            *y2 = j - 1;
            break;
        }

    }
}

PNM* EdgeCanny::transform()
{
    int width  = image->width();
    int height = image->height();


    int upper_thresh = getParameter("upper_threshold").toInt();
    int lower_thresh = getParameter("lower_threshold").toInt();

    PNM* newImage = new PNM(width, height, image->format());

    // Create grayscale
    ConversionGrayscale* grayscale = new ConversionGrayscale(image);
    PNM* gray_image = grayscale->transform();

    // Blur grayscale
    BlurGaussian* blur_gaussian = new BlurGaussian(gray_image);
    blur_gaussian->setParameter("size", 3);
    blur_gaussian->setParameter("sigma", 1.6);
    PNM* blur_image = blur_gaussian->transform();

    // Calculate Gx and Gy used EdgeSobel
    EdgeSobel *es = new EdgeSobel(blur_image);

    math::matrix<float>Gx = *es->rawHorizontalDetection();
    math::matrix<float>Gy = *es->rawVerticalDetection();

    // Define directions


    // Matrix of pixel power
    math::matrix<float> pixelPower(width, height);

    // Marix od pixel Angle
    math::matrix<float> pixelAngle(width, height);

    // Matrix of pixel directions
    math::matrix<directions> pixelDirection(width, height);

    // Matrix of edges
    math::matrix<bool> edges(width, height);


    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            // Calculate current pixel angle
            pixelAngle[i][j] = atan2(Gy[i][j], Gx[i][j]) * 180 / M_PI;

            // Calculate current pixel power
            pixelPower[i][j] = sqrt(pow(Gx[i][j], 2) + pow(Gy[i][j], 2));

            // Set current pixel direction
            if ((pixelAngle[i][j] > 22.5 && pixelAngle[i][j] <= 67.5) ||
                (pixelAngle[i][j] > 202.5 && pixelAngle[i][j] <= 247.5))
            {
                pixelDirection[i][j] = TOP_LEFT_BOTTOM_RIGHT;
            }
            else if ((pixelAngle[i][j] > 67.5 && pixelAngle[i][j] <= 112.5) ||
                     (pixelAngle[i][j] > 247.5 && pixelAngle[i][j] <= 292.5))
            {
                pixelDirection[i][j] = TOP_MIDDLE_BOTTOM_MIDDLE;
            }
            else if ((pixelAngle[i][j] > 112.5 && pixelAngle[i][j] <= 157.5) ||
                     (pixelAngle[i][j] > 292.5 && pixelAngle[i][j] <= 337.5))
            {
                pixelDirection[i][j] = TOP_RIGHT_BOTTOM_LEFT;
            }
            else
            {
                pixelDirection[i][j] = LEFT_MIDDLE_RIGHT_MIDDLE;
            }

        }
    }

    // Detect edges
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (pixelDirection[i][j] == TOP_MIDDLE_BOTTOM_MIDDLE)
            {
                if (pixelPower[i][j] > pixelPower[i-1][j] &&
                    pixelPower[i][j] > pixelPower[i+1][j] &&
                    pixelPower[i][j] > upper_thresh)
                {
                    edges[i][j] = true;
                }
            }
            else if (pixelDirection[i][j] == LEFT_MIDDLE_RIGHT_MIDDLE)
            {
                if (pixelPower[i][j] > pixelPower[i][j-1] &&
                    pixelPower[i][j] > pixelPower[i][j+1] &&
                    pixelPower[i][j] > upper_thresh)
                {
                    edges[i][j]=true;
                }
            }
            else if (pixelDirection[i][j] == TOP_LEFT_BOTTOM_RIGHT)
            {
                if (pixelPower[i][j] > pixelPower[i-1][j+1] &&
                    pixelPower[i][j] > pixelPower[i+1][j-1] &&
                    pixelPower[i][j] > upper_thresh)
                {
                    edges[i][j]=true;
                }
            }
            else if (pixelDirection[i][j] == TOP_RIGHT_BOTTOM_LEFT)
            {
                if (pixelPower[i][j] > pixelPower[i+1][j+1] &&
                    pixelPower[i][j] > pixelPower[i-1][j-1] &&
                    pixelPower[i][j] > upper_thresh)
                {
                    edges[i][j]=true;
                }
            }
        }
    }

    // Set variables
    int x1 = 0;
    int y1 = 0;

    int x2 = 0;
    int y2 = 0;

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (edges[i][j] == true)
            {
                Calculate_XY(i, j, pixelDirection[i][j], &x1, &y1, &x2, &y2);

                if (x1 < width && y1 < height && edges[x1][y1] != true)
                {
                    if (pixelPower[x1][y1] > lower_thresh && pixelDirection[x1][y1] == pixelDirection[i][j])
                    {
                        int x11 = 0;
                        int y11 = 0;
                        int x12 = 0;
                        int y12 = 0;

                        Calculate_XY(x1, y1, pixelDirection[x1][y1], &x11, &y11, &x12, &y12);

                        if (x11 < width && y11 < height && x12 < width && y12 < height)
                        {
                            if (pixelPower[x11][y11] < pixelPower[x1][y1] && pixelPower[x12][y12] < pixelPower[x1][y1])
                            {
                                edges[x1][y1] = true;
                            }
                        }
                        else if (x11 < width && y11 < height)
                        {
                            if (pixelPower[x11][y11] < pixelPower[x1][y1])
                            {
                                edges[x1][y1] = true;
                            }
                        }
                        else if (x12 < width && y12 < height)
                        {
                            if (pixelPower[x12][y12] < pixelPower[x1][y1])
                            {
                                edges[x1][y1] = true;
                            }
                        }

                    }
                }

                if (x2 < width && y2 < height && edges[x2][y2] != true)
                {
                    if (pixelPower[x2][y2] > lower_thresh && pixelDirection[x2][y2] == pixelDirection[i][j])
                    {
                        int x21 = 0;
                        int y21 = 0;
                        int x22 = 0;
                        int y22 = 0;

                        Calculate_XY(x2, y2, pixelDirection[x2][y2], &x21, &y21, &x22, &y22);

                        if (x21 < width && y21 < height && x22 < width && y22 < height)
                        {
                            if (pixelPower[x21][y21] < pixelPower[x2][y2] && pixelPower[x22][y22] < pixelPower[x2][y2])
                            {
                                edges[x2][y2] = true;
                            }
                        }
                        else if (x21 < width && y21 < height)
                        {
                            if (pixelPower[x21][y21] < pixelPower[x2][y2])
                            {
                                edges[x2][y2] = true;
                            }
                        }
                        else if (x22 < width && y22 < height)
                        {
                            if (pixelPower[x22][y22] < pixelPower[x2][y2])
                            {
                                edges[x2][y2] = true;
                            }
                        }
                    }
                }
            }
        }
    }

    // Prepare new image
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (edges[i][j] == true)
            {
                newImage->setPixel(i, j, QColor(Qt::white).rgb());
            }
            else
            {
                newImage->setPixel(i, j, QColor(Qt::black).rgb());
            }
        }
    }

    return newImage;
}
