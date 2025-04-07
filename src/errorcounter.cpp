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

double calSSIM(const vector<vector<RGB>>& imgRef, const vector<vector<RGB>>& img, int x, int y, int width, int height) {
    if (imgRef.empty() || imgRef[0].empty()) {
        std::cout << "imgRef kosong! SSIM tidak bisa dihitung." << std::endl;
        return 0.0; // Return default SSIM
    }
    const double C1 = (0.01 * 255) * (0.01 * 255);
    const double C2 = (0.03 * 255) * (0.03 * 255);
    const double wR = 0.2989, wG = 0.5870, wB = 0.1140;

    int N = width * height;

    std::vector<double> sumRef(3, 0.0), sumImg(3, 0.0);
    std::vector<double> varRef(3, 0.0), varImg(3, 0.0), cov(3, 0.0);

    for (int i = y; i < y + height && i < img.size(); i++) {
        for (int j = x; j < x + width && j < img[0].size(); j++) {
            sumRef[0] += imgRef[i][j][0];
            sumRef[1] += imgRef[i][j][1];
            sumRef[2] += imgRef[i][j][2];

            sumImg[0] += img[i][j][0];
            sumImg[1] += img[i][j][1];
            sumImg[2] += img[i][j][2];
        }
    }

    double meanRefR = sumRef[0] / (double)N;
    double meanRefG = sumRef[1] / (double)N;
    double meanRefB = sumRef[2] / (double)N;

    double meanImgR = sumImg[0] / (double)N;
    double meanImgG = sumImg[1] / (double)N;
    double meanImgB = sumImg[2] / (double)N;

    for (int i = y; i < y + height && i < img.size(); i++) {
        for (int j = x; j < x + width && j < img[0].size(); j++) {
            double diffRefR = imgRef[i][j][0] - meanRefR;
            double diffRefG = imgRef[i][j][1] - meanRefG;
            double diffRefB = imgRef[i][j][2] - meanRefB;

            double diffImgR = img[i][j][0] - meanImgR;
            double diffImgG = img[i][j][1] - meanImgG;
            double diffImgB = img[i][j][2] - meanImgB;

            varRef[0] += diffRefR * diffRefR;
            varRef[1] += diffRefG * diffRefG;
            varRef[2] += diffRefB * diffRefB;

            varImg[0] += diffImgR * diffImgR;
            varImg[1] += diffImgG * diffImgG;
            varImg[2] += diffImgB * diffImgB;

            cov[0] += diffRefR * diffImgR;
            cov[1] += diffRefG * diffImgG;
            cov[2] += diffRefB * diffImgB;
        }
    }

    varRef[0] /= N - 1;
    varRef[1] /= N - 1;
    varRef[2] /= N - 1;

    varImg[0] /= N - 1;
    varImg[1] /= N - 1;
    varImg[2] /= N - 1;

    cov[0] /= N - 1;
    cov[1] /= N - 1;
    cov[2] /= N - 1;

    double ssimR = ((2 * meanRefR * meanImgR + C1) * (2 * cov[0] + C2)) /
                   ((meanRefR * meanRefR + meanImgR * meanImgR + C1) * (varRef[0] + varImg[0] + C2));

    double ssimG = ((2 * meanRefG * meanImgG + C1) * (2 * cov[1] + C2)) /
                   ((meanRefG * meanRefG + meanImgG * meanImgG + C1) * (varRef[1] + varImg[1] + C2));

    double ssimB = ((2 * meanRefB * meanImgB + C1) * (2 * cov[2] + C2)) /
                   ((meanRefB * meanRefB + meanImgB * meanImgB + C1) * (varRef[2] + varImg[2] + C2));

    return wR * ssimR + wG * ssimG + wB * ssimB;
}