#include <iostream>

#include "../include/HistogramOfOrientedGradients.h"

using namespace std;

int main() {
    double *result_hog = nullptr;
    cout << "HOG started\n";
    PPMImage image((char *) "C:/Users/Luka/ClionProjects/diplomski-hog/resources/lenna_rsz.ppm");
    //cout << +image.image[5].red;

    PPMImage gray_image = image.toGrayImage();
    cout << "Generated grayscale image\n";
    //gray_image.saveImage((char *) "C:/Users/Luka/ClionProjects/diplomski-hog/out/lenna_gray.ppm");

    HistogramOfOrientedGradients hog(&gray_image);
    result_hog = hog.getDescriptor();
    if(result_hog) {
        cout << "Descriptor is generated\n";
    } else {
        cout << "Descriptor is not generated\n";
    }

    cout << "HOG finished\n";

    free(result_hog);
    return 0;
}