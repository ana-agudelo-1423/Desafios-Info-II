
#include <fstream>
using namespace std;
#include <iostream>
#include <QCoreApplication>
#include <QImage>


unsigned char rotateRight(unsigned char value, int bits) {
    return (value >> bits) | (value << (8 - bits));
}

unsigned char rotateLeft(unsigned char value, int bits) {
    return (value << bits) | (value >> (8 - bits));
}

void xorImages(unsigned char* img1, unsigned char* img2, unsigned char* result, int totalBytes) {
    for (int i = 0; i < totalBytes; ++i) {
        result[i] = img1[i] ^ img2[i];
    }
}

void rotateImageRight(unsigned char* src, unsigned char* dest, int totalBytes, int bits) {
    for (int i = 0; i < totalBytes; ++i) {
        dest[i] = rotateRight(src[i], bits);
    }
}

void rotateImageLeft(unsigned char* src, unsigned char* dest, int totalBytes, int bits) {
    for (int i = 0; i < totalBytes; ++i) {
        dest[i] = rotateLeft(src[i], bits);
    }
}

bool verificarEnmascaramiento(unsigned char* imagen, unsigned char* mascara, unsigned int* resultado, int seed, int n_pixels) {
    for (int i = 0; i < n_pixels * 3; ++i) {
        int idxImagen = seed * 3 + i;
        if ((int(imagen[idxImagen]) + int(mascara[i])) % 256 != resultado[i]) {
            return false;
        }
    }
    return true;
}

// ----------------------------- FUNCIONES DE IMAGEN ----------------------------- //

unsigned char* loadPixels(QString input, int &width, int &height) {
    QImage imagen(input);
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << endl;
        return nullptr;
    }
    imagen = imagen.convertToFormat(QImage::Format_RGB888);
    width = imagen.width();
    height = imagen.height();
    int dataSize = width * height * 3;
    unsigned char* pixelData = new unsigned char[dataSize];
    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);
        unsigned char* dstLine = pixelData + y * width * 3;
        memcpy(dstLine, srcLine, width * 3);
    }
    return pixelData;
}

bool exportImage(unsigned char* pixelData, int width, int height, QString archivoSalida) {
    QImage outputImage(width, height, QImage::Format_RGB888);
    for (int y = 0; y < height; ++y) {
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }
    if (!outputImage.save(archivoSalida, "BMP")) {
        cout << "Error: No se pudo guardar la imagen BMP modificada.";
        return false;
    } else {
        cout << "Imagen BMP modificada guardada como " << archivoSalida.toStdString() << endl;
        return true;
    }
}

unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return nullptr;
    }
    archivo >> seed;
    int r, g, b;
    while (archivo >> r >> g >> b) {
        n_pixels++;
    }
    archivo.close();
    archivo.open(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error al reabrir el archivo." << endl;
        return nullptr;
    }
    unsigned int* RGB = new unsigned int[n_pixels * 3];
    archivo >> seed;
    for (int i = 0; i < n_pixels * 3; i += 3) {
        archivo >> r >> g >> b;
        RGB[i] = r;
        RGB[i + 1] = g;
        RGB[i + 2] = b;
    }
    archivo.close();
    return RGB;
}

// ----------------------------- MAIN ----------------------------- //

int main() {
    QString archivoEntrada = "I_D.bmp";
    QString archivoIM = "IM.bmp";
    QString archivoMascara = "M.bmp";

    int width = 0, height = 0;
    unsigned char* imagen = loadPixels(archivoEntrada, width, height);
    unsigned char* imgIM = loadPixels(archivoIM, width, height);
    unsigned char* mascara = loadPixels(archivoMascara, width, height);

    unsigned char* xorResult = new unsigned char[width * height * 3];
    xorImages(imagen, imgIM, xorResult, width * height * 3);
    exportImage(xorResult, width, height, "xor_output.bmp");

    unsigned char* rotada = new unsigned char[width * height * 3];
    rotateImageRight(imagen, rotada, width * height * 3, 3);
    exportImage(rotada, width, height, "rotada_output.bmp");

    int seed = 0, n_pixels = 0;
    unsigned int* resultado = loadSeedMasking("M1.txt", seed, n_pixels);
    bool valido = verificarEnmascaramiento(imagen, mascara, resultado, seed, n_pixels);
    cout << "\n¿El enmascaramiento es válido? " << (valido ? "Sí" : "No") << endl;

    delete[] imagen;
    delete[] imgIM;
    delete[] mascara;
    delete[] xorResult;
    delete[] rotada;
    delete[] resultado;

    return 0;
}
