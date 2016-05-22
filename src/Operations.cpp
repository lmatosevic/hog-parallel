#include <iostream>
#include <math.h>
#include "../include/Operations.h"

using namespace std;

double *Operations::imfilter(double *image, int width, int height, double *vector_filter, int length,
                             bool is_filter_row) {
    int middle, slider, counter;
    double result;
    double *image_copy;

    image_copy = (double *) malloc(width * height * sizeof(double));
    middle = length / 2;
    slider = (length - 1) / 2;
    if (is_filter_row) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                result = vector_filter[middle] * image[width * i + j];
                counter = 1;
                for (int k = j - 1; k >= 0 && counter <= slider; k--) {
                    result += vector_filter[middle - counter] * image[width * i + k];
                    counter++;
                }
                counter = 1;
                for (int k = j + 1; k < width && counter <= slider; k++) {
                    result += vector_filter[middle + counter] * image[width * i + k];
                    counter++;
                }
                image_copy[width * i + j] = result;
            }
        }
    } else {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                result = vector_filter[middle] * image[height * j + i];
                counter = 1;
                for (int k = j - 1; k >= 0 && counter <= slider; k--) {
                    result += vector_filter[middle - counter] * image[height * k + i];
                    counter++;
                }
                counter = 1;
                for (int k = j + 1; k < height && counter <= slider; k++) {
                    result += vector_filter[middle + counter] * image[height * k + i];
                    counter++;
                }
                image_copy[height * j + i] = result;
            }
        }
    }

    return image_copy;
}

double *Operations::toDouble(PPMPixel *image, int width, int height) {
    double *image_copy;
    image_copy = (double *) malloc(width * height * sizeof(double));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image_copy[width * i + j] = (double) image[width * i + j].red;
        }
    }
    return image_copy;
}

double *Operations::magnitude(double *vector_x, double *vector_y, int length) {
    double *magnit;
    magnit = (double *) malloc(length * sizeof(double));
    for (int i = 0; i < length; i++) {
        magnit[i] = pow(pow(vector_y[i], 2) * pow(vector_x[i], 2), 5);
    }
    return magnit;
}

double *Operations::angle(double *vector_x, double *vector_y, int length) {
    double *angle;
    angle = (double *) malloc(length * sizeof(double));
    for (int i = 0; i < length; i++) {
        angle[i] = atan2(vector_y[i], vector_x[i]);
    }
    return angle;
}

double *Operations::subMatrix(double *matrix, int width, int height, int row_start, int row_end, int col_start,
                              int col_end) {
    double *matrix_copy;
    int k = 0, m = 0;
    int wd = row_end - row_start;
    int hg = col_start - col_end;
    matrix_copy = (double *) malloc(wd * hg * sizeof(double));
    for (int i = row_start; i <= row_end; i++) {
        m = 0;
        for (int j = col_start; j <= col_end; j++) {
            matrix_copy[wd * k + m] = matrix[width * i + j];
            m++;
        }
        k++;
    }
    return matrix_copy;
}

double Operations::norm(double *vector, int length) {
    double result = 0;
    for (int i = 0; i < length; i++) {
        result += pow(vector[i], 2);
    }
    result = sqrt(result);
    return result;
}

double Operations::sumOfProducts(double *vector_first, double *vector_second, int length) {
    double sum = 0;
    for (int i = 0; i < length; i++) {
        sum += vector_first[i] * vector_second[i];
    }
    return sum;
}

double *Operations::divideByScalar(double *vector, int length, double scalar) {
    double *result = (double *) malloc(length * sizeof(double));
    for (int i = 0; i < length; i++) {
        result[i] = vector[i] / scalar;
    }
    return result;
}
