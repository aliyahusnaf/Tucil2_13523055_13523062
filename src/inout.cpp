#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include <queue>
#include <sstream>
#include <FreeImage.h> 
#include "headers/json.hpp"
#include "headers/inout.h"

using json = nlohmann::json;
using namespace std;
int stepCounter = 0;

vector<vector<RGB>> loadImage(const string& filename, int& width, int& height) {

    FreeImage_Initialise();
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename.c_str(), 0);
    if (fif == FIF_UNKNOWN) {
        fif = FreeImage_GetFIFFromFilename(filename.c_str());
    }

    if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(fif)) {
        cerr << "Format file tidak didukung atau tidak bisa dibaca.\n";
        exit(1);
    }

FIBITMAP* bitmap = FreeImage_Load(fif, filename.c_str());
    FIBITMAP* rgb_bitmap = FreeImage_ConvertTo24Bits(bitmap);

    width = FreeImage_GetWidth(rgb_bitmap);
    height = FreeImage_GetHeight(rgb_bitmap);

    vector<vector<RGB>> image(height, vector<RGB>(width));

    for (int y = 0; y < height; ++y) { // baris
        BYTE* bits = FreeImage_GetScanLine(rgb_bitmap, height - 1 - y);  // FreeImage defaultnya terbalik (pertama di-scan = terakhir)
        for (int x = 0; x < width; ++x) { // kolom
            int b = bits[x * 3 + 0]; // FreeImage defaultnya BGR
            int g = bits[x * 3 + 1];
            int r = bits[x * 3 + 2];
            image[y][x] = { r, g, b }; // RGB (disesuaikan)
        }
    }

    FreeImage_Unload(rgb_bitmap);
    FreeImage_Unload(bitmap);

    FreeImage_DeInitialise();

    return image;
}

void fillImage(vector<vector<RGB>> &img, const Node* node) {
    if (node == nullptr) return;
    
    int x = node->x, y = node->y, width = node->width, height = node->height;
    
    if (node->isLeaf) {
        RGB color = node->avgColor;
        for (int i = y; i < y + height; i++) {
            for (int j = x; j < x + width; j++) {
                if (i < img.size() && j < img[0].size()) {
                    img[i][j] = color;
                }
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            if (node->children[i] != nullptr) {
                fillImage(img, node->children[i]);
            }
        }
    }
}

void saveImage(const vector<vector<RGB>>& img, const std::string& filename) {
    int width = img[0].size();
    int height = img.size();

    // Buat bitmap kosong
    FIBITMAP* bitmap = FreeImage_Allocate(width, height, 24);

    // Masukkan data piksel ke bitmap
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RGBQUAD color;
            color.rgbRed = img[height - 1 - y][x][0]; // Pembalikan posisi y
            color.rgbGreen = img[height - 1 - y][x][1]; // Pembalikan posisi y
            color.rgbBlue = img[height - 1 - y][x][2]; // Pembalikan posisi y

            FreeImage_SetPixelColor(bitmap, x, y, &color);
        }
    }

    // Simpan gambar ke file
    FreeImage_Save(FIF_PNG, bitmap, filename.c_str(), 0);

    // Hapus bitmap dari memori
    FreeImage_Unload(bitmap);
}


void fillImageWithDepthLimit(std::vector<std::vector<RGB>>& image, Node* node, int maxDepth, int currentDepth) {
    if (node == nullptr) return;
    
    if (node->isLeaf || currentDepth >= maxDepth) {
        RGB color = node->avgColor;
        for (int y = 0; y < node->height; y++) {
            for (int x = 0; x < node->width; x++) {
                if (node->y + y < image.size() && node->x + x < image[0].size()) {
                    image[node->y + y][node->x + x] = color;
                }
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            if (node->children[i] != nullptr) {
                fillImageWithDepthLimit(image, node->children[i], maxDepth, currentDepth + 1);
            }
        }
    }
}

void generateGifFromSteps(const std::string& frameDir, const std::string& gifPath) {

    std::string cmd = "magick -delay 50 -loop 0 " + frameDir + "/step_*.png " + gifPath;
    int result = system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Error creating GIF. Make sure ImageMagick is installed." << std::endl;
    }
}