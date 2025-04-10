# Tucil 2 Stima: Kompresi Gambar dengan Metode Quadtree

**Program kompresi gambar** ini adalah program yang melakukan kompresi gambar menggunakan struktur **Quadtree** dan pendekatan **Divide and Conquer**. Metode ini membagi gambar secara rekursif ke dalam kuadran berdasarkan _threshold error_ dan ukuran minimum, sehingga dapat mengurangi ukuran penyimpanan tanpa menghilangkan detail penting.

## Features
- Kompresi gambar menggunakan dekomposisi Quadtree.
- Dapat dilakukan menggunakan beberapa jenis metode perhitungan error (Variance, MAD, Max Pixel Difference, Entropy, dan SSIM).
- Dapat menerima input parameter: threshold dan ukuran minimum blok.
- Dapat menghasilkan GIF proses kompresi gambar.

## Project Structure
```
Tucil2_13523055_13523062/
├── bin/
├── doc/
├── library/
│   ├── include/
│   │   ├── FreeImage.h
│   ├── x32/
│   │   ├── FreeImage.dll
│   │   ├── FreeImage.lib
│   ├── x64/
│   │   ├── FreeImage.dll
│   │   ├── FreeImage.lib
├── src/
│   ├── headers/
│   │   ├── errorcounter.h
│   │   ├── inout.h
│   │   ├── quadtree.h
│   ├── errorcounter.cpp
│   ├── inout.cpp
│   ├── main.cpp
│   ├── quadtree.cpp
├── README.md
└── test/
```

## Instalasi dan Dependensi
### Linux
Pastikan sudah memiliki g++ (C++17 atau lebih tinggi) dan sudah menginstall library-library yang dibutuhkan:
```bash
sudo apt install g++ libfreeimage-dev imagemagick
```
### Windows
Pastikan sudah memiliki g++ (C++17 atau lebih tinggi) dan sudah menginstall library-library yang dibutuhkan dengan cara:
1. Buka tautan https://imagemagick.org/script/download.php#windows
2. Pilih "ImageMagick-7.x.x-Q16-HDRI-x64-dll.exe" sesuai dengan versi 64-bit laptop/PC
3. Install dengan centang opsi: ✅ Install legacy utilities (convert) dan ✅ Add application directory to your system PATH
Catatan: tidak usah menginstall library freeimage karena sudah di automatisasi melalui penggunaan file bat

### MacOS
Pastikan sudah memiliki g++ (C++17 atau lebih tinggi) dan sudah menginstall library-library yang dibutuhkan:
```bash
brew install freeimage imagemagick
```

## Kompilasi program
Clone repository ini lalu,
### Linux
Jalankan command ini pada terminal (sesuaikan versi c++):
```bash
cd src
g++ -std=c++17 main.cpp quadtree.cpp errorcounter.cpp inout.cpp -o main -lfreeimage
```
### MacOS
Jalankan command ini pada terminal (sesuaikan versi c++ dan direktori freeimage):
```bash
cd src
g++ -std=c++17 main.cpp quadtree.cpp errorcounter.cpp inout.cpp -o main \
    -I/opt/homebrew/include -L/opt/homebrew/lib -lfreeimage
```
### Windows
Jalankan command ini pada terminal (untuk 32 bit):
```bash
./build-x32.bat
```
Jalankan command ini pada terminal (untuk 64 bit):
```bash
./build-x64.bat
```

## Pemakaian
Program akan meminta input dari pengguna sebagai berikut:
1. **Path Gambar Input** - Path absolut menuju gambar yang akan dikompresi.
2. **Metode Perhitungan Error** - 1: Variance, 2: MAD, 3: Max Pixel Difference, 4: Entropy, 5: SSIM
3. **Threshold** - Menentukan batas error maksimum untuk membagi blok.
4. **Ukuran blok minimum** – Menentukan ukuran terkecil blok.
5. **Path gambar output** - Path absolut untuk menyimpan gambar hasil kompresi.
6. **Path GIF Output (Bonus)** – Path absolut visualisasi proses kompresi dalam format GIF.

## Pembuat program
| Name | NIM | Kelas |
|------|------|------|
| Muhammad Timur Kanigara | 13523055 | K01 |
| Aliya Husna Fayyaxa | 13523062 | K02 |
