#include "morph_dilate.h"

MorphDilate::MorphDilate(PNM* img) :
    MorphologicalOperator(img)
{
}

MorphDilate::MorphDilate(PNM* img, ImageViewer* iv) :
    MorphologicalOperator(img, iv)
{
}

const int MorphDilate::morph(math::matrix<float> window, math::matrix<bool> se)
{
    float min = PIXEL_VAL_MAX + 1;

    for (int i = 0; i < int(window.colsize()); i++)
    {
        for (int j = 0; j < int(window.rowsize()); j++)
        {
            if (se[i][j] == true && window[i][j] < min)
            {
                min = window[i][j];
            }
        }
    }
    return min;
}
