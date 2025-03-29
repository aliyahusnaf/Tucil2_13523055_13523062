#include "headers/preprocessor.h"
#include <iostream>
#include <cmath>
#include <fstream>
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
void buildQuadtree(const vector<vector<RGB>>& image, int x, int y, int size, double threshold, int min_size, vector<Node>& tree) {
    double var = calVariance(image, x, y, size);

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
        // set x,y tetep jd kiri atas
        buildQuadtree(image, x, y, half, threshold, min_size, tree);              // TL
        buildQuadtree(image, x + half, y, half, threshold, min_size, tree);       // TR
        buildQuadtree(image, x, y + half, half, threshold, min_size, tree);       // BL
        buildQuadtree(image, x + half, y + half, half, threshold, min_size, tree); // BR

        // tree[curr_index] = parent, dibikinin anak lg karena masih di atas threshold/besar 
        for (int i = 0; i < 4; ++i)
            tree[curr_index].children[i] = curr_index + i + 1;
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
