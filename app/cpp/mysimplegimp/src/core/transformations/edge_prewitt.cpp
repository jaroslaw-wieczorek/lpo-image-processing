#include "edge_prewitt.h"

EdgePrewitt::EdgePrewitt(PNM*img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

EdgePrewitt::EdgePrewitt(PNM*img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

void EdgePrewitt::prepareMatrices()
{
    g_x = math::matrix<float>(3, 3, {-1, 0, 1, -1, 0, 1, -1, 0, 1});
    g_y = math::matrix<float>(3, 3, {-1, -1, -1, 0, 0, 0, 1, 1, 1});
}
