#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include <queue>
#include <FreeImage.h> 
#include "headers/json.hpp"
#include "headers/inout.h"
#include "headers/errorcounter.h"

using json = nlohmann::json;
using namespace std;

// Hitung variance dalam blok RGB
double calVariance(const vector<vector<RGB>>& img, int x, int y, int width, int height) {
   long totR = 0, totG = 0, totB = 0;
   int N = width * height;
   
   for (int i = y; i < y + height; i++) {
       for (int j = x; j < x + width; j++) {
           totR += img[i][j][0];
           totG += img[i][j][1];
           totB += img[i][j][2];
       }
   }
   
   double avgR = totR / (double)N;
   double avgG = totG / (double)N;
   double avgB = totB / (double)N;
   
   double var = 0.0;
   for (int i = y; i < y + height; i++) {
       for (int j = x; j < x + width; j++) {
           var += pow(img[i][j][0] - avgR, 2);
           var += pow(img[i][j][1] - avgG, 2);
           var += pow(img[i][j][2] - avgB, 2);
       }
   }
   
   return var / N;
}

// Hitung MAD dalam blok RGB
double calMAD(const vector<vector<RGB>>& img, int x, int y, int width, int height) {
    long totR = 0, totG = 0, totB = 0;
    int N = width * height;
    
    for (int i = y; i < y + height && i < img.size(); i++) {
        for (int j = x; j < x + width && j < img[0].size(); j++) {
            totR += img[i][j][0];
            totG += img[i][j][1];
            totB += img[i][j][2];
        }
    }
    
    double avgR = totR / (double)N;
    double avgG = totG / (double)N;
    double avgB = totB / (double)N;
    
    double mad = 0.0;
    for (int i = y; i < y + height && i < img.size(); i++) {
        for (int j = x; j < x + width && j < img[0].size(); j++) {
            mad += abs(img[i][j][0] - avgR);
            mad += abs(img[i][j][1] - avgG);
            mad += abs(img[i][j][2] - avgB);
        }
    }
    
    return mad / N;
}

// Hitung max diff dalam blok RGB
double calMaxDiff(const vector<vector<RGB>>& img, int x, int y, int width, int height) {
    int minR = 255, minG = 255, minB = 255;
    int maxR = 0, maxG = 0, maxB = 0;
    
    for (int i = y; i < y + height && i < img.size(); i++) {
        for (int j = x; j < x + width && j < img[0].size(); j++) {
            minR = min(minR, img[i][j][0]);
            minG = min(minG, img[i][j][1]);
            minB = min(minB, img[i][j][2]);
            maxR = max(maxR, img[i][j][0]);
            maxG = max(maxG, img[i][j][1]);
            maxB = max(maxB, img[i][j][2]);
        }
    }
    
    return (maxR - minR + maxG - minG + maxB - minB) / 3.0;
}

// Hitung entropy dalam blok RGB
double calEntropy(const vector<vector<RGB>>& img, int x, int y, int width, int height) {
    map<int, int> histogramR, histogramG, histogramB;
    int N = width * height;
    
    // Hitung histogram untuk setiap kanal warna
    for (int i = y; i < y + height && i < img.size(); i++) {
        for (int j = x; j < x + width && j < img[0].size(); j++) {
            histogramR[img[i][j][0]]++;
            histogramG[img[i][j][1]]++;
            histogramB[img[i][j][2]]++;
        }
    }
    
    // Hitung entropy untuk setiap kanal
    double entropyR = 0.0, entropyG = 0.0, entropyB = 0.0;
    
    for (const auto& pair : histogramR) {
        double p = (double)pair.second / N;
        if (p > 0) entropyR -= p * log2(p);
    }
    
    for (const auto& pair : histogramG) {
        double p = (double)pair.second / N;
        if (p > 0) entropyG -= p * log2(p);
    }
    
    for (const auto& pair : histogramB) {
        double p = (double)pair.second / N;
        if (p > 0) entropyB -= p * log2(p);
    }
    
    // Rata-rata entropy dari tiga kanal
    double entropy = (entropyR + entropyG + entropyB) / 3.0;

    return entropy;
}

double calSSIM(const vector<vector<RGB>>& imgRef, const vector<vector<RGB>>& imgPred, int x, int y, int width, int height) {
    if (imgRef.empty() || imgRef[0].empty()) {
        std::cout << "imgRef kosong! SSIM tidak bisa dihitung." << std::endl;
        return 0.0;
    }

    const double C1 = (0.01 * 255) * (0.01 * 255);
    const double C2 = (0.03 * 255) * (0.03 * 255);
    const double wR = 0.2989, wG = 0.5870, wB = 0.1140;
    int N = width * height;

    std::vector<double> sumRef(3, 0.0), sumPred(3, 0.0);
    std::vector<double> varRef(3, 0.0), varPred(3, 0.0), cov(3, 0.0);

    for (int i = 0; i < height && y + i < imgRef.size(); ++i) {
        for (int j = 0; j < width && x + j < imgRef[0].size(); ++j) {
            const RGB& ref = imgRef[y + i][x + j];
            const RGB& pred = imgPred[i][j];

            for (int c = 0; c < 3; ++c) {
                sumRef[c] += ref[c];
                sumPred[c] += pred[c];
            }
        }
    }

    std::vector<double> meanRef(3), meanPred(3);
    for (int c = 0; c < 3; ++c) {
        meanRef[c] = sumRef[c] / N;
        meanPred[c] = sumPred[c] / N;
    }

    for (int i = 0; i < height && y + i < imgRef.size(); ++i) {
        for (int j = 0; j < width && x + j < imgRef[0].size(); ++j) {
            const RGB& ref = imgRef[y + i][x + j];
            const RGB& pred = imgPred[i][j];

            for (int c = 0; c < 3; ++c) {
                double diffRef = ref[c] - meanRef[c];
                double diffPred = pred[c] - meanPred[c];

                varRef[c] += diffRef * diffRef;
                varPred[c] += diffPred * diffPred;
                cov[c] += diffRef * diffPred;
            }
        }
    }

    for (int c = 0; c < 3; ++c) {
        varRef[c] /= (N - 1);
        varPred[c] /= (N - 1);
        cov[c] /= (N - 1);
    }

    double ssim[3];
    for (int c = 0; c < 3; ++c) {
        ssim[c] = ((2 * meanRef[c] * meanPred[c] + C1) * (2 * cov[c] + C2)) /
                  ((meanRef[c] * meanRef[c] + meanPred[c] * meanPred[c] + C1) * (varRef[c] + varPred[c] + C2));
    }

    return wR * ssim[0] + wG * ssim[1] + wB * ssim[2];
}
