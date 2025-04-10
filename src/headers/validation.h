#ifndef VALIDATION_H
#define VALIDATION_H

#include <string>

// Mengecek apakah path adalah absolut
bool isAbsolutePath(const std::string& path);

// Mengecek ekstensi file
bool isValidImageExtension(const std::string& path);
bool isValidGifExtension(const std::string& path);

// Mengecek apakah file eksis
bool fileExists(const std::string& path);

// Fungsi-fungsi input validasi
std::string getValidatedInputPath();
int getValidatedMethod();
int getValidatedMinSize();
std::string getValidatedOutputImagePath();
std::string getValidatedGifPath();
double getValidatedThreshold(int method);

#endif
