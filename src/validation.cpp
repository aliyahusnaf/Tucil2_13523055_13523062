#include "headers/validation.h"
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>

using namespace std;

bool isAbsolutePath(const string& path) {
#ifdef _WIN32
    return path.length() > 2 && path[1] == ':' && (path[2] == '\\' || path[2] == '/');
#else
    return !path.empty() && path[0] == '/';
#endif
}

string getExtension(const string& path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos == string::npos) return "";
    string ext = path.substr(dotPos);
    // ubah ke huruf kecil semua
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

bool isValidImageExtension(const string& path) {
    set<string> validExt = { ".png", ".jpg", ".jpeg", ".bmp" };
    return validExt.count(getExtension(path)) > 0;
}

bool isValidGifExtension(const string& path) {
    return getExtension(path) == ".gif";
}

bool fileExists(const string& path) {
    ifstream f(path.c_str());
    return f.good();
}

string getValidatedInputPath() {
    string path;
    while (true) {
        cout << "Masukkan path gambar input absolut: ";
        cin >> path;
        if (!isAbsolutePath(path)) {
            cout << "Path harus absolut.\n";
            continue;
        }
        if (!fileExists(path)) {
            cout << "File tidak ditemukan.\n";
            continue;
        }
        if (!isValidImageExtension(path)) {
            cout << "Jenis file tidak valid untuk gambar input.\n";
            continue;
        }
        return path;
    }
}

int getValidatedMethod() {
    int method;
    while (true) {
        cout << "Pilih metode error (1. Variance, 2. MAD, 3. MaxDiff, 4. Entropy, 5. SSIM): ";
        cin >> method;
        if (cin.fail() || method < 1 || method > 5) {
            cin.clear(); // Reset error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Buang input yang salah
            cout << "Pilihan tidak valid. Harap masukkan angka antara 1 hingga 5.\n";
        } else {
            return method;
        }
    }
}

int getValidatedMinSize() {
    int minSize;
    while (true) {
        cout << "Masukkan ukuran blok minimum (positif): ";
        cin >> minSize;
        if (cin.fail() || minSize <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ukuran blok minimum harus berupa bilangan bulat positif.\n";
        } else {
            return minSize;
        }
    }
}


string getValidatedOutputImagePath() {
    string path;
    while (true) {
        cout << "Masukkan path output gambar hasil: ";
        cin >> path;
        if (!isAbsolutePath(path)) {
            cout << "Path harus absolut.\n";
            continue;
        }
        if (!isValidImageExtension(path)) {
            cout << "Jenis file tidak valid untuk gambar output.\n";
            continue;
        }
        return path;
    }
}

string getValidatedGifPath() {
    string path;
    cout << "Masukkan path output GIF proses (kosongkan jika tidak ingin GIF): ";
    cin.ignore();
    getline(cin, path);
    if (path.empty()) return path;

    while (!isAbsolutePath(path) || !isValidGifExtension(path)) {
        if (!isAbsolutePath(path)) {
            cout << "Path harus absolut.\n";
        } else if (!isValidGifExtension(path)) {
            cout << "Ekstensi GIF harus .gif\n";
        }
        cout << "Masukkan ulang path output GIF (atau kosongkan): ";
        getline(cin, path);
        if (path.empty()) break;
    }

    return path;
}

double getValidatedThreshold(int method) {
    double threshold;

    while (true) {
        cout << "Masukkan threshold (>= 0): ";
        cin >> threshold;
        if (threshold < 0) {
            cout << "Threshold di luar range.\n";
        } else {
            return threshold;
        }
    }
}
