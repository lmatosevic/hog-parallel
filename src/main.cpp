#include <iostream>

#include "../include/HistogramOfOrientedGradients.h"

using namespace std;

int main() {
    double *result_hog = nullptr;
    int result_length;
    cout << "HOG started\n";
    // For bigger images split them into smaller ones and iterate hog algorithm over them in loop
    PPMImage image((char *) "C:/Users/Luka/ClionProjects/diplomski-hog/resources/woman.ppm");

    PPMImage gray_image = image.toGrayImage();
    cout << "Generated grayscale image\n";

    HistogramOfOrientedGradients hog(&gray_image);
    result_hog = hog.getDescriptor(&result_length);
    if (result_hog) {
        cout << "Descriptor is generated\n";
    } else {
        cout << "Descriptor is not generated\nExiting\n";
        return -1;
    }
    cout << "HOG finished\n";

    cout << "HOG descriptor vector:\n\n";
    for (int i = 0; i < result_length; i++) {
        cout << "value " << i << ": " << result_hog[i] << "\n";
    }

    free(result_hog);
    return 0;
}