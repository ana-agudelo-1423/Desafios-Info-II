#include <iostream>

using namespace std;

class  usuario{
public:
    string numerodocumento;
    int antiguedad;
    float puntuacion;
};
class Huesped : public Usuario {
public:

    struct NodoReserva {
        class Reservacion* reserva;
        NodoReserva* siguiente;
    };
    NodoReserva* reservaciones;

    Huesped(const char* doc, int ant, float punt)
        : Usuario(doc, ant, punt), reservaciones(nullptr) {}


    void agregarReserva(class Reservacion* r) {
        NodoReserva* nuevoNodo = new NodoReserva{r, reservaciones};
        reservaciones = nuevoNodo;
    }

    ~Huesped() {
        while (reservaciones) {
            NodoReserva* temp = reservaciones;
            reservaciones = reservaciones->siguiente;
            delete temp->reserva;
            delete temp;
        }
    }
};
class Anfitrion : public Usuario {
private:
    struct NodoAlojamiento {
        class Alojamiento* alojamiento;
        NodoAlojamiento* siguiente;
    };
    NodoAlojamiento* alojamientos;

public:

    Anfitrion(const char* doc, int ant, float punt)
        : Usuario(doc, ant, punt), alojamientos(nullptr) {}

    void agregarAlojamiento(class Alojamiento* a) {
        NodoAlojamiento* nuevoNodo = new NodoAlojamiento{a, alojamientos};
        alojamientos = nuevoNodo;
    }

    ~Anfitrion() {
        while (alojamientos) {
            NodoAlojamiento* temp = alojamientos;
            alojamientos = alojamientos->siguiente;
            delete temp->alojamiento;  // Liberar el alojamiento
            delete temp;              // Liberar el nodo
        }
    }
};

class Alojamiento {
public:
    char* codigo;
    Anfitrion* anfitrion;  // Relación: pertenece a un anfitrión

    Alojamiento(const char* cod, Anfitrion* anf) : anfitrion(anf) {
        codigo = new char[strlen(cod) + 1];
        strcpy(codigo, cod);
    }

    ~Alojamiento() {
        delete[] codigo;
    }
};
class Reservacion {
public:
    char* codigo;
    Huesped* huesped;      // Relación: hecha por un huésped
    Alojamiento* alojamiento; // Relación: en un alojamiento

    Reservacion(const char* cod, Huesped* h, Alojamiento* a)
        : huesped(h), alojamiento(a) {
        codigo = new char[strlen(cod) + 1];
        strcpy(codigo, cod);
    }

    ~Reservacion() {
        delete[] codigo;
    }
};
