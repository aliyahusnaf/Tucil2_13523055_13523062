#include "headers/preprocessor.h"
#include <iostream>
using namespace std;

int main() {
    string image_path = //"C:/Users/ASUS/Downloads/S__4022279.jpg";
    "C:/Users/ASUS/Downloads/test.jpeg"; 
    int width, height;
    int threshold;
    int min_size;
    int method;

    cout << "Masukkan threshold: ";
    cin >> threshold;

    cout << "Masukkan ukuran minimum: ";
    cin >> min_size;

    cout << "Metode Error : \n1. Variance \n2. MAD \n3. MaxDiff \n4. Entropy\n";
    cout << "Pilih metode error (1,2,3,4): ";
    cin >> method;

    auto image = loadImage(image_path, width, height);
    cout << "Gambar dimuat: " << width << "x" << height << endl;
    if (image.empty()) {
        cerr << "Gagal memuat gambar!" << endl;
        return 1;
    }

    vector<Node> tree;
    // int threshold = 100;    
    // int min_size = 1;        
    buildQuadtree(image, 0, 0, width, threshold, min_size, tree, method); // variance, mad, maxdiff, entropy

    cout << "Quadtree terbentuk: " << tree.size() << " simpul\n";
    cout << "Kedalaman pohon: " << getDepth(tree, 0) << endl;
    save_to_json(tree, "output.json");
    cout << "Hasil disimpan ke output.json\n";
     // Konversi Quadtree ke gambar
    vector<vector<RGB>> reconstructedImage = quadtreeToImage(tree, 0, width, height);
    saveImage(reconstructedImage, "output.png");
    cout << "Gambar hasil disimpan ke output.png" << endl;


    return 0;
}
