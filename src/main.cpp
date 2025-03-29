#include "headers/preprocessor.h"
#include <iostream>
using namespace std;

int main() {
    string image_path = "/Users/aliyahusnafayyaza/Downloads/km.jpeg"; 
    int width, height;

    auto image = loadImage(image_path, width, height);
    cout << "Gambar dimuat: " << width << "x" << height << endl;

    vector<Node> tree;
    int threshold = 500;    
    int min_size = 1;        
    buildQuadtree(image, 0, 0, width, threshold, min_size, tree);

    cout << "Quadtree terbentuk: " << tree.size() << " simpul\n";
    cout << "Kedalaman pohon: " << getDepth(tree, 0) << endl;
    save_to_json(tree, "output.json");
    cout << "Hasil disimpan ke output.json\n";

    return 0;
}
