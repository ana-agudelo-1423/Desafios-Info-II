#include <iostream>
#include <string.h>
#include <ctime>

using namespace std;

// Declaraciones adelantadas para resolver problemas de tipo incompleto
class Reservacion;
class Alojamiento;

// Clase base Usuario
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

    const char* getDocumento() const { return numeroDocumento; }
    int getAntiguedad() const { return antiguedad; }
    float getPuntuacion() const { return puntuacion; }
};

// Clase Huesped
class Huesped : public Usuario {
public:
    struct NodoReserva {
        Reservacion* reserva;
        NodoReserva* siguiente;
        NodoReserva(Reservacion* r, NodoReserva* sig) : reserva(r), siguiente(sig) {}
    };
    NodoReserva* reservaciones;

    Huesped(const char* doc, int ant, float punt)
        : Usuario(doc, ant, punt), reservaciones(nullptr) {}

    void agregarReserva(Reservacion* r);
    bool tieneReservaEnFecha(const tm& fecha) const;

    ~Huesped() {
        while (reservaciones) {
            NodoReserva* temp = reservaciones;
            reservaciones = reservaciones->siguiente;
            delete temp->reserva;
            delete temp;
        }
    }
};

// Clase Anfitrion
class Anfitrion : public Usuario {
private:
    struct NodoAlojamiento {
        Alojamiento* alojamiento;
        NodoAlojamiento* siguiente;
        NodoAlojamiento(Alojamiento* a, NodoAlojamiento* sig) : alojamiento(a), siguiente(sig) {}
    };
    NodoAlojamiento* alojamientos;

public:
    Anfitrion(const char* doc, int ant, float punt)
        : Usuario(doc, ant, punt), alojamientos(nullptr) {}

    void agregarAlojamiento(Alojamiento* a) {
        NodoAlojamiento* nuevoNodo = new NodoAlojamiento(a, alojamientos);
        alojamientos = nuevoNodo;
    }

    void mostrarReservaciones(const tm& fechaInicio, const tm& fechaFin) const;

    ~Anfitrion() {
        while (alojamientos) {
            NodoAlojamiento* temp = alojamientos;
            alojamientos = alojamientos->siguiente;
            delete temp->alojamiento;
            delete temp;
        }
    }
};

// Clase Alojamiento (completada con todos los miembros necesarios)
class Alojamiento {
private:
    char* codigo;
    char* nombre;
    char* direccion;
    char* municipio;
    char* departamento;
    char tipo;
    float precioPorNoche;
    bool amenidades[7];
    Anfitrion* anfitrion; // Miembro añadido

    struct NodoFecha {
        tm fecha;
        NodoFecha* siguiente;
        NodoFecha(const tm& f, NodoFecha* sig) : fecha(f), siguiente(sig) {}
    };
    NodoFecha* fechasReservadas;

public:
    Alojamiento(const char* cod, const char* nom, const char* dir, const char* mun,
                const char* dep, char tip, float precio, Anfitrion* anf)
        : tipo(tip), precioPorNoche(precio), anfitrion(anf), fechasReservadas(nullptr) {
        codigo = new char[strlen(cod) + 1]; strcpy(codigo, cod);
        nombre = new char[strlen(nom) + 1]; strcpy(nombre, nom);
        direccion = new char[strlen(dir) + 1]; strcpy(direccion, dir);
        municipio = new char[strlen(mun) + 1]; strcpy(municipio, mun);
        departamento = new char[strlen(dep) + 1]; strcpy(departamento, dep);
    }

    bool estaDisponible(const tm& fechaInicio, int noches) const {
        // Implementación simplificada
        return true;
    }

    void agregarReserva(const tm& fecha) {
        fechasReservadas = new NodoFecha(fecha, fechasReservadas);
    }

    void mostrarReservaciones(const tm& fechaInicio, const tm& fechaFin) const {
        NodoFecha* actual = fechasReservadas;
        while (actual) {
            tm fechaTemp = actual->fecha; // Copia para evitar modificar el original
            time_t fechaReserva = mktime(&fechaTemp);
            tm fechaInicioTemp = fechaInicio;
            tm fechaFinTemp = fechaFin;
            time_t inicio = mktime(&fechaInicioTemp);
            time_t fin = mktime(&fechaFinTemp);

            if (difftime(fechaReserva, inicio) >= 0 &&
                difftime(fechaReserva, fin) <= 0) {
                cout << "Reserva: " << codigo << " - " << asctime(&fechaTemp);
            }
            actual = actual->siguiente;
        }
    }

    // Métodos getter añadidos
    const char* getNombre() const { return nombre; }
    float getPrecio() const { return precioPorNoche; }
    const char* getCodigo() const { return codigo; }

    ~Alojamiento() {
        delete[] codigo;
        delete[] nombre;
        delete[] direccion;
        delete[] municipio;
        delete[] departamento;
        while (fechasReservadas) {
            NodoFecha* temp = fechasReservadas;
            fechasReservadas = fechasReservadas->siguiente;
            delete temp;
        }
    }
};

// Clase Reservacion (completada)
class Reservacion {
private:
    char* codigo;
    Huesped* huesped;
    Alojamiento* alojamiento;
    tm fechaEntrada;
    int duracion;
    char metodoPago[20];
    tm fechaPago;
    float monto;
    char* anotaciones;

public:
    Reservacion(const char* cod, Huesped* h, Alojamiento* a, const tm& fecha, int dur,
                const char* metodo, float mont, const char* notas)
        : huesped(h), alojamiento(a), duracion(dur), monto(mont) {
        codigo = new char[strlen(cod) + 1]; strcpy(codigo, cod);
        strcpy(metodoPago, metodo);
        fechaEntrada = fecha;
        fechaPago = fecha; // Simplificado
        anotaciones = new char[strlen(notas) + 1]; strcpy(anotaciones, notas);
    }

    bool coincideConFecha(const tm& fecha) const {
        tm fechaInicioTemp = fechaEntrada;
        tm fechaCompararTemp = fecha;
        time_t fechaReservaInicio = mktime(&fechaInicioTemp);
        time_t fechaReservaFin = fechaReservaInicio + (duracion * 24 * 3600);
        time_t fechaComparar = mktime(&fechaCompararTemp);

        return (difftime(fechaComparar, fechaReservaInicio) >= 0 &&
                difftime(fechaComparar, fechaReservaFin) <= 0);
    }

    // Métodos getter añadidos
    const char* getCodigo() const { return codigo; }
    const Alojamiento* getAlojamiento() const { return alojamiento; }
    const Huesped* getHuesped() const { return huesped; }

    ~Reservacion() {
        delete[] codigo;
        delete[] anotaciones;
    }
};

// Implementación de métodos de Huesped que necesitan la definición completa de Reservacion
void Huesped::agregarReserva(Reservacion* r) {
    NodoReserva* nuevoNodo = new NodoReserva(r, reservaciones);
    reservaciones = nuevoNodo;
}

bool Huesped::tieneReservaEnFecha(const tm& fecha) const {
    NodoReserva* actual = reservaciones;
    while (actual) {
        if (actual->reserva->coincideConFecha(fecha)) {
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}

// Implementación de métodos de Anfitrion que necesitan la definición completa de Alojamiento
void Anfitrion::mostrarReservaciones(const tm& fechaInicio, const tm& fechaFin) const {
    NodoAlojamiento* actual = alojamientos;
    while (actual) {
        actual->alojamiento->mostrarReservaciones(fechaInicio, fechaFin);
        actual = actual->siguiente;
    }
}

// Resto de las funciones (reserva_alojamiento, anulacion_reserva, etc.) permanecen igual
// pero ahora pueden acceder a los métodos getter públicos

int main() {
    // Ejemplo de uso
    Anfitrion* anfitrion = new Anfitrion("ANF-001", 12, 4.8);
    Alojamiento* alojamiento1 = new Alojamiento("ALO-001", "Casa Campestre", "Calle 123",
                                                "Medellín", "Antioquia", 'C', 120000, anfitrion);
    Alojamiento* alojamiento2 = new Alojamiento("ALO-002", "Apartamento Moderno", "Carrera 45",
                                                "Medellín", "Antioquia", 'A', 90000, anfitrion);
    anfitrion->agregarAlojamiento(alojamiento1);
    anfitrion->agregarAlojamiento(alojamiento2);

    Huesped* huesped = new Huesped("HUE-001", 6, 4.5);

    Alojamiento* alojamientos[] = {alojamiento1, alojamiento2};

    // Menú de ejemplo
    int opcion;
    do {
        cout << "\n1. Reservar alojamiento\n2. Anular reserva\n3. Salir\nOpcion: ";
        cin >> opcion;

        switch(opcion) {
        case 1: {
            tm fecha;
            cout << "Ingrese fecha de entrada (dd mm aaaa): ";
            cin >> fecha.tm_mday >> fecha.tm_mon >> fecha.tm_year;
            fecha.tm_mon--; // Ajuste para struct tm
            fecha.tm_year -= 1900;

            int noches;
            cout << "Número de noches: ";
            cin >> noches;

            // Mostrar alojamientos disponibles
            cout << "\nAlojamientos disponibles:\n";
            for (int i = 0; i < 2; i++) {
                cout << i+1 << ". " << alojamientos[i]->getNombre()
                << " - $" << alojamientos[i]->getPrecio() << "/noche\n";
            }

            int seleccion;
            cout << "Seleccione un alojamiento (1-2): ";
            cin >> seleccion;

            if (seleccion >= 1 && seleccion <= 2) {
                if (alojamientos[seleccion-1]->estaDisponible(fecha, noches) &&
                    !huesped->tieneReservaEnFecha(fecha)) {
                    char codigoReserva[20];
                    sprintf(codigoReserva, "RES-%03d", rand() % 1000);

                    Reservacion* reserva = new Reservacion(
                        codigoReserva, huesped, alojamientos[seleccion-1],
                        fecha, noches, "TC",
                        noches * alojamientos[seleccion-1]->getPrecio(),
                        "Sin anotaciones");

                    huesped->agregarReserva(reserva);
                    alojamientos[seleccion-1]->agregarReserva(fecha);

                    cout << "Reserva creada. Código: " << codigoReserva << "\n";
                } else {
                    cout << "No se pudo completar la reserva.\n";
                }
            } else {
                cout << "Selección inválida.\n";
            }
            break;
        }
        case 2: {
            char codigo[20];
            cout << "Código de reserva a anular: ";
            cin >> codigo;

            Huesped::NodoReserva* actual = huesped->reservaciones;
            Huesped::NodoReserva* anterior = nullptr;

            while (actual) {
                if (strcmp(actual->reserva->getCodigo(), codigo) == 0) {
                    if (anterior) {
                        anterior->siguiente = actual->siguiente;
                    } else {
                        huesped->reservaciones = actual->siguiente;
                    }

                    delete actual->reserva;
                    delete actual;
                    cout << "Reserva anulada.\n";
                    break;
                }
                anterior = actual;
                actual = actual->siguiente;
            }

            if (!actual) {
                cout << "Reserva no encontrada.\n";
            }
            break;
        }
        }
    } while (opcion != 3);

    delete huesped;
    delete anfitrion;

    return 0;
}
