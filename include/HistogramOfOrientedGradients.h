#ifndef DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H
#define DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H

class HistogramOfOrientedGradients {
public:
    int image;

    HistogramOfOrientedGradients(int image);

    void getDescriptor();

private:
    void getHistogram();
};
#endif //DIPLOMSKI_HOG_HISTOGRAMOFORIENTEDGRADIENTS_H
