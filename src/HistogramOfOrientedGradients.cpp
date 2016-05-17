#include <iostream>
#include "../include/HistogramOfOrientedGradients.h"

using namespace std;

HistogramOfOrientedGradients::HistogramOfOrientedGradients(PPMImage *image) {
    this->image = image;
}

void HistogramOfOrientedGradients::getDescriptor() {
    if(this->image->width != IMG_WIDTH || this->image->height != IMG_HEIGHT) {
        cout << "Invalid image size\n";
        return;
    }


}

void HistogramOfOrientedGradients::getHistogram() {

};
