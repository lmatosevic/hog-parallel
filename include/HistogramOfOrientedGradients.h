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

    void getDescriptor();

private:
    void getHistogram();
};

#endif //DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H
