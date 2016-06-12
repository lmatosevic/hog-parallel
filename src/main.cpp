#include <iostream>
#include <mpi.h>

#include "../include/Common.h"
#include "../include/HistogramOfOrientedGradients.h"
#include "../include/QPCounter.h"
#include "../include/Operations.h"

using namespace std;

int my_rank;
int nproc;
bool debug;

void worker_job() {
    do {
        MPI_Status status;
        double test;
        double msg = 0;

        MPI_Send(&msg, 1, MPI_DOUBLE, 0, TAG_REQUEST_FOR_TASK, MPI_COMM_WORLD);
        if(debug) cout << "Worker[" << my_rank << "] has sent requrest for new task" << endl;
        MPI_Recv(&test, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TAG_END) {
            if(debug) cout << "Worker[" << my_rank << "] has received END of work" << endl;
            break;
        }

        if (status.MPI_TAG == TAG_NEW_TASK) {
            double data_angles[CELL_SIZE * CELL_SIZE];
            double data_magnits[CELL_SIZE * CELL_SIZE];

            if(debug) cout << "Worker[" << my_rank << "] has received a new task affirmation" << endl;

            MPI_Recv(data_angles, CELL_SIZE * CELL_SIZE, MPI_DOUBLE, 0, TAG_NEW_TASK_1, MPI_COMM_WORLD, &status);
            if(debug) cout << "Worker[" << my_rank << "] has received a new task part 1/2(angles)" << endl;
            MPI_Recv(data_magnits, CELL_SIZE * CELL_SIZE, MPI_DOUBLE, 0, TAG_NEW_TASK_2, MPI_COMM_WORLD, &status);
            if(debug) cout << "Worker[" << my_rank << "] has received a new task part 2/2(magnitudes)" << endl;
            double *histogram = HistogramOfOrientedGradients::getHistogram(data_magnits, data_angles);
            double magnitude = Operations::norm(histogram, NUM_BINS) + 0.01;
            double *normalized = Operations::divideByScalar(histogram, NUM_BINS, magnitude);
            MPI_Send(&msg, 1, MPI_DOUBLE, 0, TAG_TASK_FINISHED, MPI_COMM_WORLD);
            MPI_Send(normalized, NUM_BINS, MPI_DOUBLE, 0, TAG_RESULT, MPI_COMM_WORLD);
            if(debug) cout << "Worker[" << my_rank << "] has sent result to master" << endl;
        }
    } while (1);
}

double *master_job(HistogramOfOrientedGradients hog, int *result_length) {
    return hog.getDescriptor(result_length);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    if(my_rank == 0) {
        if (argc <= 1) {
            cout << "Image path not specified." << endl;
            cout << "usage: program image_path [-debug]" << endl;
            exit(-1);
        }
        if (argc == 3) {
            if (strcmp(argv[2], "-debug") != 0) {
                cout << "Invalid second argument: " << argv[2] << endl;
                cout << "usage: program image_path [-debug]" << endl;
                exit(-1);
            } else {
                debug = true;
            }
        }
        if(argc == 2) {
            debug = false;
        }
        if (argc > 3) {
            cout << "Too many arguments in program call." << endl;
            cout << "usage: program image_path [-debug]" << endl;
            exit(-1);
        }
    }

    QPCounter counter;
    counter.StartCounter();

    if (my_rank == 0) {
        double *result_hog = nullptr;
        int result_length;
        cout << "HOG started\n";
        // For bigger images split them into smaller ones and iterate hog algorithm over them in loop
        PPMImage image(argv[1]);

        PPMImage gray_image = image.toGrayImage();
        cout << "Generated grayscale image\n";

        HistogramOfOrientedGradients hog(&gray_image);
        cout << "Starting descriptor vector generation with " << nproc << " processes" << endl;
        result_hog = master_job(hog, &result_length);
        if (result_hog) {
            cout << "Descriptor is generated\n";
            if(debug) {
                cout << "HOG descriptor vector:\n";
                for (int i = 0; i < result_length; i++) {
                    cout << "value " << i << ": " << result_hog[i] << endl;
                }
            }
        } else {
            cout << "Descriptor is not generated\nExiting\n";
        }
        free(result_hog);
        cout << "HOG finished\n" << endl;
        cout << "Elapsed time: " << counter.GetCounter() << " ns" << endl;
    } else {
        worker_job();
    }

    MPI_Finalize();
    return 0;
}