#ifndef ERRORCOUNTER_H
#define ERRORCOUNTER_H

#include <vector>
#include <array>
#include <string>
#include "inout.h"
#include "quadtree.h"

double calVariance(const std::vector<std::vector<RGB>>& img, int x, int y, int width, int height);

double calMAD(const std::vector<std::vector<RGB>>& img, int x, int y, int width, int height);

double calMaxDiff(const std::vector<std::vector<RGB>>& img, int x, int y, int width, int height);

double calEntropy(const std::vector<std::vector<RGB>>& img, int x, int y, int width, int height);

#endif