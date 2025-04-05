#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include <queue>
#include <FreeImage.h> 
#include <sstream>
#include "headers/json.hpp"
#include "headers/quadtree.h"
#include "headers/errorcounter.h"
#include <functional> 

using json = nlohmann::json;
using namespace std;

Node* rootNodeGlobal = nullptr;

Node* createNode(int x, int y, int width, int height) {
    Node* node = new Node;
    node->x = x;
    node->y = y;
    node->width = width;
    node->height = height;
    node->isLeaf = true;
    
    // Inisialisasi pointer anak dengan nullptr
    for (int i = 0; i < 4; ++i) {
        node->children[i] = nullptr;
    }
    
    return node;
}

// Hitung rata-rata RGB dari node leaf buat normalisasi
RGB avgColor(const vector<vector<RGB>>& img, int x, int y, int width, int height) {
    double sum[3] = {0};
    int N = width * height;

    for (int i = y; i < y + height && i < img.size(); ++i) {
        for (int j = x; j < x + width && j < img[0].size(); ++j) {
            for (int c = 0; c < 3; ++c) {
                sum[c] += img[i][j][c];
            }
        }
    }

    RGB avg = {0, 0, 0};
    if (N > 0) {
        for (int c = 0; c < 3; ++c) {
            avg[c] = sum[c] / N;
        }
    }
    return avg;
}

// Bangun Quadtree dan simpan ke array of Node
void buildQuadtree(const vector<vector<RGB>>& image, Node* node, double threshold, int min_size,
                   int errorMethod, bool createGIF) {
    if (node == nullptr) return;

    int x = node->x;
    int y = node->y;
    int width = node->width;
    int height = node->height;
    node->avgColor = avgColor(image, x, y, width, height);

    double var;
    if (errorMethod == 1) {
        var = calVariance(image, x, y, width, height);
    } else if (errorMethod == 2) {
        var = calMAD(image, x, y, width, height);
    } else if (errorMethod == 3) {
        var = calMaxDiff(image, x, y, width, height);
    } else if (errorMethod == 4) {
        var = calEntropy(image, x, y, width, height);
    } else {
        cerr << "Error: Metode " << errorMethod << " tidak dikenal!" << endl;
        return;
    }

    bool isLeaf = (var <= threshold || width <= min_size || height <= min_size);
    node->isLeaf = isLeaf;

    if (isLeaf) {
        node->avgColor = avgColor(image, x, y, width, height);
    } else {
        int halfWidth = width / 2;
        int halfHeight = height / 2;

        if (halfWidth < min_size || halfHeight < min_size) {
            node->isLeaf = true;
            return;
        }

        node->children[0] = createNode(x, y, halfWidth, halfHeight); // TL
        node->children[1] = createNode(x + halfWidth, y, width - halfWidth, halfHeight); // TR
        node->children[2] = createNode(x, y + halfHeight, halfWidth, height - halfHeight); // BL
        node->children[3] = createNode(x + halfWidth, y + halfHeight, width - halfWidth, height - halfHeight); // BR

        for (int i = 0; i < 4; ++i) {
            buildQuadtree(image, node->children[i], threshold, min_size, errorMethod, createGIF);
        }
    }
}

void save_to_json(const Node* root, const string& filename) {
    std::function<void(const Node*, json&)> writeNodeToJson = [&](const Node* node, json& jsonArray) {
        if (node == nullptr) return;
        
        json node_json;
        node_json["x"] = node->x;
        node_json["y"] = node->y;
        node_json["width"] = node->width;
        node_json["height"] = node->height;
        node_json["isLeaf"] = node->isLeaf;
        
        if (node->isLeaf) {
            node_json["color"] = { node->avgColor[0], node->avgColor[1], node->avgColor[2] };
        } else {
            int childIndices[4] = {-1, -1, -1, -1};
            for (int i = 0; i < 4; ++i) {
                if (node->children[i] != nullptr) {
                    childIndices[i] = jsonArray.size() + 1;
                }
            }
            node_json["children"] = { childIndices[0], childIndices[1], childIndices[2], childIndices[3] };
        }

        jsonArray.push_back(node_json);
    
        for (int i = 0; i < 4; ++i) {
            if (node->children[i] != nullptr) {
                writeNodeToJson(node->children[i], jsonArray);
            }
        }
    };
    
    json j;
    j["nodes"] = json::array();

    writeNodeToJson(root, j["nodes"]);
    
    ofstream out(filename);
    out << j.dump(2);
    out.close();
}

int getDepth(Node* node) {
    if (node == nullptr) return 0;
    if (node->isLeaf) return 1;
    
    int maxDepth = 0;
    for (int i = 0; i < 4; ++i) {
        maxDepth = max(maxDepth, getDepth(node->children[i]));
    }
    return 1 + maxDepth;
}

int countNodes(Node* node) {
    if (!node) return 0;
    int count = 1;
    for (int i = 0; i < 4; ++i) {
        count += countNodes(node->children[i]);
    }
    return count;
}

void generateGifFrames(const std::vector<std::vector<RGB>>& originalImage, Node* root, const std::string& frameDir) {
   
    int maxDepth = getDepth(root);
    
    for (int depth = 0; depth <= maxDepth; depth++) {
       
        std::vector<std::vector<RGB>> frame(originalImage.size(), std::vector<RGB>(originalImage[0].size()));
        
        fillImageWithDepthLimit(frame, root, depth);
        
        std::stringstream ss;
        ss << frameDir << "/step_" << std::setw(4) << std::setfill('0') << depth << ".png";
        saveImage(frame, ss.str());
    }
}