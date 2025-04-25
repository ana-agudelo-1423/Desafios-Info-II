#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>
#include <QFileInfo>

using namespace std;

// ----------------------------- OPERACIONES A NIVEL DE BITS ----------------------------- //

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

unsigned char* loadPixels(QString input, int &width, int &height) {
    QImage imagen(input);
    if (imagen.isNull()) {
        cerr << "Error: No se pudo cargar la imagen: " << input.toStdString() << endl;
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
        cerr << "Error: No se pudo guardar la imagen: " << archivoSalida.toStdString() << endl;
        return false;
    }
    cout << "Imagen guardada como " << archivoSalida.toStdString() << endl;
    return true;
}


struct MaskingResult {
    int seed;
    int pixelCount;
    unsigned int* RGB;
};

MaskingResult* loadSeedMasking(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        return nullptr;
    }

    MaskingResult* result = new MaskingResult;
    archivo >> result->seed;

    // Contar el número de píxeles
    result->pixelCount = 0;
    int r, g, b;
    while (archivo >> r >> g >> b) {
        result->pixelCount++;
    }

    // Volver a leer para almacenar los datos
    archivo.clear();
    archivo.seekg(0);
    archivo >> result->seed;

    result->RGB = new unsigned int[result->pixelCount * 3];
    for (int i = 0; i < result->pixelCount * 3; i += 3) {
        archivo >> r >> g >> b;
        result->RGB[i] = r;
        result->RGB[i+1] = g;
        result->RGB[i+2] = b;
    }

    archivo.close();
    return result;
}

bool verificarEnmascaramiento(unsigned char* imagen, unsigned char* mascara, MaskingResult* resultado) {
    for (int i = 0; i < resultado->pixelCount * 3; ++i) {
        int idxImagen = resultado->seed * 3 + i;
        if ((int(imagen[idxImagen]) + int(mascara[i])) % 256 != resultado->RGB[i]) {
            return false;
        }
    }
    return true;
}

// ----------------------------- RECONSTRUCCIÓN DE IMAGEN ----------------------------- //

bool revertTransformation(unsigned char* currentImage, unsigned char* IM, MaskingResult* masking,
                          int width, int height, int& transformationType) {
    // Intenta revertir XOR
    unsigned char* temp = new unsigned char[width * height * 3];
    xorImages(currentImage, IM, temp, width * height * 3);

    if (masking && verificarEnmascaramiento(temp, IM, masking)) {
        transformationType = 1; // XOR
        memcpy(currentImage, temp, width * height * 3);
        delete[] temp;
        return true;
    }

    // Intenta revertir rotación derecha 3 bits
    rotateImageLeft(currentImage, temp, width * height * 3, 3);
    if (masking && verificarEnmascaramiento(temp, IM, masking)) {
        transformationType = 2; // Rotación derecha
        memcpy(currentImage, temp, width * height * 3);
        delete[] temp;
        return true;
    }

    // Intenta revertir rotación izquierda 3 bits
    rotateImageRight(currentImage, temp, width * height * 3, 3);
    if (masking && verificarEnmascaramiento(temp, IM, masking)) {
        transformationType = 3; // Rotación izquierda
        memcpy(currentImage, temp, width * height * 3);
        delete[] temp;
        return true;
    }

    delete[] temp;
    return false;
}

// ----------------------------- MAIN ----------------------------- //

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Verificar archivos de entrada
    QString archivoEntrada = "I_O.bmp";
    QString archivoIM = "I_M.bmp";
    QString archivoMascara = "M.bmp";

    QFileInfo checkFile(archivoEntrada);
    if (!checkFile.exists()) {
        cerr << "Error: Archivo " << archivoEntrada.toStdString() << " no encontrado." << endl;
        return 1;
    }

    // Cargar imágenes
    int width = 0, height = 0;
    unsigned char* imagen = loadPixels(archivoEntrada, width, height);
    unsigned char* imgIM = loadPixels(archivoIM, width, height);
    unsigned char* mascara = loadPixels(archivoMascara, width, height);

    if (!imagen || !imgIM || !mascara) {
        cout << "Error al cargar una o mas imagenes." << endl;
        return 1;
    }

    // Cargar archivos de enmascaramiento
    MaskingResult* masking1 = loadSeedMasking("M1.txt");
    MaskingResult* masking2 = loadSeedMasking("M2.txt");

    if (!masking1 || !masking2) {
        cerr << "Error al cargar archivos de enmascaramiento." << endl;
        return 1;
    }

    // Proceso de reconstrucción
    unsigned char* currentImage = new unsigned char[width * height * 3];
    memcpy(currentImage, imagen, width * height * 3);

    int transformationType = 0;
    bool success = false;

    // Intentar revertir transformaciones en diferentes órdenes
    for (int attempt = 0; attempt < 2; ++attempt) {
        memcpy(currentImage, imagen, width * height * 3);

        // Primer paso de reversión
        if (revertTransformation(currentImage, imgIM, masking2, width, height, transformationType)) {
            cout << "Primera transformación revertida: " << transformationType << endl;

            // Segundo paso de reversión
            if (revertTransformation(currentImage, imgIM, masking1, width, height, transformationType)) {
                cout << "Segunda transformación revertida: " << transformationType << endl;
                success = true;
                break;
            }
        }

        // Intentar otro orden si el primero falló
        swap(masking1, masking2);
    }

    if (success) {
        exportImage(currentImage, width, height, "reconstructed.bmp");
        cout << "Reconstrucción exitosa!" << endl;
    } else {
        cerr << "No se pudo reconstruir la imagen original." << endl;
    }

    // Liberar memoria
    delete[] imagen;
    delete[] imgIM;
    delete[] mascara;
    delete[] currentImage;
    delete masking1;
    delete masking2;

    return success ? 0 : 1;
}
