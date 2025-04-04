#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include <queue>
#include <vector>
#include <chrono> // Untuk menghitung waktu eksekusi
#include <FreeImage.h>
#include "headers/json.hpp"
#include "headers/inout.h"
#include "headers/quadtree.h"
#include "headers/errorcounter.h"

using json = nlohmann::json;
using namespace std;

// Fungsi untuk menghitung ukuran file dalam byte
long long getFileSize(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    return file.is_open() ? static_cast<long long>(file.tellg()) : -1;// -1 jika gagal membaca file
}

int main() {
    int width, height;
    double threshold;
    int min_size;
    int method;
    string image_path;
    string output_path;
    
    cout << "Masukkan alamat absolut gambar yang akan dikompresi: ";
    getline(cin, image_path);
    // Dapatkan ukuran file asli dalam byte
    long long original_file_size = getFileSize(image_path);

    // Muat gambar
    auto image = loadImage(image_path, width, height);
    if (image.empty()) {
        cerr << "Gagal memuat gambar!" << endl;
        return 1;
    }
    //C:\COOLYEAH\SMT_4\STIMA\Tubes2\Tucil2_13523055_13523062\test\input\test1.png
    cout << "Metode Error : \n1. Variance \n2. MAD \n3. MaxDiff \n4. Entropy \n5. SSIM \n";
    cout << "Pilih metode error (1,2,3,4): ";
    cin >> method;
    cout << "Range threshold: \n";
    cout << "1. Variance: 0 - 65025 \n2. MAD: 0 - 255 \n3. MaxDiff: 0 - 255 \n4. Entropy: 0 - 8 \n5. SSIM: -1 - 1 \n";
    cout << "Masukkan threshold: ";
    cin >> threshold;
    cout << "Masukkan ukuran minimum: ";
    cin >> min_size;
    cout << "Masukkan alamat absolut untuk menyimpan gambar hasil kompresi: ";
    cin >> output_path;

    // Mulai hitung waktu eksekusi
    auto start = std::chrono::high_resolution_clock::now();

    // Buat root node
    Node* root = createNode(0, 0, width, height);
    buildQuadtree(image, root, root, threshold, min_size, method);

    // Hitung kedalaman pohon
    int depth = getDepth(root);

    // Hitung jumlah simpul
    int nodes = getNodes(root);

    // Simpan ke JSON
    save_to_json(root, "test/output/output.json");

    // Rekonstruksi gambar
    vector<vector<RGB>> reconstructedImage(height, vector<RGB>(width));
    fillImage(reconstructedImage, root);
    saveImage(reconstructedImage, output_path);

    // Ambil ukuran gambar setelah kompresi
    int new_width, new_height;
    auto compressedImage = loadImage(output_path, new_width, new_height);

    // Dapatkan ukuran file hasil kompresi dalam byte
    long long compressed_file_size = getFileSize(output_path);

    // Hitung persentase kompresi berdasarkan ukuran file
    double compression_ratio = ((double)(original_file_size - compressed_file_size) / original_file_size) * 100.0;

    // Hitung waktu eksekusi
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // OUTPUT SESUAI URUTAN YANG DIMINTA
    cout << "Execution Time      : " << elapsed.count() << " seconds" << endl;
    cout << "Original Size       : " << width << " x " << height << " (" << original_file_size << " bytes)" << endl;
    cout << "Compressed Size     : " << new_width << " x " << new_height << " (" << compressed_file_size << " bytes)" << endl;
    cout << "Compression Ratio   : " << compression_ratio << "%" << endl;
    cout << "Tree Depth          : " << depth << endl;
    cout << "Total Nodes         : " << nodes << endl;
    cout << "Compressed Image    : " << output_path << endl;

    return 0;
}
