#ifndef PREPROCESSING_H
#define PREPROCESSING_H

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

void buildQuadtree(const std::vector<std::vector<RGB>>& image, int x, int y, int size, double threshold, int min_size, std::vector<Node>& tree);

void save_to_json(const std::vector<Node>& tree, const std::string& filename);

int getDepth(const std::vector<Node>& tree, int index);


#endif
