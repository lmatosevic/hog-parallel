#ifndef DIPLOMSKI_HOG_PPMIMAGE_H
#define DIPLOMSKI_HOG_PPMIMAGE_H

typedef struct {
    unsigned char red, green, blue;
} PPMPixel;

class PPMImage {
public:

    int width;
    int height;
    PPMPixel *image;

    PPMImage(char *filename);

    PPMImage(int width, int height, PPMPixel *image);

    PPMImage toGrayImage();

    void saveImage(char *filename);

private:
    unsigned char getGrayValue(unsigned char red, unsigned char green, unsigned char blue);
};

#endif //DIPLOMSKI_HOG_PPMIMAGE_H
