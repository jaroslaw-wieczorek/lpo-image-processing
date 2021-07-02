#ifndef BIN_OTSU_H
#define BIN_OTSU_H

#include "transformation.h"

class BinarizationOtsu : public Transformation
{
public:
    BinarizationOtsu(PNM*);
    BinarizationOtsu(PNM*, ImageViewer*);
    int T = 0;
    virtual PNM* transform();
};


#endif // BIN_OTSU_H
