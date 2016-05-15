#ifndef DIPLOMSKI_HOG_PPMIMAGE_H
#define DIPLOMSKI_HOG_PPMIMAGE_H

#include <string>

typedef struct {
    unsigned char red, green, blue;
} PPMPixel;

class PPMImage {
public:
    int width;
    int height;
    PPMPixel *image;

    PPMImage(const char *filename);
};

#endif //DIPLOMSKI_HOG_PPMIMAGE_H
