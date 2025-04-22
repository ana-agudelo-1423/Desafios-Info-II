#include <QCoreApplication>
#include <QImage>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <iostream>
#include <cstring>
#include <cstdlib>

// Constantes
#define MAX_BITS 8

// --- Funciones de utilidad para operaciones bitwise ---
unsigned char xor_byte(unsigned char a, unsigned char b) {
    return a ^ b;
}

unsigned char rotate_right(unsigned char val, int n) {
    return (val >> n) | (val << (8 - n));
}

unsigned char rotate_left(unsigned char val, int n) {
    return (val << n) | (val >> (8 - n));
}

unsigned char shift_right(unsigned char val, int n) {
    return val >> n;
}

unsigned char shift_left(unsigned char val, int n) {
    return val << n;
}

// --- Cargar imagen BMP usando QImage ---
unsigned char* load_image(const char* path, int* width, int* height) {
    QImage image(path);
    if (image.isNull()) {
        qDebug() << "No se pudo cargar la imagen.";
        return nullptr;
    }
    *width = image.width();
    *height = image.height();
    image = image.convertToFormat(QImage::Format_RGB888);
    int size = (*width) * (*height) * 3;
    unsigned char* data = new unsigned char[size];
    int index = 0;
    for (int y = 0; y < *height; ++y) {
        for (int x = 0; x < *width; ++x) {
            QColor color(image.pixel(x, y));
            data[index++] = color.red();
            data[index++] = color.green();
            data[index++] = color.blue();
        }
    }
    return data;
}

// --- Guardar imagen BMP desde datos RGB ---
void save_image(const char* path, unsigned char* data, int width, int height) {
    QImage img(width, height, QImage::Format_RGB888);
    int index = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int r = data[index++];
            int g = data[index++];
            int b = data[index++];
            img.setPixel(x, y, qRgb(r, g, b));
        }
    }
    img.save(path);
}

// --- Cargar archivo de enmascaramiento ---
bool load_mask_result(const char* path, int* seed, unsigned char** result, int* length) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo de rastreo.";
        return false;
    }
    QTextStream in(&file);
    QString line = in.readLine();
    *seed = line.toInt();

    int capacity = 1024;
    *result = (unsigned char*)malloc(capacity);
    *length = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList vals = line.split(' ');
        for (int i = 0; i < vals.size(); ++i) {
            if (*length >= capacity) {
                capacity *= 2;
                *result = (unsigned char*)realloc(*result, capacity);
            }
            (*result)[(*length)++] = (unsigned char)vals[i].toInt();
        }
    }
    file.close();
    return true;
}

// --- Verificar enmascaramiento ---
bool verify_mask(unsigned char* img, unsigned char* mask, unsigned char* result, int seed, int mask_size) {
    for (int k = 0; k < mask_size; ++k) {
        if ((int)img[seed + k] + (int)mask[k] != (int)result[k]) {
            return false;
        }
    }
    return true;
}

// --- Punto de entrada ---
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    int width = 0, height = 0;

    qDebug() << "Ruta actual:" << QDir::currentPath();

    // Cargar imágenes con los nombres correctos
    unsigned char* id = load_image("I_D.bmp", &width, &height);
    unsigned char* im = load_image("I_M.bmp", &width, &height);
    unsigned char* mask = load_image("M.bmp", &width, &height); // Asegúrate de que esta imagen exista

    if (!id || !im || !mask) {
        qDebug() << "Error cargando imagenes.";
        if (!id) qDebug() << "No se pudo cargar I_D.bmp";
        if (!im) qDebug() << "No se pudo cargar I_M.bmp";
        if (!mask) qDebug() << "No se pudo cargar M.bmp";
        return -1;
    }

    // Aquí va el resto de tu lógica de procesamiento y reconstrucción

    // Liberar memoria
    delete[] id;
    delete[] im;
    delete[] mask;

    return a.exec();
}

