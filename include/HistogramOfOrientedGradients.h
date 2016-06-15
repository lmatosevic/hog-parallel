#ifndef DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H
#define DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H

#include "PPMImage.h"
#include "Common.h"

#define IMG_WIDTH 64
#define IMG_HEIGHT 128
#define NUM_BINS 9
#define CELL_SIZE 8
#define NUM_HORIZ_CELLS 8
#define NUM_VERT_CELLS 16

class HistogramOfOrientedGradients {
public:
    PPMImage *image;

    HistogramOfOrientedGradients(PPMImage *image);

    double *getDescriptor(int *result_length);

    static double *getHistogram(double *cell_magnitudes, double *cell_angles);

private:
    static task getNextTask(task *tasks, int index_proc, int length);

    static int *getPixelIndices(int *vector, int length, int index, int *new_length);

    static double *getValuesFromIndices(double *vector, int length, int *indices, int ind_length);

    static bool arrayContainsIndex(int *array, int length, int value);
};

#endif //DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H
