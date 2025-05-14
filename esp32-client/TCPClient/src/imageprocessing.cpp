#include "imageprocessing.h"

int ImageProcessing::detectEdges(const uint8_t* image, int width, int height, uint8_t* output) {

    static const THRESHOLD = 100;

    static const int sobelX[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    static const int sobelY[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    int edgePixelCount = 0;

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = 0;
            int gy = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int idx = ((y + ky) * width + (x + kx)) * 3;
                    uint8_t gray = image[idx]; // Assuming grayscale image
                    gx += sobelX[ky + 1][kx + 1] * gray;
                    gy += sobelY[ky + 1][kx + 1] * gray;
                }
            }

            int magnitude = static_cast<int>(sqrt(gx * gx + gy * gy));
            magnitude = magnitude > 255 ? 255 : magnitude;

            if (magnitude > THRESHOLD) {
                edgePixelCount++;
            }
        }
    }

    return edgePixelCount;
}

bool ImageProcessing::isDoorOpen(int edgeCount) {
    return edgeCount < 50000;
}