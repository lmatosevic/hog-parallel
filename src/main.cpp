#include <iostream>

#include "../include/HistogramOfOrientedGradients.h"
#include "../include/PPMImage.h"

using namespace std;

int main() {
    cout << "Hello world!\n";
    PPMImage *image = new PPMImage("C:/Users/Luka/ClionProjects/diplomski-hog/resources/lenna.ppm");
    cout << +image->image[5].blue;
    return 0;
}