#include "morphological_operator.h"

MorphologicalOperator::MorphologicalOperator(PNM* img) :
    Transformation(img)
{
}

MorphologicalOperator::MorphologicalOperator(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

// abstract
const int MorphologicalOperator::morph(math::matrix<float>, math::matrix<bool>)
{
    return 0;
}

math::matrix<bool> MorphologicalOperator::getSE(int size, SE shape)
{
    switch (shape)
    {
    case Square:    return seSquare(size);
    case Cross:     return seCross(size);
    case XCross:    return seXCross(size);
    case VLine:     return seVLine(size);
    case HLine:     return seHLine(size);
    default:        return seSquare(size);
    }
}


math::matrix<bool> MorphologicalOperator::seSquare(int size)
{
    math::matrix<bool> ret(size, size);

    // set true in each field
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            ret[i][j] = true;
        }
    }

    return ret;
}

math::matrix<bool> MorphologicalOperator::seCross(int size)
{
    math::matrix<bool> ret(size, size);

    int half = size / 2;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i == half || j == half)
            {
                ret[i][j] = true;
            }
            else
            {
                ret[i][j] = false;
            }

        }
    }

    return ret;
}

math::matrix<bool> MorphologicalOperator::seXCross(int size)
{
    math::matrix<bool> ret(size, size);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i == j)
            {
                ret[i][j] = true;
            }
            else
            {
                ret[i][j] = false;
            }

        }
    }

    return ret;
}

math::matrix<bool> MorphologicalOperator::seVLine(int size)
{
    math::matrix<bool> ret(size, size);

    int half = size / 2;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (j == half)
            {
                ret[i][j] = true;
            }
            else
            {
                ret[i][j] = false;
            }

        }
    }

    return ret;
}

math::matrix<bool> MorphologicalOperator::seHLine(int size)
{
    math::matrix<bool> ret(size, size);


    int half = size / 2;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i == half)
            {
                ret[i][j] = true;
            }
            else
            {
                ret[i][j] = false;
            }

        }
    }

    return ret;
}

PNM* MorphologicalOperator::transform()
{  
    int size  = getParameter("size").toInt();
    SE  shape = (MorphologicalOperator::SE) getParameter("shape").toInt();

    PNM* newImage = new PNM(image->width(), image->height(), QImage::Format_RGB32);

    int width = image->width();
    int height = image->height();
    int half = int(size / 2);

    if (image->format() == QImage::Format_Mono)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                math::matrix<float> L_matrix = getWindow(x, y, size, LChannel, RepeatEdge);

                for (int i = 0; i < size; i++)
                {
                    for (int j = 0; j < size; j++)
                    {
                        L_matrix[i][j] = qGray(getPixel((x - half) + i, (y - half) + j, RepeatEdge));
                    }
                }

                int l = morph(L_matrix, getSE(size, shape));
                newImage->setPixel(x, y, l);
            }
        }
    }
    else
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {

                math::matrix<float> R_matrix = getWindow(x, y, size, RChannel, RepeatEdge);
                math::matrix<float> G_matrix = getWindow(x, y, size, GChannel, RepeatEdge);
                math::matrix<float> B_matrix = getWindow(x, y, size, BChannel, RepeatEdge);

                QRgb pixel = image->pixel(x,y);
                int r = qRed(pixel);
                int g = qGreen(pixel);
                int b = qBlue(pixel);

                for (int i = 0; i < size; i++)
                {
                    for (int j = 0; j < size; j++)
                    {
                        R_matrix[i][j] = qRed(getPixel((x - half) + j, (y - half) + i, RepeatEdge));
                        B_matrix[i][j] = qBlue(getPixel((x - half) + j, (y - half) + i, RepeatEdge));
                        G_matrix[i][j] = qGreen(getPixel((x - half) + j, (y - half) + i, RepeatEdge));
                    }
                }

                r = morph(R_matrix, getSE(size, shape));
                g = morph(G_matrix, getSE(size, shape));
                b = morph(B_matrix, getSE(size, shape));

                QColor newPixel = QColor(r,g,b);
                newImage->setPixel(x, y, newPixel.rgb());
            }

         }
    }


    return newImage;
}
