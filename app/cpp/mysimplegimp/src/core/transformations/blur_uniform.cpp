#include "blur_uniform.h"

BlurUniform::BlurUniform(PNM* img) :
    Convolution(img)
{
}

BlurUniform::BlurUniform(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

math::matrix<float> BlurUniform::getMask(int size, Mode)
{
    math::matrix<float> mask(size, size);

    for (int i=0; i < size; i++)
    {
        for (int j=0; j < size; j++)
        {
            mask[i][j] = 1;
        }
    }
    return mask;
}
