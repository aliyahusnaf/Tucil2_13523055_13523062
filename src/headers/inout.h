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
    Node* root;
    std::vector<std::vector<RGB>> originalImage;  
};

std::vector<std::vector<RGB>> loadImage(const std::string& filename, int& width, int& height);

void fillImageUntilDepth(std::vector<std::vector<RGB>>& img, const Node* node, int currentDepth, int maxDepth);

void saveStepImage(const std::vector<std::vector<RGB>>& img, const std::string& folder = "steps");


void fillImage(std::vector<std::vector<RGB>> &img, const Node* node);

void saveImage(const std::vector<std::vector<RGB>>& img, const std::string& filename);


void fillImageWithDepthLimit(std::vector<std::vector<RGB>>& image, Node* node, int maxDepth, int currentDepth = 0);
void generateGifFrames(const std::vector<std::vector<RGB>>& originalImage, Node* root, const std::string& frameDir);


#endif
