#ifndef EDGECANNY_H
#define EDGECANNY_H

#include "convolution.h"

class EdgeCanny : public Convolution
{
public:
    EdgeCanny(PNM*);
    EdgeCanny(PNM*, ImageViewer*);

    virtual PNM* transform();
    enum directions {
        TOP_LEFT_BOTTOM_RIGHT,
        TOP_RIGHT_BOTTOM_LEFT,
        TOP_MIDDLE_BOTTOM_MIDDLE,
        LEFT_MIDDLE_RIGHT_MIDDLE
    };

    void Calculate_XY(int i, int j, enum directions dir, int* x1, int* y1, int* x2, int* y2);

private:

};

#endif // EDGECANNY_H
