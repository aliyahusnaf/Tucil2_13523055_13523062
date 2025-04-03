#include "headers/preprocessor.h"
#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include <queue>
#include <FreeImage.h> 
#include "headers/json.hpp"

using json = nlohmann::json;
using namespace std;

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

// Hitung variance dalam blok RGB
double calVariance(const vector<vector<RGB>>& img, int x, int y, int size) {
    long totR = 0, totG = 0, totB = 0;
    int N = size * size;

    for (int i = y; i < y + size; i++) {
        for (int j = x; j < x + size; j++) {
            totR += img[i][j][0];
            totG += img[i][j][1];
            totB += img[i][j][2];
        }
    }

    double avgR = totR / (double)N;
    double avgG = totG / (double)N;
    double avgB = totB / (double)N;

    double var = 0.0;

    for (int i = y; i < y + size; i++) {
        for (int j = x; j < x + size; j++) {
            var += pow(img[i][j][0] - avgR, 2);
            var += pow(img[i][j][1] - avgG, 2);
            var += pow(img[i][j][2] - avgB, 2);
        }
    }

    return var / N;
}

// Hitung MAD dalam blok RGB
double calMAD(const vector<vector<RGB>>& img, int x, int y, int size) {
    long totR = 0, totG = 0, totB = 0;
    int N = size * size;

    for (int i = y; i < y + size; i++) {
        for (int j = x; j < x + size; j++) {
            totR += img[i][j][0];
            totG += img[i][j][1];
            totB += img[i][j][2];
        }
    }

    double avgR = totR / (double)N;
    double avgG = totG / (double)N;
    double avgB = totB / (double)N;
    double mad = 0.0;

    for (int i = y; i < y + size; i++) {
        for (int j = x; j < x + size; j++) {
            mad += abs(img[i][j][0] - avgR);
            mad += abs(img[i][j][1] - avgG);
            mad += abs(img[i][j][2] - avgB);
        }
    }

    return mad / N;
}

// Hitung max diff dalam blok RGB
double calMaxDiff(const vector<vector<RGB>>& img, int x, int y, int size) {
    int minR = 255, minG = 255, minB = 255;
    int maxR = 0, maxG = 0, maxB = 0;

    for (int i = y; i < y + size; i++) {
        for (int j = x; j < x + size; j++) {
            minR = min(minR, img[i][j][0]);
            minG = min(minG, img[i][j][1]);
            minB = min(minB, img[i][j][2]);

            maxR = max(maxR, img[i][j][0]);
            maxG = max(maxG, img[i][j][1]);
            maxB = max(maxB, img[i][j][2]);
        }
    }

    return max({maxR - minR, maxG - minG, maxB - minB});
}

// Hitung entropy dalam blok RGB
// double calEntropy(const vector<vector<RGB>>& img, int x, int y, int size) {
//     map<int, int> histogram;
//     int N = size * size;

//     for (int i = y; i < y + size; i++) {
//         for (int j = x; j < x + size; j++) {
//             int intensity = (img[i][j][0] + img[i][j][1] + img[i][j][2]) / 3;
//             histogram[intensity]++;
//         }
//     }

//     double entropy = 0.0;
//     for (const auto& pair : histogram) {  
//         int intensity = pair.first;  // Key dari map
//         int count = pair.second;     // Value dari map
//         double p = count / (double)N;
//         entropy -= p * log2(p);
//     }

//     return entropy;
// }
double calEntropy(const vector<vector<RGB>>& img, int x, int y, int size) {
    map<int, int> histogram;
    int N = size * size;

    // Hitung histogram intensitas
    for (int i = y; i < y + size; i++) {
        for (int j = x; j < x + size; j++) {
            int intensity = round((img[i][j][0] + img[i][j][1] + img[i][j][2]) / 3.0);  // Pakai floating-point division
            histogram[intensity]++;
        }
    }

    double entropy = 0.0;
    for (const auto& pair : histogram) {
        double p = (double)pair.second / N; // Pastikan ini `double`
        if (p > 0) { 
            entropy -= p * log2(p);
        }
    }

    // Debug: Cek hasil entropy sebelum thresholding
    std::cout << "Entropy at (" << x << "," << y << ") size " << size << " = " << entropy << std::endl;

    return entropy;
}


// Hitung rata-rata RGB dari node leaf buat normalisasi
RGB avgColor(const vector<vector<RGB>>& img, int x, int y, int size) {
    double sum[3] = {0};
    int N = size * size;

    for (int i = y; i < y + size; ++i)
        for (int j = x; j < x + size; ++j)
            for (int c = 0; c < 3; ++c)
                sum[c] += img[i][j][c];

    RGB avg; // std::array<int, 3>;
    for (int c = 0; c < 3; ++c)
        avg[c] = (sum[c] / N);
    return avg;
}

// Bangun Quadtree dan simpan ke array of Node
void buildQuadtree(const vector<vector<RGB>>& image, int x, int y, int size, double threshold, int min_size, vector<Node>& tree, int errorMethod) {

    double var;

    // Pilih metode error yang digunakan
    if (errorMethod == 1 ) {
        var = calVariance(image, x, y, size);
    } else if (errorMethod == 2) {
        var = calMAD(image, x, y, size);
    } else if (errorMethod == 3) {
        var = calMaxDiff(image, x, y, size);
    } else if (errorMethod == 4) {
        var = calEntropy(image, x, y, size);
    } else {
        cerr << "Error: Metode " << errorMethod << " tidak dikenal!" << endl;
        return;
    }
    // if (size <= min_size || var < threshold) {
    //     Node n;
    //     n.x = x;
    //     n.y = y;
    //     n.size = size;
    //     n.isLeaf = true;
    //     n.color[0] = 0;  // Misalnya set warna default hitam
    //     n.color[1] = 0;
    //     n.color[2] = 0;  // Tentukan warna berdasarkan perhitungan rata-rata atau entropy
    //     std::fill(std::begin(n.children), std::end(n.children), -1);  // Tidak ada children untuk leaf
    //     tree.push_back(n);
    // } else {
    //     // Jika perlu melakukan rekursi untuk subdivisi lebih lanjut
    //     int halfSize = size / 2;
    //     int childTL = tree.size();
    //     buildQuadtree(image, x, y, halfSize, threshold, min_size, tree, errorMethod);  // Top Left
    //     int childTR = tree.size();
    //     buildQuadtree(image, x + halfSize, y, halfSize, threshold, min_size, tree, errorMethod);  // Top Right
    //     int childBL = tree.size();
    //     buildQuadtree(image, x, y + halfSize, halfSize, threshold, min_size, tree, errorMethod);  // Bottom Left
    //     int childBR = tree.size();
    //     buildQuadtree(image, x + halfSize, y + halfSize, halfSize, threshold, min_size, tree, errorMethod);  // Bottom Right
        
    //     // Setelah rekursi, buat node non-leaf dan set children
    //     Node n;
    //     n.x = x;
    //     n.y = y;
    //     n.size = size;
    //     n.isLeaf = false;
    //     n.color[0] = 0;  // Misalnya set warna default hitam
    //     n.color[1] = 0;
    //     n.color[2] = 0;  // Tentukan warna berdasarkan perhitungan rata-rata atau entropy
    //     n.children[0] = childTL;
    //     n.children[1] = childTR;
    //     n.children[2] = childBL;
    //     n.children[3] = childBR;
        
    //     tree.push_back(n);
    // }

    Node node;
    node.x = x;
    node.y = y;
    node.size = size; 
    node.isLeaf = (var <= threshold || size <= min_size); // jadiin leaf kalau udh masuk threshold atau udh terlalu kecil

    for (int i = 0; i < 4; ++i){
        node.children[i] = -1;  // inisialisasi 4 leaf awal
    }

    if (node.isLeaf) {  // normalisasi RGB kalau leaf
        RGB avg = avgColor(image, x, y, size);
        for (int i = 0; i < 3; ++i)
            node.color[i] = avg[i];
    }

    int curr_index = tree.size();
    tree.push_back(node);

    if (!node.isLeaf) {
        int half = size / 2; // 8x8 jadi 4x4
        int childIndex[4];

        // Simpan indeks setelah rekursi untuk memastikan benar
        childIndex[0] = tree.size();
        buildQuadtree(image, x, y, half, threshold, min_size, tree, errorMethod);              // TL
        
        childIndex[1] = tree.size();
        buildQuadtree(image, x + half, y, half, threshold, min_size, tree, errorMethod);       // TR
        
        childIndex[2] = tree.size();
        buildQuadtree(image, x, y + half, half, threshold, min_size, tree, errorMethod);       // BL
        
        childIndex[3] = tree.size();
        buildQuadtree(image, x + half, y + half, half, threshold, min_size, tree, errorMethod); // BR

        // Update indeks anak dengan benar setelah semua rekursi selesai
        for (int i = 0; i < 4; ++i)
            tree[curr_index].children[i] = childIndex[i];
    }
}

void save_to_json(const vector<Node>& tree, const string& filename) {
    json j;
    j["nodes"] = json::array();

    for (const Node& node : tree) {
        json node_json;
        node_json["x"] = node.x;
        node_json["y"] = node.y;
        node_json["size"] = node.size;
        node_json["isLeaf"] = node.isLeaf;

        if (node.isLeaf) {
            node_json["color"] = { node.color[0], node.color[1], node.color[2] };
        } else {
            node_json["children"] = { node.children[0], node.children[1], node.children[2], node.children[3] };
        }

        j["nodes"].push_back(node_json);
    }

    ofstream out(filename);
    out << j.dump(2);
    out.close();
}

int getDepth(const vector<Node>& tree, int index = 0) {
    if (tree[index].isLeaf) return 1;

    int maxDepth = 0;
    for (int i = 0; i < 4; ++i) {
        int child = tree[index].children[i];
        if (child != -1) {
            maxDepth = max(maxDepth, getDepth(tree, child)); // bandingin depth antar anak
        }
    }
    return 1 + maxDepth;
}

void fillImage(vector<vector<RGB>> &img, const vector<Node> &nodes, int nodeIdx) {
    if (nodeIdx == -1) return; // Tidak ada node

    const Node &node = nodes[nodeIdx]; // Ambil node berdasarkan indeks
    int x = node.x, y = node.y, size = node.size;

    if (node.isLeaf) {
        RGB color = {node.color[0], node.color[1], node.color[2]};
        for (int i = y; i < min(y + size, (int)img.size()); i++) {
            for (int j = x; j < min(x + size, (int)img[0].size()); j++) {
                img[i][j] = color; // Pastikan setiap piksel benar-benar diisi
            }
        }
    } else {
        // Rekursi untuk setiap anak node
        for (int i = 0; i < 4; i++) {
            if (node.children[i] != -1) {
                fillImage(img, nodes, node.children[i]);
            }
        }
    }
}

vector<vector<RGB>> quadtreeToImage(const vector<Node> &nodes, int rootIdx, int width, int height) {
    vector<vector<RGB>> img(height, vector<RGB>(width, {0, 0, 0}));
    fillImage(img, nodes, rootIdx);
    return img;
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
            color.rgbRed = img[y][x][0];
            color.rgbGreen = img[y][x][1];
            color.rgbBlue = img[y][x][2];

            FreeImage_SetPixelColor(bitmap, x, y, &color);
        }
    }

    // Simpan gambar ke file
    FreeImage_Save(FIF_PNG, bitmap, filename.c_str(), 0);

    // Hapus bitmap dari memori
    FreeImage_Unload(bitmap);
}