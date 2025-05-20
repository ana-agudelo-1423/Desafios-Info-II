#include <iostream>
#include <string.h>

using namespace std;

class Usuario {
protected:
    char* numeroDocumento;
    int antiguedad;
    float puntuacion;
public:
    Usuario(const char* doc, int ant, float punt)
        : antiguedad(ant), puntuacion(punt) {
        numeroDocumento = new char[strlen(doc) + 1];
        strcpy(numeroDocumento, doc);
    }
    virtual ~Usuario() { delete[] numeroDocumento; }
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
            delete temp->alojamiento;
            delete temp;
        }
    }
};

class Alojamiento {
public:
    char* codigo;
    Anfitrion* anfitrion;

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
    Huesped* huesped;
    Alojamiento* alojamiento;

    Reservacion(const char* cod, Huesped* h, Alojamiento* a)
        : huesped(h), alojamiento(a) {
        codigo = new char[strlen(cod) + 1];
        strcpy(codigo, cod);
    }

    ~Reservacion() {
        delete[] codigo;
    }
};
void actualizacion_datos(){
    FILE* archivo = fopen("alojamientos.txt", "a"); // Modo append
    if (!archivo) {
        printf("Error al abrir archivo.\n");
        return;
    }

    Anfitrion::NodoAlojamiento* actual = anfitrion->alojamientos;
    while (actual != nullptr) {
        fprintf(archivo, "%s|%s|%.2f\n",
                actual->alojamiento->codigo,
                anfitrion->numeroDocumento,
                actual->alojamiento->precioPorNoche);
        actual = actual->siguiente;
    }
    fclose(archivo);
}
void cargarAlojamientos(Anfitrion* anfitrion) {
    FILE* archivo = fopen("alojamientos.txt", "r");
    if (!archivo) {
        printf("Archivo no encontrado.\n");
        return;
    }

    char codigo[50], documento[50];
    float precio;
    while (fscanf(archivo, "%49[^|]|%49[^|]|%f\n", codigo, documento, &precio) == 3) {
        // Crear alojamiento y asignar al anfitrión
        Alojamiento* aloj = new Alojamiento(codigo, anfitrion);
        aloj->precioPorNoche = precio;
        anfitrion->agregarAlojamiento(aloj);
    }
    fclose(archivo);
}
void ingreso_usuarios(){

}
void reserva_alojamiento(){

}
void anulacion_reserva(){

}
void consultar_alojamiento(){}
void actualizar_historico(){}
void medicion_consumo_recursos(){}
int main() {
    Anfitrion* anfitrion = new Anfitrion("ANF-001", 12, 4.8);
    Alojamiento* alojamiento = new Alojamiento("ALO-001", anfitrion);
    anfitrion->agregarAlojamiento(alojamiento);

    Huesped* huesped = new Huesped("HUE-001", 6, 4.5);
    Reservacion* reserva = new Reservacion("RES-001", huesped, alojamiento);
    huesped->agregarReserva(reserva);

    delete huesped;
    delete anfitrion;

    cout << "Programa ejecutado correctamente." << endl;
    return 0;
}
