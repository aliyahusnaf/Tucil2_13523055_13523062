#ifndef INOUT_H
#define INOUT_H

#include <vector>
#include <array>
#include <string>

using RGB = std::array<int, 3>;

struct Node {
    int x, y;
    int width, height;
    RGB avgColor;
    bool isLeaf;
    Node* children[4];
};

std::vector<std::vector<RGB>> loadImage(const std::string& filename, int& width, int& height);

void fillImage(std::vector<std::vector<RGB>> &img, const Node* node);

void saveImage(const std::vector<std::vector<RGB>>& img, const std::string& filename);

#endif
