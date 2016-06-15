#ifndef DIPLOMSKI_HOG_COMMON_H
#define DIPLOMSKI_HOG_COMMON_H

#define TAG_CONFIGURATION 0
#define TAG_NEW_TASK 1
#define TAG_NEW_TASK_1 2
#define TAG_NEW_TASK_2 3
#define TAG_REQUEST_FOR_TASK 4
#define TAG_RESULT 5
#define TAG_TASK_FINISHED 6
#define TAG_END 7

extern int my_rank; // Id of currently running process
extern int nproc; // Total number of running processes
extern bool debug; // Debug flag, if true all debugginf output will be printed to stdout
extern int cell_size; // Cell size of one operational image block, depends on image size

typedef struct Task {
    int task_id;
    int process_id;
    int status;
    int row, col;
    double *cell_magnitudes;
    double *cell_angles;
} task;

#endif //DIPLOMSKI_HOG_COMMON_H
