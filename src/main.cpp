#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include <queue>
#include <sstream>
#include <vector>
#include <chrono> 
#include <FreeImage.h>
#include "headers/inout.h"
#include "headers/quadtree.h"
#include "headers/errorcounter.h"
#include "headers/validation.h"

using namespace std;

long long getFileSize(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    return file.is_open() ? static_cast<long long>(file.tellg()) : -1; 
}

int main() {
    string image_path = getValidatedInputPath();
    
    cout << "1. Variance: 0 - 65025 \n"
         << "2. MAD: 0 - 255 \n"
         << "3. MaxDiff: 0 - 255 \n"
         << "4. Entropy: 0 - 8 \n"
         << "5. SSIM: 0 - 1 \n";
    
    int method = getValidatedMethod();
    double threshold = getValidatedThreshold(method);
    int min_size = getValidatedMinSize();

    string output_image_path = getValidatedOutputImagePath();
    string output_gif_path = getValidatedGifPath();

    // Muat gambar
    int width, height;
    auto image = loadImage(image_path, width, height);
    if (image.empty()) {
        cerr << "Gagal memuat gambar!" << endl;
        return 1;
    }

    cout << "Gambar dimuat: " << width << "x" << height << endl;

    auto start = chrono::high_resolution_clock::now();

    Node* root = createNode(0, 0, width, height);
    string stepFrameDir = "steps";
    bool gifMode = !output_gif_path.empty();

    buildQuadtree(image, root, threshold, min_size, method, false);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    vector<vector<RGB>> reconstructedImage(height, vector<RGB>(width));
    fillImage(reconstructedImage, root);
    saveImage(reconstructedImage, output_image_path);

    cout << "Waktu eksekusi: " << duration.count() << " detik\n";

    auto sizeBefore = getFileSize(image_path);
    auto sizeAfter = getFileSize(output_image_path);
    double compressionRatio = 1.0 - (double)sizeAfter / sizeBefore;

    cout << "Ukuran gambar sebelum: " << sizeBefore << " bytes\n";
    cout << "Ukuran gambar setelah: " << sizeAfter << " bytes\n";
    cout << "Persentase kompresi: " << (compressionRatio * 100) << "%\n";

    int depth = getDepth(root);
    int nodeCount = countNodes(root);

    cout << "Kedalaman pohon: " << depth << endl;
    cout << "Banyak simpul: " << nodeCount << endl;

    if (gifMode) {
        #ifdef _WIN32
            system(("rmdir /S /Q " + stepFrameDir).c_str());
            system(("mkdir " + stepFrameDir).c_str());
        #else
            system(("rm -rf " + stepFrameDir).c_str());
            system(("mkdir -p " + stepFrameDir).c_str());
        #endif
    
        generateGifFrames(image, root, stepFrameDir);
        generateGif(stepFrameDir, output_gif_path);
        cout << "GIF proses kompresi disimpan ke " << output_gif_path << endl;

        #ifdef _WIN32
            system(("rmdir /S /Q " + stepFrameDir).c_str());
        #else
            system(("rm -rf " + stepFrameDir).c_str());
        #endif
    }

    return 0;
}