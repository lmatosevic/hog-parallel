#include <iostream>
#include <w32api/tom.h>
#include "../include/HistogramOfOrientedGradients.h"
#include "../include/Operations.h"

using namespace std;

HistogramOfOrientedGradients::HistogramOfOrientedGradients(PPMImage *image) {
    this->image = image;
}

void HistogramOfOrientedGradients::getDescriptor() {
    if (this->image->width != IMG_WIDTH || this->image->height != IMG_HEIGHT) {
        cout << "Invalid image size\n";
        //return;
    }
    int width = this->image->width, height = this->image->height;
    int size = width * height;
    double filter[3] = {-1, 0, 1};
    double *image = Operations::toDouble(this->image->image, width, height);
    double *dx = Operations::imfilter(image, width, height, filter, 3, false);
    double *dy = Operations::imfilter(image, width, height, filter, 3, true);
    double *angles = Operations::angle(dx, dy, size);
    double *magnit = Operations::magnitude(dx, dy, size);

}

void HistogramOfOrientedGradients::getHistogram() {

};
