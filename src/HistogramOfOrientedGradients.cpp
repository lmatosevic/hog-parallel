#include <iostream>
#include <math.h>
#include <algorithm>
#include <mpi.h>

#include "../include/HistogramOfOrientedGradients.h"
#include "../include/Operations.h"

#define M_PI 3.1415926535897

using namespace std;

HistogramOfOrientedGradients::HistogramOfOrientedGradients(PPMImage *image) {
    this->image = image;
}

double *HistogramOfOrientedGradients::getDescriptor(int *result_length) {
    if (this->image->width % IMG_WIDTH != 0 || this->image->height % IMG_HEIGHT != 0) {
        cout << "Invalid image size. Valid size format is h*128 x w*64, where w,h := {1,2,3,...N}";
        return nullptr;
    }
    int width = this->image->width, height = this->image->height;
    int widthMultiplier = width / IMG_WIDTH;
    int heightMultiplier = height / IMG_HEIGHT;
    cell_size = min(widthMultiplier, heightMultiplier) > 4 ? CELL_SIZE * 2 : CELL_SIZE;
    int num_horiz_cells = (widthMultiplier * NUM_HORIZ_CELLS) / (cell_size / CELL_SIZE);
    int num_vert_cells = (heightMultiplier * NUM_VERT_CELLS) / (cell_size / CELL_SIZE);
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
    double **histograms = (double **) malloc(num_vert_cells * num_horiz_cells * sizeof(double *));
    double *descriptor_vector = (double *) malloc(num_vert_cells * num_horiz_cells * NUM_BINS * sizeof(double));
    task *tasks = (task *) malloc(num_vert_cells * num_horiz_cells * sizeof(task));

    for (int row = 0; row < num_vert_cells; row++) {
        row_offset = row * cell_size;
        for (int col = 0; col < num_horiz_cells; col++) {
            col_offset = col * cell_size;
            cell_angles = Operations::subMatrix(angles, width, height, row_offset, (row_offset + cell_size),
                                                col_offset, (col_offset + cell_size));
            cell_magnitudes = Operations::subMatrix(magnit, width, height, row_offset, (row_offset + cell_size),
                                                    col_offset, (col_offset + cell_size));
            if (nproc == 1) {
                histograms[num_horiz_cells * row + col] = getHistogram(cell_magnitudes, cell_angles);
            } else {
                task new_task;
                new_task.task_id = num_horiz_cells * row + col;
                new_task.process_id = 0;
                new_task.status = 0;
                new_task.row = row;
                new_task.col = col;
                new_task.cell_angles = cell_angles;
                new_task.cell_magnitudes = cell_magnitudes;
                tasks[num_horiz_cells * row + col] = new_task;
            }
        }
    }

    if (nproc == 1) {
        for (int row = 0; row < num_vert_cells; row++) {
            for (int col = 0; col < num_horiz_cells; col++) {
                block_hists = histograms[num_horiz_cells * row + col];
                magnitude = Operations::norm(block_hists, NUM_BINS) + 0.01;
                normalized = Operations::divideByScalar(block_hists, NUM_BINS, magnitude);
                for (int i = 0; i < NUM_BINS; i++) {
                    descriptor_vector[num_horiz_cells * NUM_BINS * row + NUM_BINS * col + i] = normalized[i];
                }
            }
        }
    } else {
        int is_end = 0;
        int nworkers = nproc - 1;
        if (debug) cout << "Master has started sending config to workers (cell_size=" << cell_size << ")" << endl;
        for (int i = 1; i <= nworkers; i++) {
            MPI_Send(&cell_size, 1, MPI_INTEGER, i, TAG_CONFIGURATION, MPI_COMM_WORLD);
            if (debug) cout << "Master has sent configuration to worker[" << i << "]" << endl;
        }
        if (debug) cout << "Master has started with work" << endl;
        do {
            MPI_Status status;
            double data[NUM_BINS];
            double test;
            double msg = 0;

            MPI_Recv(&test, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TAG_REQUEST_FOR_TASK) {
                if (debug)
                    cout << "Master has received a new job offer from worker[" << status.MPI_SOURCE << "]" << endl;
                task newtask = getNextTask(tasks, status.MPI_SOURCE, num_horiz_cells * num_vert_cells);
                if (newtask.status == -1) {
                    if (debug) cout << "Master is out of tasks for worker[" << status.MPI_SOURCE << "]" << endl;
                    MPI_Send(&msg, 1, MPI_DOUBLE, status.MPI_SOURCE, TAG_END, MPI_COMM_WORLD);
                    nworkers--;
                    if (nworkers == 0) {
                        is_end = 1;
                    } else {
                        continue;
                    }
                } else {
                    int length = cell_size * cell_size;
                    newtask.process_id = status.MPI_SOURCE;
                    newtask.status = 1;
                    MPI_Send(&msg, 1, MPI_DOUBLE, status.MPI_SOURCE, TAG_NEW_TASK, MPI_COMM_WORLD);
                    MPI_Send(newtask.cell_angles, length, MPI_DOUBLE, status.MPI_SOURCE, TAG_NEW_TASK_1,
                             MPI_COMM_WORLD);
                    MPI_Send(newtask.cell_magnitudes, length, MPI_DOUBLE, status.MPI_SOURCE, TAG_NEW_TASK_2,
                             MPI_COMM_WORLD);
                    if (debug)
                        cout << "Master has sent new task[" << newtask.task_id << "] to worker[" << status.MPI_SOURCE <<
                        "]" << endl;
                }
            }

            if (status.MPI_TAG == TAG_TASK_FINISHED) {
                MPI_Recv(data, NUM_BINS, MPI_DOUBLE, status.MPI_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status);
                for (int i = 0; i < num_vert_cells * num_horiz_cells; i++) {
                    if (tasks[i].process_id == status.MPI_SOURCE && tasks[i].status == 1) {
                        if (debug)
                            cout << "Master has received a result of task[" << i << "] from worker[" <<
                            status.MPI_SOURCE << "]" << endl;
                        tasks[i].status = 2;
                        for (int j = 0; j < NUM_BINS; j++) {
                            descriptor_vector[num_horiz_cells * NUM_BINS * tasks[i].row +
                                              NUM_BINS * tasks[i].col + j] = data[j];
                        }
                        break;
                    }
                }
            }
        } while (is_end == 0);
        if (debug) cout << "Master has finished work with all workers" << endl;
    }
    free(tasks);
    free(cell_angles);
    free(cell_magnitudes);
    free(block_hists);
    free(normalized);
    *result_length = num_vert_cells * num_horiz_cells * NUM_BINS;
    return descriptor_vector;
}

double *HistogramOfOrientedGradients::getHistogram(double *cell_magnitudes, double *cell_angles) {
    int length = cell_size * cell_size;
    int result_length;
    double bin_size = M_PI / NUM_BINS;
    double min_angle = 0;
    int *indices = nullptr;
    double *portion_pixels = nullptr, *magnits = nullptr;
    double *histogram = (double *) malloc(NUM_BINS * sizeof(double));
    int *left_bin_indices = (int *) malloc(cell_size * cell_size * sizeof(int));
    int *right_bin_indices = (int *) malloc(cell_size * cell_size * sizeof(int));
    double *left_bin_center = (double *) malloc(cell_size * cell_size * sizeof(double));
    double *right_portions = (double *) malloc(cell_size * cell_size * sizeof(double));
    double *left_portions = (double *) malloc(cell_size * cell_size * sizeof(double));

    for (int i = 0; i < length; i++) {
        if (cell_angles[i] < 0) {
            cell_angles[i] += M_PI;
        }
        left_bin_indices[i] = (int) round((cell_angles[i] - min_angle) / bin_size) - 1;
        right_bin_indices[i] = left_bin_indices[i] + 1;
        left_bin_center[i] = (((left_bin_indices[i] - 0.5) * bin_size) - min_angle);

        if (left_bin_indices[i] == -1) {
            left_bin_indices[i] = NUM_BINS - 1;
        }
        if (right_bin_indices[i] == NUM_BINS) {
            right_bin_indices[i] = 0;
        }

        right_portions[i] = cell_angles[i] - left_bin_center[i];
        left_portions[i] = bin_size - right_portions[i];
        right_portions[i] = right_portions[i] / bin_size;
        left_portions[i] = left_portions[i] / bin_size;

        if (fabs(right_portions[i]) < 1e-6) {
            right_portions[i] = 0;
        }
        if (fabs(left_portions[i]) < 1e-6) {
            left_portions[i] = 0;
        }
    }

    for (int i = 0; i < NUM_BINS; i++) {
        indices = getPixelIndices(left_bin_indices, length, i, &result_length);
        portion_pixels = getValuesFromIndices(left_portions, length, indices, result_length);
        magnits = getValuesFromIndices(cell_magnitudes, length, indices, result_length);
        histogram[i] = Operations::sumOfProducts(portion_pixels, magnits, result_length);

        indices = getPixelIndices(right_bin_indices, length, i, &result_length);
        portion_pixels = getValuesFromIndices(right_portions, length, indices, result_length);
        magnits = getValuesFromIndices(cell_magnitudes, length, indices, result_length);
        histogram[i] += Operations::sumOfProducts(portion_pixels, magnits, result_length);
        if (fabs(histogram[i]) < 1e-6) {
            histogram[i] = 0;
        }
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
        if (arrayContainsIndex(indices, ind_length, i)) {
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

task HistogramOfOrientedGradients::getNextTask(task *tasks, int index_proc, int length) {
    task no;
    no.status = -1;
    for (int i = 0; i < length; i++) {
        if (tasks[i].status == 0) {
            tasks[i].status = 1;
            tasks[i].process_id = index_proc;
            return tasks[i];
        }
    }
    return no;
}
