#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include <queue>
#include <sstream>
#include <vector>
#include <chrono> // Untuk menghitung waktu eksekusi
#include <FreeImage.h>
#include "headers/inout.h"
#include "headers/quadtree.h"
#include "headers/errorcounter.h"
using namespace std;

// Fungsi untuk menghitung ukuran file dalam byte
long long getFileSize(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    return file.is_open() ? static_cast<long long>(file.tellg()) : -1;// -1 jika gagal membaca file
}

int main() {
    string image_path, output_image_path, output_gif_path;
    int method, min_size;
    double threshold;
    
    cout << "Masukkan path gambar input absolut: ";
    //C:/COOLYEAH/SMT_4/STIMA/Tubes2/Tucil2_13523055_13523062/test/input/test1.png
    cin >> image_path;
    cout << "1. Variance: 0 - 65025 \n2. MAD: 0 - 255 \n3. MaxDiff: 0 - 255 \n4. Entropy: 0 - 8 \n5. SSIM: 0 - 1 \n";
    cout << "Pilih metode error (1,2,3,4,5): ";
    cin >> method;
    cout << "Masukkan threshold: ";
    cin >> threshold;
    cout << "Masukkan ukuran blok minimum: ";
    cin >> min_size;
    cout << "Masukkan path output gambar hasil: ";
    cin >> output_image_path;
    cout << "Masukkan path output GIF proses (kosongkan jika tidak ingin GIF): ";
    cin.ignore();
    getline(cin, output_gif_path);
    
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
    
    ifstream inOriginal(image_path, ios::binary | ios::ate);
    ifstream inCompressed(output_image_path, ios::binary | ios::ate);
    auto sizeBefore = inOriginal.tellg();
    auto sizeAfter = inCompressed.tellg();
    double compressionRatio = 1.0 - (double)sizeAfter / sizeBefore;
    
    cout << "Ukuran gambar sebelum: " << sizeBefore << " bytes\n";
    cout << "Ukuran gambar setelah: " << sizeAfter << " bytes\n";
    cout << "Persentase kompresi: " << (compressionRatio * 100) << "%\n";
    
    int depth = getDepth(root);
    int nodeCount = countNodes(root);
    
    cout << "Kedalaman pohon: " << depth << endl;
    cout << "Banyak simpul: " << nodeCount << endl;
    
    if (gifMode) {
        // system(("rm -rf " + stepFrameDir).c_str());
        // system(("mkdir -p " + stepFrameDir).c_str());
        system(("rmdir /S /Q " + stepFrameDir).c_str()); // hapus folder steps
        system(("mkdir " + stepFrameDir).c_str());      // buat ulang folder steps
        
        generateGifFrames(image, root, stepFrameDir);
        
        generateGif(stepFrameDir, output_gif_path);
        cout << "GIF proses kompresi disimpan ke " << output_gif_path << endl;
    }
    
    return 0;
}
