#include <iostream>
#include <math.h>

#include "../include/HistogramOfOrientedGradients.h"
#include "../include/Operations.h"

using namespace std;

HistogramOfOrientedGradients::HistogramOfOrientedGradients(PPMImage *image) {
    this->image = image;
}

double *HistogramOfOrientedGradients::getDescriptor(int *result_length) {
    if (this->image->width != IMG_WIDTH || this->image->height != IMG_HEIGHT) {
        cout << "Invalid image size\n";
        return nullptr;
    }
    int width = this->image->width, height = this->image->height;
    int size = width * height;
    int row_offset, col_offset;
    double *cell_angles = nullptr, *cell_magnitudes = nullptr, *block_hists = nullptr, *normalized = nullptr;
    double magnitude;
    double filter[3] = {-1, 0, 1};
    double *image = Operations::toDouble(this->image->image, width, height);
    double *dx = Operations::imfilter(image, width, height, filter, 3, false);
    double *dy = Operations::imfilter(image, width, height, filter, 3, true);
    double *angles = Operations::angle(dx, dy, size);
    double *magnit = Operations::magnitude(dx, dy, size);
    double **histograms = (double **) malloc(NUM_VERT_CELLS * NUM_HORIZ_CELLS * sizeof(double *));
    double *descriptor_vector = (double *) malloc(NUM_VERT_CELLS * NUM_HORIZ_CELLS * NUM_BINS * sizeof(double));

    for (int row = 0; row < NUM_VERT_CELLS - 1; row++) {
        row_offset = row * CELL_SIZE;
        for (int col = 0; col < NUM_HORIZ_CELLS - 1; col++) {
            col_offset = col * CELL_SIZE;
            cell_angles = Operations::subMatrix(angles, width, height, row_offset, (row_offset + CELL_SIZE),
                                                col_offset, (col_offset + CELL_SIZE));
            cell_magnitudes = Operations::subMatrix(magnit, width, height, row_offset, (row_offset + CELL_SIZE),
                                                    col_offset, (col_offset + CELL_SIZE));
            histograms[NUM_HORIZ_CELLS * row + col] = this->getHistogram(cell_magnitudes, cell_angles);
        }
    }

    for (int row = 0; row < NUM_VERT_CELLS - 1; row++) {
        for (int col = 0; col < NUM_HORIZ_CELLS - 1; col++) {
            block_hists = histograms[NUM_HORIZ_CELLS * row + col];
            magnitude = Operations::norm(block_hists, NUM_BINS) + 0.01;
            normalized = Operations::divideByScalar(block_hists, NUM_BINS, magnitude);
            for (int i = 0; i < NUM_BINS; i++) {
                descriptor_vector[NUM_HORIZ_CELLS * row + NUM_BINS * col + i] = normalized[i];
            }
        }
    }
    free(cell_angles);
    free(cell_magnitudes);
    free(block_hists);
    free(normalized);
    *result_length = NUM_VERT_CELLS * NUM_HORIZ_CELLS * NUM_BINS;
    return descriptor_vector;
}

double *HistogramOfOrientedGradients::getHistogram(double *cell_magnitudes, double *cell_angles) {
    int length = CELL_SIZE * CELL_SIZE;
    int result_length;
    double bin_size = M_PI / NUM_BINS;
    double min_angle = 0;
    int *indices = nullptr;
    double *portion_pixels = nullptr, *magnits = nullptr;
    double *histogram = (double *) malloc(NUM_BINS * sizeof(double));
    int *left_bin_indices = (int *) malloc(CELL_SIZE * CELL_SIZE * sizeof(int));
    int *right_bin_indices = (int *) malloc(CELL_SIZE * CELL_SIZE * sizeof(int));
    double *left_bin_center = (double *) malloc(CELL_SIZE * CELL_SIZE * sizeof(double));
    double *right_portions = (double *) malloc(CELL_SIZE * CELL_SIZE * sizeof(double));
    double *left_portions = (double *) malloc(CELL_SIZE * CELL_SIZE * sizeof(double));

    for (int i = 0; i < length; i++) {
        if (cell_angles[i] < 0) {
            cell_angles[i] += M_PI;
        }
        left_bin_indices[i] = (int) round((cell_angles[i] - min_angle) / bin_size);
        right_bin_indices[i] = left_bin_indices[i] + 1;
        left_bin_center[i] = (((left_bin_indices[i] - 0.5) * bin_size) - min_angle);

        if (left_bin_indices[i] == 0) {
            left_bin_indices[i] = NUM_BINS;
        }
        if (right_bin_indices[i] == (NUM_BINS + 1)) {
            right_bin_indices[i] = 1;
        }

        right_portions[i] = cell_angles[i] - left_bin_center[i];
        left_portions[i] = bin_size - right_portions[i];
        right_portions[i] = right_portions[i] / bin_size;
        left_portions[i] = left_portions[i] / bin_size;
    }

    for (int i = 0; i < NUM_BINS; i++) {
        indices = this->getPixelIndices(left_bin_indices, length, i, &result_length);
        portion_pixels = this->getValuesFromIndices(left_portions, length, indices, result_length);
        magnits = this->getValuesFromIndices(cell_magnitudes, length, indices, result_length);
        histogram[i] = Operations::sumOfProducts(portion_pixels, magnits, result_length);

        indices = this->getPixelIndices(right_bin_indices, length, i, &result_length);
        portion_pixels = this->getValuesFromIndices(right_portions, length, indices, result_length);
        magnits = this->getValuesFromIndices(cell_magnitudes, length, indices, result_length);
        histogram[i] += Operations::sumOfProducts(portion_pixels, magnits, result_length);
    }
    free(left_portions);
    free(right_portions);
    free(left_bin_center);
    free(right_bin_indices);
    free(left_bin_indices);
    free(indices);
    free(portion_pixels);
    free(magnits);
    return histogram;
};

int *HistogramOfOrientedGradients::getPixelIndices(int *vector, int length, int index, int *new_length) {
    int *indices = (int *) malloc(length * sizeof(int));
    int cursor = 0;
    for (int i = 0; i < length; i++) {
        if (vector[i] == index) {
            indices[cursor] = vector[i];
            cursor++;
        }
    }
    indices = (int *) realloc(indices, cursor * sizeof(int));
    *new_length = cursor;
    return indices;
}

double *HistogramOfOrientedGradients::getValuesFromIndices(double *vector, int length, int *indices, int ind_length) {
    double *values = (double *) malloc(ind_length * sizeof(double));
    int cursor = 0;
    for (int i = 0; i < length; i++) {
        if (this->arrayContainsIndex(indices, ind_length, i)) {
            values[cursor] = vector[i];
            cursor++;
        }
    }
    return values;
}

bool HistogramOfOrientedGradients::arrayContainsIndex(int *array, int length, int index) {
    for (int i = 0; i < length; i++) {
        if (array[i] == index) {
            return true;
        }
    }
    return false;
}
