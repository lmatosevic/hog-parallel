#ifndef DIPLOMSKI_HOG_OPERATIONS_H
#define DIPLOMSKI_HOG_OPERATIONS_H

#include "PPMImage.h"

class Operations {
public:
    static double *imfilter(double *image, int width, int height, double *vector_filter, int length,
                            bool is_filter_transposed);

    static double *toDouble(PPMPixel *image, int width, int height);

    static double *magnitude(double *vector_x, double *vector_y, int length);

    static double *angle(double *vector_x, double *vector_y, int length);

    static double norm(double *vector, int length);

    static double *subMatrix(double *matrix, int width, int height, int row_start, int row_end, int col_start,
                             int col_end);
};

#endif //DIPLOMSKI_HOG_OPERATIONS_H
