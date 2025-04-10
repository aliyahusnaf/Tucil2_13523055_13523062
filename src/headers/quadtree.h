#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <array>
#include <string>
#include "inout.h"

Node* createNode(int x, int y, int width, int height);

void buildQuadtree(const std::vector<std::vector<RGB>>& image, Node* node, Node* root, double threshold, int min_size,
                   int errorMethod, bool createGIF);

int getDepth(Node* node);

int getNodes(Node* node);

RGB avgColor(const std::vector<std::vector<RGB>>& img, int x, int y, int width, int size);

int countNodes(Node* node);

void generateGifFrames(const std::vector<std::vector<RGB>>& originalImage, Node* root, const std::string& frameDir);
#endif