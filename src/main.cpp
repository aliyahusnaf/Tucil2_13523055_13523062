#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include <queue>
#include <vector>
#include <FreeImage.h>
#include "headers/json.hpp"
#include "headers/inout.h"
#include "headers/quadtree.h"
#include "headers/errorcounter.h"

using json = nlohmann::json;
using namespace std;

int main() {
    string image_path = "/Users/aliyahusnafayyaza/Downloads/S__19775494.jpg";
    int width, height;
    double threshold;
    int min_size;
    int method;

    cout << "Masukkan threshold: ";
    cin >> threshold;
    cout << "Masukkan ukuran minimum: ";
    cin >> min_size;
    cout << "Metode Error : \n1. Variance \n2. MAD \n3. MaxDiff \n4. Entropy\n";
    cout << "Pilih metode error (1,2,3,4): ";
    cin >> method;

    // Muat gambar
    auto image = loadImage(image_path, width, height);
    cout << "Gambar dimuat: " << width << "x" << height << endl;

    if (image.empty()) {
        cerr << "Gagal memuat gambar!" << endl;
        return 1;
    }

    // Buat root node
    Node* root = createNode(0, 0, width, height);

    // Bangun quadtree mulai dari root
    buildQuadtree(image, root, threshold, min_size, method);
    cout << "Quadtree terbentuk\n";

    // Hitung kedalaman pohon
    int depth = getDepth(root);
    cout << "Kedalaman pohon: " << depth << endl;

    // Simpan ke JSON
    save_to_json(root, "output.json");
    cout << "Hasil disimpan ke output.json\n";

    // Rekonstruksi gambar
    vector<vector<RGB>> reconstructedImage(height, vector<RGB>(width));
    fillImage(reconstructedImage, root);
    saveImage(reconstructedImage, "output.png");
    cout << "Gambar hasil disimpan ke output.png" << endl;

    // Pembersihan memori (opsional, tergantung implementasi)
    // Anda mungkin perlu membuat fungsi untuk menghapus seluruh pohon

    return 0;
}