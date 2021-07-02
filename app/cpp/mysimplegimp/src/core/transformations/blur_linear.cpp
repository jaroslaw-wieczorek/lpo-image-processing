#include "blur_linear.h"
#include <iostream>

BlurLinear::BlurLinear(PNM* img) :
    Convolution(img)
{
}

BlurLinear::BlurLinear(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* BlurLinear::transform()
{
    int maskSize = getParameter("size").toInt();
    QList<QVariant> tmpMask = getParameter("mask").toList();
    bool normalize = getParameter("normalize").toBool();

    math::matrix<float> mask(maskSize, maskSize);

    int temp = 0;

    for (int x = 0; x < maskSize; x++)
    {
        for (int y = 0; y < maskSize; y++)
        {
             std::cout << "value " << tmpMask.at(temp).toDouble() << std::endl;
             mask(x, y) = tmpMask.at(temp).toDouble();
             temp++;
        }
    }

    float sum_mask = sum(mask);

    if (normalize)
    {
            if (sum_mask == 0){
                sum_mask = 1.0;
            }
            for (int x = 0; x < maskSize; x++)
            {
                for (int y = 0; y < maskSize; y++)
                {
                    mask(x, y) = mask(x, y) / sum_mask;
                }
            }
    }
    return convolute(mask, RepeatEdge);
}
