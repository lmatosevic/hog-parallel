#include <iostream>

#include "../include/HistogramOfOrientedGradients.h"
#include "../include/PPMImage.h"

using namespace std;

int main() {
    cout << "HOG started\n";
    PPMImage image((char *) "C:/Users/Luka/ClionProjects/diplomski-hog/resources/lenna.ppm");
    //cout << +image.image[5].red;
    PPMImage gray_image = image.toGrayImage();
    gray_image.saveImage((char *) "C:/Users/Luka/ClionProjects/diplomski-hog/out/lenna_gray.ppm");

    cout << "HOG finished\n";
    return 0;
}