#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <iomanip>

using namespace std;

const char* ARCHIVO_DATOS = "UdeAStay_data.csv";

// Estructura para fecha
struct Fecha {
    int dia;
    int mes;
    int ano;
};

// Declaraciones adelantadas
class Alojamiento;
class Reservacion;
class Usuario;
class Anfitrion;
class Huesped;

// Clase base Usuario
class Usuario {
protected:
    char* documento;
    int antiguedad;
    float puntuacion;
public:
    Usuario(const char* doc, int ant, float punt) : antiguedad(ant), puntuacion(punt) {
        documento = new char[strlen(doc) + 1];
        strcpy(documento, doc);
    }
    virtual ~Usuario() { delete[] documento; }

    const char* getDocumento() const { return documento; }
    int getAntiguedad() const { return antiguedad; }
    float getPuntuacion() const { return puntuacion; }

    virtual void mostrarMenu() = 0;
    virtual const char* getTipo() const = 0;
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
    Anfitrion(const char* doc, int ant, float punt) : Usuario(doc, ant, punt), alojamientos(nullptr) {}

    void agregarAlojamiento(Alojamiento* a);
    void mostrarReservaciones(Fecha inicio, Fecha fin) const;
    void mostrarMenu() override;
    const char* getTipo() const override { return "ANFITRION"; }

    ~Anfitrion() {
        while(alojamientos) {
            NodoAlojamiento* temp = alojamientos;
            alojamientos = alojamientos->siguiente;
            delete temp;
        }
    }
};

// Clase Huesped
class Huesped : public Usuario {
private:
    struct NodoReserva {
        Reservacion* reserva;
        NodoReserva* siguiente;
        NodoReserva(Reservacion* r, NodoReserva* sig) : reserva(r), siguiente(sig) {}
    };
    NodoReserva* reservaciones;

public:
    Huesped(const char* doc, int ant, float punt) : Usuario(doc, ant, punt), reservaciones(nullptr) {}

    void agregarReserva(Reservacion* r);
    bool tieneReservaEnFecha(Fecha fecha) const;
    void mostrarReservaciones() const;
    void mostrarMenu() override;
    const char* getTipo() const override { return "HUESPED"; }

    ~Huesped() {
        while(reservaciones) {
            NodoReserva* temp = reservaciones;
            reservaciones = reservaciones->siguiente;
            delete temp->reserva;
            delete temp;
        }
    }
};

// Clase Alojamiento
class Alojamiento {
private:
    char* codigo;
    char* nombre;
    char* direccion;
    char* municipio;
    char* departamento;
    char tipo; // 'C' Casa, 'A' Apartamento
    float precioNoche;
    bool amenidades[7]; // Ascensor, Piscina, Aire, Caja, Parqueadero, Patio, Wifi
    Anfitrion* anfitrion;

    struct NodoFecha {
        Fecha fecha;
        NodoFecha* siguiente;
        NodoFecha(Fecha f, NodoFecha* sig) : fecha(f), siguiente(sig) {}
    };
    NodoFecha* fechasReservadas;

public:
    Alojamiento(const char* cod, const char* nom, const char* dir, const char* mun,
                const char* dep, char tip, float precio, Anfitrion* anf);

    bool estaDisponible(Fecha inicio, int noches) const;
    void agregarReserva(Fecha fecha);
    void mostrarReservaciones(Fecha inicio, Fecha fin) const;
    void mostrarInfo() const;

    const char* getCodigo() const { return codigo; }
    const char* getNombre() const { return nombre; }
    const char* getDireccion() const { return direccion; }
    const char* getMunicipio() const { return municipio; }
    const char* getDepartamento() const { return departamento; }
    char getTipo() const { return tipo; }
    float getPrecio() const { return precioNoche; }
    Anfitrion* getAnfitrion() const { return anfitrion; }

    ~Alojamiento();
};

// Clase Reservacion
class Reservacion {
private:
    char* codigo;
    Huesped* huesped;
    Alojamiento* alojamiento;
    Fecha fechaEntrada;
    int duracion;
    char metodoPago[20];
    float monto;
    char* anotaciones;

public:
    Reservacion(const char* cod, Huesped* h, Alojamiento* a, Fecha fecha,
                int dur, const char* metodo, float mont, const char* notas);

    bool coincideConFecha(Fecha fecha) const;
    void mostrarComprobante() const;

    const char* getCodigo() const { return codigo; }
    Huesped* getHuesped() const { return huesped; }
    Alojamiento* getAlojamiento() const { return alojamiento; }
    Fecha getFechaEntrada() const { return fechaEntrada; }
    int getDuracion() const { return duracion; }
    const char* getMetodoPago() const { return metodoPago; }
    float getMonto() const { return monto; }
    const char* getAnotaciones() const { return anotaciones; }

    ~Reservacion();
};

// Implementación de Alojamiento
Alojamiento::Alojamiento(const char* cod, const char* nom, const char* dir, const char* mun,
                         const char* dep, char tip, float precio, Anfitrion* anf)
    : tipo(tip), precioNoche(precio), anfitrion(anf), fechasReservadas(nullptr) {
    codigo = new char[strlen(cod) + 1]; strcpy(codigo, cod);
    nombre = new char[strlen(nom) + 1]; strcpy(nombre, nom);
    direccion = new char[strlen(dir) + 1]; strcpy(direccion, dir);
    municipio = new char[strlen(mun) + 1]; strcpy(municipio, mun);
    departamento = new char[strlen(dep) + 1]; strcpy(departamento, dep);
    for(int i = 0; i < 7; i++) amenidades[i] = false;
}

bool Alojamiento::estaDisponible(Fecha inicio, int noches) const {
    tm tmInicio = {0, 0, 0, inicio.dia, inicio.mes - 1, inicio.ano - 1900};
    time_t tInicio = mktime(&tmInicio);
    time_t tFin = tInicio + noches * 24 * 3600;

    NodoFecha* actual = fechasReservadas;
    while(actual) {
        tm tmReserva = {0, 0, 0, actual->fecha.dia, actual->fecha.mes - 1, actual->fecha.ano - 1900};
        time_t tReserva = mktime(&tmReserva);
        if((tReserva >= tInicio && tReserva <= tFin) ||
            (tReserva + 24*3600 >= tInicio && tReserva + 24*3600 <= tFin)) {
            return false;
        }
        actual = actual->siguiente;
    }
    return true;
}

void Alojamiento::agregarReserva(Fecha fecha) {
    fechasReservadas = new NodoFecha(fecha, fechasReservadas);
}

void Alojamiento::mostrarInfo() const {
    cout << "Código: " << codigo << "\n"
         << "Nombre: " << nombre << "\n"
         << "Dirección: " << direccion << ", " << municipio << ", " << departamento << "\n"
         << "Tipo: " << (tipo == 'C' ? "Casa" : "Apartamento") << "\n"
         << "Precio por noche: $" << fixed << setprecision(2) << precioNoche << "\n"
         << "Anfitrión: " << anfitrion->getDocumento() << " (Puntuación: "
         << anfitrion->getPuntuacion() << ")\n"
         << "Amenidades: ";
    const char* amenidadesNombres[] = {"Ascensor", "Piscina", "Aire acondicionado",
                                       "Caja fuerte", "Parqueadero", "Patio", "Wifi"};
    bool primera = true;
    for(int i = 0; i < 7; i++) {
        if(amenidades[i]) {
            if(!primera) cout << ", ";
            cout << amenidadesNombres[i];
            primera = false;
        }
    }
    if(primera) cout << "Ninguna";
    cout << "\n";
}

void Alojamiento::mostrarReservaciones(Fecha inicio, Fecha fin) const {
    cout << "Reservaciones entre " << inicio.dia << "/" << inicio.mes << "/" << inicio.ano
         << " y " << fin.dia << "/" << fin.mes << "/" << fin.ano << ":\n";

    NodoFecha* actual = fechasReservadas;
    bool hayReservas = false;

    while(actual) {
        if((actual->fecha.ano > inicio.ano ||
             (actual->fecha.ano == inicio.ano && actual->fecha.mes > inicio.mes) ||
             (actual->fecha.ano == inicio.ano && actual->fecha.mes == inicio.mes && actual->fecha.dia >= inicio.dia)) &&
            (actual->fecha.ano < fin.ano ||
             (actual->fecha.ano == fin.ano && actual->fecha.mes < fin.mes) ||
             (actual->fecha.ano == fin.ano && actual->fecha.mes == fin.mes && actual->fecha.dia <= fin.dia))) {
            cout << "- " << actual->fecha.dia << "/" << actual->fecha.mes << "/" << actual->fecha.ano << "\n";
            hayReservas = true;
        }
        actual = actual->siguiente;
    }

    if(!hayReservas) {
        cout << "No hay reservaciones en este período.\n";
    }
}

Alojamiento::~Alojamiento() {
    delete[] codigo;
    delete[] nombre;
    delete[] direccion;
    delete[] municipio;
    delete[] departamento;
    while(fechasReservadas) {
        NodoFecha* temp = fechasReservadas;
        fechasReservadas = fechasReservadas->siguiente;
        delete temp;
    }
}

// Implementación de Reservacion
Reservacion::Reservacion(const char* cod, Huesped* h, Alojamiento* a, Fecha fecha,
                         int dur, const char* metodo, float mont, const char* notas)
    : huesped(h), alojamiento(a), fechaEntrada(fecha), duracion(dur), monto(mont) {
    codigo = new char[strlen(cod) + 1]; strcpy(codigo, cod);
    strncpy(metodoPago, metodo, 19); metodoPago[19] = '\0';
    anotaciones = new char[strlen(notas) + 1]; strcpy(anotaciones, notas);
}

bool Reservacion::coincideConFecha(Fecha fecha) const {
    tm tmInicio = {0, 0, 0, fechaEntrada.dia, fechaEntrada.mes - 1, fechaEntrada.ano - 1900};
    time_t tInicio = mktime(&tmInicio);
    time_t tFin = tInicio + duracion * 24 * 3600;

    tm tmFecha = {0, 0, 0, fecha.dia, fecha.mes - 1, fecha.ano - 1900};
    time_t tFecha = mktime(&tmFecha);

    return (tFecha >= tInicio && tFecha <= tFin);
}

void Reservacion::mostrarComprobante() const {
    tm tmFecha = {0, 0, 0, fechaEntrada.dia, fechaEntrada.mes - 1, fechaEntrada.ano - 1900};
    mktime(&tmFecha);
    char fechaStr[100];
    strftime(fechaStr, 100, "%A, %d de %B del %Y", &tmFecha);

    tm tmFin = tmFecha;
    tmFin.tm_mday += duracion;
    mktime(&tmFin);
    char finStr[100];
    strftime(finStr, 100, "%A, %d de %B del %Y", &tmFin);

    cout << "\n=== COMPROBANTE DE RESERVA ===\n";
    cout << "Código de reserva: " << codigo << "\n";
    cout << "Huésped: " << huesped->getDocumento() << "\n";
    cout << "Alojamiento: " << alojamiento->getCodigo() << " - " << alojamiento->getNombre() << "\n";
    cout << "Fecha de entrada: " << fechaStr << "\n";
    cout << "Fecha de salida: " << finStr << "\n";
    cout << "Duración: " << duracion << " noches\n";
    cout << "Monto total: $" << fixed << setprecision(2) << monto << "\n";
    cout << "Método de pago: " << metodoPago << "\n";
    cout << "Anotaciones: " << anotaciones << "\n";
    cout << "=============================\n";
}

Reservacion::~Reservacion() {
    delete[] codigo;
    delete[] anotaciones;
}

// Implementación de métodos de Huesped
void Huesped::agregarReserva(Reservacion* r) {
    reservaciones = new NodoReserva(r, reservaciones);
}

bool Huesped::tieneReservaEnFecha(Fecha fecha) const {
    NodoReserva* actual = reservaciones;
    while(actual) {
        if(actual->reserva->coincideConFecha(fecha)) {
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}

void Huesped::mostrarReservaciones() const {
    if(!reservaciones) {
        cout << "No tiene reservaciones activas.\n";
        return;
    }

    cout << "\n=== SUS RESERVACIONES ===\n";
    NodoReserva* actual = reservaciones;
    while(actual) {
        actual->reserva->mostrarComprobante();
        actual = actual->siguiente;
    }
}

void Huesped::mostrarMenu() {
    int opcion;
    do {
        cout << "\n=== MENÚ HUÉSPED ===\n";
        cout << "1. Ver mis reservaciones\n";
        cout << "2. Reservar alojamiento\n";
        cout << "3. Anular reservación\n";
        cout << "4. Salir\n";
        cout << "Opción: ";
        cin >> opcion;

        switch(opcion) {
        case 1:
            mostrarReservaciones();
            break;
        case 2: {
            Fecha fecha;
            int noches;
            cout << "Fecha de entrada (dd mm aaaa): ";
            cin >> fecha.dia >> fecha.mes >> fecha.ano;
            cout << "Número de noches: ";
            cin >> noches;

            // Aquí iría la lógica para buscar y reservar alojamientos
            cout << "Funcionalidad de reserva en desarrollo\n";
            break;
        }
        case 3: {
            char codigo[20];
            cout << "Código de reserva a anular: ";
            cin >> codigo;

            // Aquí iría la lógica para anular reservaciones
            cout << "Funcionalidad de anulación en desarrollo\n";
            break;
        }
        case 4:
            cout << "Saliendo del menú de huésped...\n";
            break;
        default:
            cout << "Opción inválida.\n";
        }
    } while(opcion != 4);
}

// Implementación de métodos de Anfitrion
void Anfitrion::agregarAlojamiento(Alojamiento* a) {
    alojamientos = new NodoAlojamiento(a, alojamientos);
}

void Anfitrion::mostrarReservaciones(Fecha inicio, Fecha fin) const {
    cout << "\nReservaciones de " << getDocumento() << " entre "
         << inicio.dia << "/" << inicio.mes << "/" << inicio.ano
         << " y " << fin.dia << "/" << fin.mes << "/" << fin.ano << ":\n";

    NodoAlojamiento* actual = alojamientos;
    bool hayReservas = false;

    while(actual) {
        cout << "Alojamiento: " << actual->alojamiento->getNombre() << " ("
             << actual->alojamiento->getCodigo() << ")\n";
        actual->alojamiento->mostrarReservaciones(inicio, fin);
        actual = actual->siguiente;
        hayReservas = true;
    }

    if(!hayReservas) {
        cout << "No hay alojamientos registrados.\n";
    }
}

void Anfitrion::mostrarMenu() {
    int opcion;
    do {
        cout << "\n=== MENÚ ANFITRIÓN ===\n";
        cout << "1. Ver mis alojamientos\n";
        cout << "2. Consultar reservaciones\n";
        cout << "3. Anular reservación\n";
        cout << "4. Actualizar histórico\n";
        cout << "5. Salir\n";
        cout << "Opción: ";
        cin >> opcion;

        switch(opcion) {
        case 1: {
            NodoAlojamiento* actual = alojamientos;
            if(!actual) {
                cout << "No tiene alojamientos registrados.\n";
            } else {
                while(actual) {
                    actual->alojamiento->mostrarInfo();
                    cout << "-----------------\n";
                    actual = actual->siguiente;
                }
            }
            break;
        }
        case 2: {
            Fecha inicio, fin;
            cout << "Fecha inicio (dd mm aaaa): ";
            cin >> inicio.dia >> inicio.mes >> inicio.ano;
            cout << "Fecha fin (dd mm aaaa): ";
            cin >> fin.dia >> fin.mes >> fin.ano;
            mostrarReservaciones(inicio, fin);
            break;
        }
        case 3: {
            char codigo[20];
            cout << "Código de reserva a anular: ";
            cin >> codigo;
            cout << "Funcionalidad de anulación en desarrollo\n";
            break;
        }
        case 4:
            cout << "Funcionalidad de actualización de histórico en desarrollo\n";
            break;
        case 5:
            cout << "Saliendo del menú de anfitrión...\n";
            break;
        default:
            cout << "Opción inválida.\n";
        }
    } while(opcion != 5);
}

// Clase SistemaUdeAStay
class SistemaUdeAStay {
private:
    struct NodoUsuario {
        Usuario* usuario;
        NodoUsuario* siguiente;
        NodoUsuario(Usuario* u, NodoUsuario* sig) : usuario(u), siguiente(sig) {}
    };
    NodoUsuario* usuarios;

    struct NodoAlojamiento {
        Alojamiento* alojamiento;
        NodoAlojamiento* siguiente;
        NodoAlojamiento(Alojamiento* a, NodoAlojamiento* sig) : alojamiento(a), siguiente(sig) {}
    };
    NodoAlojamiento* alojamientos;

    struct NodoReservacion {
        Reservacion* reservacion;
        NodoReservacion* siguiente;
        NodoReservacion(Reservacion* r, NodoReservacion* sig) : reservacion(r), siguiente(sig) {}
    };
    NodoReservacion* reservaciones;

public:
    SistemaUdeAStay() : usuarios(nullptr), alojamientos(nullptr), reservaciones(nullptr) {}

    void agregarUsuario(Usuario* u) {
        usuarios = new NodoUsuario(u, usuarios);
    }

    void agregarAlojamiento(Alojamiento* a) {
        alojamientos = new NodoAlojamiento(a, alojamientos);
    }

    void agregarReservacion(Reservacion* r) {
        reservaciones = new NodoReservacion(r, reservaciones);
    }

    Usuario* buscarUsuario(const char* documento) {
        NodoUsuario* actual = usuarios;
        while(actual) {
            if(strcmp(actual->usuario->getDocumento(), documento) == 0) {
                return actual->usuario;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    Alojamiento* buscarAlojamiento(const char* codigo) {
        NodoAlojamiento* actual = alojamientos;
        while(actual) {
            if(strcmp(actual->alojamiento->getCodigo(), codigo) == 0) {
                return actual->alojamiento;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    void guardarCSV() {
        ofstream archivo(ARCHIVO_DATOS);
        if(!archivo.is_open()) {
            cerr << "Error al crear archivo CSV\n";
            return;
        }

        // Encabezados
        archivo << "TIPO,DOCUMENTO,ANTIGUEDAD,PUNTUACION,CODIGO_ALOJ,NOMBRE,DIRECCION,"
                << "MUNICIPIO,DEPARTAMENTO,TIPO_ALOJ,PRECIO,CODIGO_RES,FECHA_ENTRADA,"
                << "DURACION,METODO_PAGO,MONTO,ANOTACIONES\n";

        // Guardar usuarios
        NodoUsuario* actualUser = usuarios;
        while(actualUser) {
            archivo << actualUser->usuario->getTipo() << ","
                    << actualUser->usuario->getDocumento() << ","
                    << actualUser->usuario->getAntiguedad() << ","
                    << actualUser->usuario->getPuntuacion() << ",,,,,,,,,,,\n";
            actualUser = actualUser->siguiente;
        }

        // Guardar alojamientos
        NodoAlojamiento* actualAloj = alojamientos;
        while(actualAloj) {
            archivo << "ALOJAMIENTO,"
                    << actualAloj->alojamiento->getAnfitrion()->getDocumento() << ",,,"
                    << actualAloj->alojamiento->getCodigo() << ","
                    << actualAloj->alojamiento->getNombre() << ","
                    << actualAloj->alojamiento->getDireccion() << ","
                    << actualAloj->alojamiento->getMunicipio() << ","
                    << actualAloj->alojamiento->getDepartamento() << ","
                    << actualAloj->alojamiento->getTipo() << ","
                    << actualAloj->alojamiento->getPrecio() << ",,,,,,\n";
            actualAloj = actualAloj->siguiente;
        }

        // Guardar reservaciones
        NodoReservacion* actualRes = reservaciones;
        while(actualRes) {
            Fecha fecha = actualRes->reservacion->getFechaEntrada();
            archivo << "RESERVACION,"
                    << actualRes->reservacion->getHuesped()->getDocumento() << ",,,,"
                    << actualRes->reservacion->getAlojamiento()->getCodigo() << ",,,,,,"
                    << actualRes->reservacion->getCodigo() << ","
                    << fecha.ano << "-" << setfill('0') << setw(2) << fecha.mes << "-"
                    << setw(2) << fecha.dia << ","
                    << actualRes->reservacion->getDuracion() << ","
                    << actualRes->reservacion->getMetodoPago() << ","
                    << actualRes->reservacion->getMonto() << ","
                    << actualRes->reservacion->getAnotaciones() << "\n";
            actualRes = actualRes->siguiente;
        }

        archivo.close();
        cout << "Datos guardados en " << ARCHIVO_DATOS << endl;
    }

    void cargarCSV() {
        ifstream archivo(ARCHIVO_DATOS);
        if(!archivo.is_open()) {
            cerr << "No se encontró archivo de datos, comenzando con sistema vacío\n";
            return;
        }

        // Leer encabezados
        char linea[1024];
        archivo.getline(linea, 1024);

        while(archivo.getline(linea, 1024)) {
            char* token = strtok(linea, ",");
            if(!token) continue;

            if(strcmp(token, "ANFITRION") == 0) {
                char* documento = strtok(NULL, ",");
                char* antiguedad = strtok(NULL, ",");
                char* puntuacion = strtok(NULL, ",");

                if(documento && antiguedad && puntuacion) {
                    agregarUsuario(new Anfitrion(
                        documento, atoi(antiguedad), atof(puntuacion)
                        ));
                }
            }
            else if(strcmp(token, "HUESPED") == 0) {
                char* documento = strtok(NULL, ",");
                char* antiguedad = strtok(NULL, ",");
                char* puntuacion = strtok(NULL, ",");

                if(documento && antiguedad && puntuacion) {
                    agregarUsuario(new Huesped(
                        documento, atoi(antiguedad), atof(puntuacion)
                        ));
                }
            }
            else if(strcmp(token, "ALOJAMIENTO") == 0) {
                char* docAnfitrion = strtok(NULL, ",");
                strtok(NULL, ","); // Saltar campos vacíos
                strtok(NULL, ",");
                strtok(NULL, ",");
                char* codigo = strtok(NULL, ",");
                char* nombre = strtok(NULL, ",");
                char* direccion = strtok(NULL, ",");
                char* municipio = strtok(NULL, ",");
                char* departamento = strtok(NULL, ",");
                char* tipo = strtok(NULL, ",");
                char* precio = strtok(NULL, ",");

                if(docAnfitrion && codigo && nombre && direccion &&
                    municipio && departamento && tipo && precio) {
                    Usuario* user = buscarUsuario(docAnfitrion);
                    Anfitrion* anf = dynamic_cast<Anfitrion*>(user);
                    if(anf) {
                        Alojamiento* nuevo = new Alojamiento(
                            codigo, nombre, direccion, municipio, departamento,
                            tipo[0], atof(precio), anf
                            );
                        agregarAlojamiento(nuevo);
                        anf->agregarAlojamiento(nuevo);
                    }
                }
            }
            else if(strcmp(token, "RESERVACION") == 0) {
                char* docHuesped = strtok(NULL, ",");
                strtok(NULL, ","); // Saltar campos vacíos
                strtok(NULL, ",");
                strtok(NULL, ",");
                strtok(NULL, ",");
                char* codAlojamiento = strtok(NULL, ",");
                strtok(NULL, ","); // Saltar campos vacíos (6 veces)
                strtok(NULL, ",");
                strtok(NULL, ",");
                strtok(NULL, ",");
                strtok(NULL, ",");
                strtok(NULL, ",");
                char* codReserva = strtok(NULL, ",");
                char* fechaEntrada = strtok(NULL, ",");
                char* duracion = strtok(NULL, ",");
                char* metodoPago = strtok(NULL, ",");
                char* monto = strtok(NULL, ",");
                char* anotaciones = strtok(NULL, ",");

                if(docHuesped && codAlojamiento && codReserva && fechaEntrada &&
                    duracion && metodoPago && monto && anotaciones) {
                    Usuario* user = buscarUsuario(docHuesped);
                    Huesped* hue = dynamic_cast<Huesped*>(user);
                    Alojamiento* alo = buscarAlojamiento(codAlojamiento);

                    if(hue && alo) {
                        Fecha fecha;
                        sscanf(fechaEntrada, "%d-%d-%d", &fecha.ano, &fecha.mes, &fecha.dia);

                        Reservacion* nueva = new Reservacion(
                            codReserva, hue, alo, fecha,
                            atoi(duracion), metodoPago,
                            atof(monto), anotaciones
                            );
                        agregarReservacion(nueva);
                        hue->agregarReserva(nueva);
                        alo->agregarReserva(fecha);
                    }
                }
            }
        }

        archivo.close();
        cout << "Datos cargados desde " << ARCHIVO_DATOS << endl;
    }

    ~SistemaUdeAStay() {
        while(usuarios) {
            NodoUsuario* temp = usuarios;
            usuarios = usuarios->siguiente;
            delete temp->usuario;
            delete temp;
        }

        while(alojamientos) {
            NodoAlojamiento* temp = alojamientos;
            alojamientos = alojamientos->siguiente;
            delete temp->alojamiento;
            delete temp;
        }

        while(reservaciones) {
            NodoReservacion* temp = reservaciones;
            reservaciones = reservaciones->siguiente;
            delete temp->reservacion;
            delete temp;
        }
    }
};

// Función principal
int main() {
    SistemaUdeAStay sistema;
    sistema.cargarCSV();

    int opcion;
    do {
        cout << "\n=== SISTEMA UdeAStay ===\n";
        cout << "1. Iniciar sesión\n";
        cout << "2. Guardar datos\n";
        cout << "3. Salir\n";
        cout << "Opción: ";
        cin >> opcion;

        switch(opcion) {
        case 1: {
            char documento[20];
            cout << "Ingrese su documento: ";
            cin >> documento;

            Usuario* user = sistema.buscarUsuario(documento);
            if(user) {
                user->mostrarMenu();
            } else {
                cout << "Usuario no encontrado.\n";
            }
            break;
        }
        case 2:
            sistema.guardarCSV();
            break;
        case 3:
            cout << "Saliendo del sistema...\n";
            break;
        default:
            cout << "Opción inválida.\n";
        }
    } while(opcion != 3);

    return 0;
}
