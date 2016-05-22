#ifndef DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H
#define DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H

#include "PPMImage.h"

#define IMG_WIDTH 66
#define IMG_HEIGHT 130
#define NUM_BINS 9
#define CELL_SIZE 8
#define NUM_HORIZ_CELLS 8
#define NUM_VERT_CELLS 16

class HistogramOfOrientedGradients {
public:
    PPMImage *image;

    HistogramOfOrientedGradients(PPMImage *image);

    double *getDescriptor();

private:
    double *getHistogram(double *cell_magnitudes, double *cell_angles);

    int *getPixelIndices(int *vector, int length, int index, int *new_length);

    double *getValuesFromIndices(double *vector, int length, int *indices, int ind_length);

    bool arrayContainsIndex(int *array, int length, int value);
};

#endif //DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H
