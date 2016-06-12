#ifndef DIPLOMSKI_HOG_COMMON_H
#define DIPLOMSKI_HOG_COMMON_H

#define TAG_NEW_TASK 1
#define TAG_NEW_TASK_1 2
#define TAG_NEW_TASK_2 3
#define TAG_REQUEST_FOR_TASK 4
#define TAG_RESULT 5
#define TAG_TASK_FINISHED 6
#define TAG_END 7

extern int my_rank;
extern int nproc;

extern bool debug;

typedef struct Task {
    int task_id;
    int process_id;
    int status;
    int row, col;
    double *cell_magnitudes;
    double *cell_angles;
} task;

#endif //DIPLOMSKI_HOG_COMMON_H
