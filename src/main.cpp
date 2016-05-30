#include <iostream>

#include "../include/HistogramOfOrientedGradients.h"

using namespace std;

int main() {
    double *result_hog = nullptr;
    int result_length;
    cout << "HOG started\n";
    PPMImage image((char *) "C:/Users/Luka/ClionProjects/diplomski-hog/resources/woman.ppm");
    //cout << +image.image[5].red;

    PPMImage gray_image = image.toGrayImage();
    cout << "Generated grayscale image\n";
    //gray_image.saveImage((char *) "C:/Users/Luka/ClionProjects/diplomski-hog/out/lenna_gray.ppm");

    HistogramOfOrientedGradients hog(&gray_image);
    result_hog = hog.getDescriptor(&result_length);
    if (result_hog) {
        cout << "Descriptor is generated\n";
    } else {
        cout << "Descriptor is not generated\n";
    }
    cout << "HOG finished\n";

    cout << "HOG descriptor vector:\n\n";
    for (int i = 0; i < result_length; i++) {
        cout << "value " << i << ": " << result_hog[i] << "\n";
    }

    free(result_hog);
    return 0;
}