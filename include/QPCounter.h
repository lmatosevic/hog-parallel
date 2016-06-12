#ifndef DIPLOMSKI_HOG_QPCOUNTER_H
#define DIPLOMSKI_HOG_QPCOUNTER_H

#include <windows.h>

class QPCounter {
private:
    double PCFreq = 0.0;
    __int64 CounterStart = 0;
public:
    void StartCounter();
    double GetCounter();
};

#endif //DIPLOMSKI_HOG_QPCOUNTER_H
