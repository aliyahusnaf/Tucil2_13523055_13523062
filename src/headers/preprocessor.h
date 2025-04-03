#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <vector>
#include <array>
#include <string>

struct Node {
    int x, y, size;
    int color[3];
    bool isLeaf;
    int children[4]; // TL, TR, BL, BR
};

using RGB = std::array<int, 3>;

std::vector<std::vector<RGB>> loadImage(const std::string& filename, int& width, int& height);

void buildQuadtree(const std::vector<std::vector<RGB>>& image, int x, int y, int size, double threshold, int min_size, std::vector<Node>& tree, int errorMethod);

void save_to_json(const std::vector<Node>& tree, const std::string& filename);

int getDepth(const std::vector<Node>& tree, int index);

double calVariance(const std::vector<std::vector<RGB>>& img, int x, int y, int size);

double calMAD(const std::vector<std::vector<RGB>>& img, int x, int y, int size);

double calMaxDiff(const std::vector<std::vector<RGB>>& img, int x, int y, int size);

double calEntropy(const std::vector<std::vector<RGB>>& img, int x, int y, int size);

RGB avgColor(const std::vector<std::vector<RGB>>& img, int x, int y, int size);

void fillImage(std::vector<std::vector<RGB>> &img, const std::vector<Node> &nodes, int nodeIdx);

std::vector<std::vector<RGB>> quadtreeToImage(const std::vector<Node> &nodes, int rootIdx, int width, int height);

void saveImage(const std::vector<std::vector<RGB>>& img, const std::string& filename);

#endif
