#include <iostream>
#include <fstream>

#include "../include/PPMImage.h"

using namespace std;

PPMImage::PPMImage(char *filename) {
    char buff[16];
    FILE *fp;
    int c, rgb_comp_color;

    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    if (!fgets(buff, sizeof(buff), fp)) {
        perror(filename);
        exit(1);
    }

    if (buff[0] != 'P' || buff[1] != '6') {
        fprintf(stderr, "Invalid image format (must be 'P6')\n");
        exit(1);
    }

    c = getc(fp);
    while (c == '#') {
        while (getc(fp) != '\n');
        c = getc(fp);
    }

    ungetc(c, fp);
    if (fscanf(fp, "%d %d", &this->width, &this->height) != 2) {
        fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
        exit(1);
    }

    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
        fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
        exit(1);
    }

    if (rgb_comp_color != 255) {
        fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
        exit(1);
    }

    while (fgetc(fp) != '\n');
    this->image = (PPMPixel *) malloc(this->width * this->height * sizeof(PPMPixel));

    if (fread(this->image, 3 * this->width, this->height, fp) != this->height) {
        fprintf(stderr, "Error loading image '%s'\n", filename);
        exit(1);
    }

    fclose(fp);
}

PPMImage::PPMImage(int width, int height, PPMPixel *image) {
    this->width = width;
    this->height = height;
    this->image = (PPMPixel *) malloc(this->width * this->height * sizeof(PPMPixel));
    for (int i = 0; i < this->width; i++) {
        for (int j = 0; j < this->height; j++) {
            this->image[this->width * i + j].red = image[this->width * i + j].red;
            this->image[this->width * i + j].green = image[this->width * i + j].green;
            this->image[this->width * i + j].blue = image[this->width * i + j].blue;
        }
    }
}

PPMImage PPMImage::toGrayImage() {
    unsigned char value;
    PPMPixel pixel;
    PPMPixel *gray_image;
    gray_image = (PPMPixel *) malloc(this->width * this->height * sizeof(PPMPixel));
    for (int i = 0; i < this->width; i++) {
        for (int j = 0; j < this->height; j++) {
            pixel = this->image[this->width * i + j];
            value = getGrayValue(pixel.red, pixel.green, pixel.blue);
            gray_image[this->width * i + j].red = value;
            gray_image[this->width * i + j].green = value;
            gray_image[this->width * i + j].blue = value;
        }
    }
    return PPMImage(this->width, this->height, gray_image);
}

unsigned char PPMImage::getGrayValue(unsigned char red, unsigned char green, unsigned char blue) {
    return (unsigned char) (0.2989 * red + 0.5870 * green + 0.1140 * blue);
}

void PPMImage::saveImage(char *filename) {
    PPMPixel pixel;
    ofstream myfile(filename);
    if (myfile.is_open()) {
        myfile << "P6\n";
        myfile << "512 512\n";
        myfile << "255\n";
        for (int i = 0; i < this->width; i++) {
            for (int j = 0; j < this->height; j++) {
                pixel = this->image[this->width * i + j];
                myfile << pixel.red;
                myfile << pixel.green;
                myfile << pixel.blue;
            }
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}