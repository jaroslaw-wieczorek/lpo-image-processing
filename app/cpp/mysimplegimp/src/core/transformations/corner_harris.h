#ifndef CORNER_HARRIS_H
#define CORNER_HARRIS_H

#include "convolution.h"

class CornerHarris : public Convolution
{
public:
    CornerHarris(PNM*);
    CornerHarris(PNM*, ImageViewer*);
    math::matrix<float>* corner_candidates;
    math::matrix<float>* corner_nonmax_suppress;
    PNM* transform();
};

#endif // CORNER_HARRIS_H
