#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <iomanip>

using namespace std;
const char* ARCHIVO_DATOS = "UdeAStay_data.txt";

// Estructura para fecha
// Estructura que representa una fecha con comparación lógica
struct Fecha {
    int dia;
    int mes;
    int ano;

    bool operator<(const Fecha& otra) const {
        if(ano != otra.ano) return ano < otra.ano;
        if(mes != otra.mes) return mes < otra.mes;
        return dia < otra.dia;
    }
};

// Declaraciones anticipadas
// Clase que representa un alojamiento ofertado en el sistema
class Alojamiento;
// Clase que representa una reservación entre un huésped y un alojamiento
class Reservacion;
// Clase central que administra todo el sistema: usuarios, alojamientos y reservaciones
class SistemaUdeAStay;

// Clase Usuario (base)
// Clase base abstracta para representar un usuario del sistema (anfitrión o huésped)
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

    virtual ~Usuario() {
        delete[] documento;
    }

    const char* getDocumento() const { return documento; }
    int getAntiguedad() const { return antiguedad; }
    float getPuntuacion() const { return puntuacion; }

    virtual void mostrarMenu(SistemaUdeAStay& sistema) = 0;
    virtual const char* getTipo() const = 0;

    void actualizarPuntuacion(float nuevaPuntuacion) {
        puntuacion = nuevaPuntuacion;
    }
};

// Clase Anfitrion
// Clase derivada de Usuario que representa un anfitrión, con alojamientos propios
class Anfitrion : public Usuario {
private:
    struct NodoAlojamiento {
        Alojamiento* alojamiento;
        NodoAlojamiento* siguiente;
        NodoAlojamiento(Alojamiento* a, NodoAlojamiento* sig = nullptr)
            : alojamiento(a), siguiente(sig) {}
    };
    NodoAlojamiento* alojamientos;

public:
    Anfitrion(const char* doc, int ant, float punt)
        : Usuario(doc, ant, punt), alojamientos(nullptr) {}

    void agregarAlojamiento(Alojamiento* a) {
        alojamientos = new NodoAlojamiento(a, alojamientos);
    }

    void mostrarReservaciones(Fecha inicio, Fecha fin) const;
    void mostrarMenu(SistemaUdeAStay& sistema) override;
    const char* getTipo() const override { return "ANFITRION"; }

    void anularReservacion(SistemaUdeAStay& sistema);
    void actualizarHistorico(SistemaUdeAStay& sistema);

    ~Anfitrion() {
        while(alojamientos) {
            NodoAlojamiento* temp = alojamientos;
            alojamientos = alojamientos->siguiente;
            delete temp;
        }
    }
};

// Clase Huesped
// Clase derivada de Usuario que representa un huésped, con reservaciones
class Huesped : public Usuario {
public:
    struct NodoReserva {
        Reservacion* reserva;
        NodoReserva* siguiente;
        NodoReserva(Reservacion* r, NodoReserva* sig = nullptr)
            : reserva(r), siguiente(sig) {}
    };

private:
    NodoReserva* reservaciones;

public:
    Huesped(const char* doc, int ant, float punt)
        : Usuario(doc, ant, punt), reservaciones(nullptr) {}

    void agregarReserva(Reservacion* r) {
        reservaciones = new NodoReserva(r, reservaciones);
    }

    bool tieneReservaEnFecha(Fecha fecha) const;
    void mostrarReservaciones() const;
    void reservarAlojamiento(SistemaUdeAStay& sistema);
    void anularReservacion(SistemaUdeAStay& sistema);
    void mostrarMenu(SistemaUdeAStay& sistema) override;
    const char* getTipo() const override { return "HUESPED"; }

    NodoReserva* getReservaciones() const { return reservaciones; }

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
// Clase que representa un alojamiento ofertado en el sistema
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
        NodoFecha(Fecha f, NodoFecha* sig = nullptr) : fecha(f), siguiente(sig) {}
    };
    NodoFecha* fechasReservadas;

public:
    Alojamiento(const char* cod, const char* nom, const char* dir, const char* mun,
                const char* dep, char tip, float precio, Anfitrion* anf)
        : tipo(tip), precioNoche(precio), anfitrion(anf), fechasReservadas(nullptr) {
        codigo = new char[strlen(cod) + 1]; strcpy(codigo, cod);
        nombre = new char[strlen(nom) + 1]; strcpy(nombre, nom);
        direccion = new char[strlen(dir) + 1]; strcpy(direccion, dir);
        municipio = new char[strlen(mun) + 1]; strcpy(municipio, mun);
        departamento = new char[strlen(dep) + 1]; strcpy(departamento, dep);
        for(int i = 0; i < 7; i++) amenidades[i] = false;
    }

    bool estaDisponible(Fecha inicio, int noches) const;
    void agregarReserva(Fecha fecha);
    void mostrarInfo() const;
    void mostrarReservaciones(Fecha inicio, Fecha fin) const;

    const char* getCodigo() const { return codigo; }
    const char* getNombre() const { return nombre; }
    const char* getDireccion() const { return direccion; }
    const char* getMunicipio() const { return municipio; }
    const char* getDepartamento() const { return departamento; }
    char getTipo() const { return tipo; }
    float getPrecio() const { return precioNoche; }
    Anfitrion* getAnfitrion() const { return anfitrion; }
    bool tieneAmenidad(int indice) const { return amenidades[indice]; }
    void setAmenidad(int indice, bool valor) { amenidades[indice] = valor; }

    ~Alojamiento() {
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
};

// Clase Reservacion
// Clase que representa una reservación entre un huésped y un alojamiento
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
                int dur, const char* metodo, float mont, const char* notas)
        : huesped(h), alojamiento(a), fechaEntrada(fecha), duracion(dur), monto(mont) {
        codigo = new char[strlen(cod) + 1]; strcpy(codigo, cod);
        strncpy(metodoPago, metodo, 19); metodoPago[19] = '\0';
        anotaciones = new char[strlen(notas) + 1]; strcpy(anotaciones, notas);
    }

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

    ~Reservacion() {
        delete[] codigo;
        delete[] anotaciones;
    }
};

// Clase SistemaUdeAStay
// Clase central que administra todo el sistema: usuarios, alojamientos y reservaciones
class SistemaUdeAStay {
private:
    struct NodoUsuario {
        Usuario* usuario;
        NodoUsuario* siguiente;
        NodoUsuario(Usuario* u, NodoUsuario* sig = nullptr) : usuario(u), siguiente(sig) {}
    };
    NodoUsuario* usuarios;

    struct NodoAlojamiento {
        Alojamiento* alojamiento;
        NodoAlojamiento* siguiente;
        NodoAlojamiento(Alojamiento* a, NodoAlojamiento* sig = nullptr) : alojamiento(a), siguiente(sig) {}
    };
    NodoAlojamiento* alojamientos;

    struct NodoReserva {
        Reservacion* reservacion;
        NodoReserva* siguiente;
        NodoReserva(Reservacion* r, NodoReserva* sig = nullptr) : reservacion(r), siguiente(sig) {}
    };
    NodoReserva* reservaciones;
    NodoReserva* historico;

    unsigned long iteraciones;

public:
    SistemaUdeAStay() : usuarios(nullptr), alojamientos(nullptr),
        reservaciones(nullptr), historico(nullptr), iteraciones(0) {}

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
            NodoReserva* temp = reservaciones;
            reservaciones = reservaciones->siguiente;
            delete temp->reservacion;
            delete temp;
        }

        while(historico) {
            NodoReserva* temp = historico;
            historico = historico->siguiente;
            delete temp->reservacion;
            delete temp;
        }
    }

    void agregarUsuario(Usuario* u) {
        usuarios = new NodoUsuario(u, usuarios);
    }

    void agregarAlojamiento(Alojamiento* a) {
        alojamientos = new NodoAlojamiento(a, alojamientos);
    }

    void agregarReservacion(Reservacion* r) {
        reservaciones = new NodoReserva(r, reservaciones);
    }

    void eliminarReservacion(const char* codigo) {
        NodoReserva* actual = reservaciones;
        NodoReserva* anterior = nullptr;

        while(actual) {
            if(strcmp(actual->reservacion->getCodigo(), codigo) == 0) {
                if(anterior) {
                    anterior->siguiente = actual->siguiente;
                } else {
                    reservaciones = actual->siguiente;
                }
                delete actual->reservacion;
                delete actual;
                return;
            }
            anterior = actual;
            actual = actual->siguiente;
        }
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

    Reservacion* buscarReservacion(const char* codigo) {
        NodoReserva* actual = reservaciones;
        while(actual) {
            if(strcmp(actual->reservacion->getCodigo(), codigo) == 0) {
                return actual->reservacion;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    void mostrarAlojamientosDisponibles(Fecha inicio, int noches, const char* municipio,
                                        float precioMax, float puntuacionMin) {
        cout << "\n=== ALOJAMIENTOS DISPONIBLES ===\n";
        NodoAlojamiento* actual = alojamientos;
        int contador = 0;

        while(actual) {
            bool cumpleFiltros = true;

            if(municipio && strcmp(actual->alojamiento->getMunicipio(), municipio) != 0) {
                cumpleFiltros = false;
            }
            if(precioMax > 0 && actual->alojamiento->getPrecio() > precioMax) {
                cumpleFiltros = false;
            }
            if(actual->alojamiento->getAnfitrion()->getPuntuacion() < puntuacionMin) {
                cumpleFiltros = false;
            }

            if(cumpleFiltros && actual->alojamiento->estaDisponible(inicio, noches)) {
                cout << ++contador << ". ";
                actual->alojamiento->mostrarInfo();
                cout << "-----------------\n";
            }
            actual = actual->siguiente;
        }

        if(contador == 0) {
            cout << "No se encontraron alojamientos disponibles con los criterios especificados.\n";
        }
    }

    void moverReservacionesAHistorico(Fecha fechaCorte) {
        iteraciones = 0;
        NodoReserva* actual = reservaciones;
        NodoReserva* anterior = nullptr;

        while(actual) {
            iteraciones++;
            if(actual->reservacion->getFechaEntrada() < fechaCorte) {
                if(anterior) {
                    anterior->siguiente = actual->siguiente;
                } else {
                    reservaciones = actual->siguiente;
                }

                actual->siguiente = historico;
                historico = actual;

                actual = anterior ? anterior->siguiente : reservaciones;
            } else {
                anterior = actual;
                actual = actual->siguiente;
            }
        }
        guardarTXT();
        cout << "Reservaciones históricas actualizadas. Iteraciones: " << iteraciones << "\n";
    }

    bool validarDisponibilidad(Huesped* huesped, Fecha inicio, int noches) {
        if (!huesped) return false;

        Huesped::NodoReserva* actual = huesped->getReservaciones();
        while(actual) {
            if(actual->reserva->coincideConFecha(inicio)) {
                return false;
            }
            actual = actual->siguiente;
        }
        return true;
    }

    void mostrarEstadisticas() const {
        cout << "\n=== ESTADÍSTICAS DEL SISTEMA ===\n";

        int totalUsuarios = 0;
        int totalAnfitriones = 0;
        int totalHuespedes = 0;
        NodoUsuario* actualUser = usuarios;
        while(actualUser) {
            totalUsuarios++;
            if(strcmp(actualUser->usuario->getTipo(), "ANFITRION") == 0) {
                totalAnfitriones++;
            } else {
                totalHuespedes++;
            }
            actualUser = actualUser->siguiente;
        }

        int totalAlojamientos = 0;
        NodoAlojamiento* actualAloj = alojamientos;
        while(actualAloj) {
            totalAlojamientos++;
            actualAloj = actualAloj->siguiente;
        }

        int totalReservaciones = 0;
        NodoReserva* actualRes = reservaciones;
        while(actualRes) {
            totalReservaciones++;
            actualRes = actualRes->siguiente;
        }

        int totalHistorico = 0;
        actualRes = historico;
        while(actualRes) {
            totalHistorico++;
            actualRes = actualRes->siguiente;
        }

        cout << "Total usuarios: " << totalUsuarios << "\n";
        cout << "  - Anfitriones: " << totalAnfitriones << "\n";
        cout << "  - Huéspedes: " << totalHuespedes << "\n";
        cout << "Total alojamientos: " << totalAlojamientos << "\n";
        cout << "Total reservaciones activas: " << totalReservaciones << "\n";
        cout << "Total reservaciones históricas: " << totalHistorico << "\n";

        size_t memoria = totalUsuarios * sizeof(Usuario) +
                         totalAnfitriones * sizeof(Anfitrion) +
                         totalHuespedes * sizeof(Huesped) +
                         totalAlojamientos * sizeof(Alojamiento) +
                         (totalReservaciones + totalHistorico) * sizeof(Reservacion);

        cout << "Memoria aproximada utilizada: " << memoria / 1024 << " KB\n";
    }

    void cargarTXT() {
        ifstream archivo(ARCHIVO_DATOS);
        if(!archivo.is_open()) {
            cerr << "No se encontró archivo de datos, comenzando con sistema vacío\n";
            return;
        }

        string linea;
        string seccionActual;

        while(getline(archivo, linea)) {
            if(linea.empty() || linea.find("===") != string::npos)
                continue;

            if(linea.find("--- USUARIOS ---") != string::npos) {
                seccionActual = "USUARIOS";
                getline(archivo, linea);
                continue;
            }
            else if(linea.find("--- ALOJAMIENTOS ---") != string::npos) {
                seccionActual = "ALOJAMIENTOS";
                getline(archivo, linea);
                continue;
            }
            else if(linea.find("--- RESERVACIONES") != string::npos) {
                seccionActual = "RESERVACIONES";
                getline(archivo, linea);
                continue;
            }

            if(seccionActual == "USUARIOS") {
                size_t pos1 = linea.find('\t');
                size_t pos2 = linea.find('\t', pos1+1);
                size_t pos3 = linea.find('\t', pos2+1);

                string tipo = linea.substr(0, pos1);
                string doc = linea.substr(pos1+1, pos2-pos1-1);
                int ant = stoi(linea.substr(pos2+1, pos3-pos2-1));
                float punt = stof(linea.substr(pos3+1));

                if(tipo == "ANFITRION") {
                    agregarUsuario(new Anfitrion(doc.c_str(), ant, punt));
                } else {
                    agregarUsuario(new Huesped(doc.c_str(), ant, punt));
                }
            }
            else if(seccionActual == "ALOJAMIENTOS") {
                // Implementación similar para alojamientos
            }
            else if(seccionActual == "RESERVACIONES") {
                // Implementación similar para reservaciones
            }
        }

        archivo.close();
        cout << "Datos cargados desde " << ARCHIVO_DATOS << endl;
    }

    void guardarTXT() {
        ofstream archivo(ARCHIVO_DATOS);
        if(!archivo.is_open()) {
            cerr << "Error al crear archivo TXT\n";
            return;
        }

        archivo << "=== SISTEMA UdeAStay - DATOS GUARDADOS ===\n\n";
        archivo << "--- USUARIOS ---\n";
        archivo << "TIPO\tDOCUMENTO\tANTIGÜEDAD\tPUNTUACIÓN\n";

        NodoUsuario* actualUser = usuarios;
        while(actualUser) {
            archivo << actualUser->usuario->getTipo() << "\t"
                    << actualUser->usuario->getDocumento() << "\t"
                    << actualUser->usuario->getAntiguedad() << "\t"
                    << actualUser->usuario->getPuntuacion() << "\n";
            actualUser = actualUser->siguiente;
        }

        archivo << "\n--- ALOJAMIENTOS ---\n";
        archivo << "CÓDIGO\tNOMBRE\tTIPO\tPRECIO\tANFITRIÓN\n";

        NodoAlojamiento* actualAloj = alojamientos;
        while(actualAloj) {
            archivo << actualAloj->alojamiento->getCodigo() << "\t"
                    << actualAloj->alojamiento->getNombre() << "\t"
                    << actualAloj->alojamiento->getTipo() << "\t"
                    << actualAloj->alojamiento->getPrecio() << "\t"
                    << actualAloj->alojamiento->getAnfitrion()->getDocumento() << "\n";
            actualAloj = actualAloj->siguiente;
        }

        archivo << "\n--- RESERVACIONES ACTIVAS ---\n";
        archivo << "CÓDIGO\tHUÉSPED\tALOJAMIENTO\tFECHA\tDURACIÓN\n";

        NodoReserva* actualRes = reservaciones;
        while(actualRes) {
            Fecha fecha = actualRes->reservacion->getFechaEntrada();
            archivo << actualRes->reservacion->getCodigo() << "\t"
                    << actualRes->reservacion->getHuesped()->getDocumento() << "\t"
                    << actualRes->reservacion->getAlojamiento()->getCodigo() << "\t"
                    << fecha.dia << "/" << fecha.mes << "/" << fecha.ano << "\t"
                    << actualRes->reservacion->getDuracion() << "\n";
            actualRes = actualRes->siguiente;
        }

        archivo << "\n--- HISTÓRICO DE RESERVACIONES ---\n";
        actualRes = historico;
        while(actualRes) {
            Fecha fecha = actualRes->reservacion->getFechaEntrada();
            archivo << actualRes->reservacion->getCodigo() << "\t"
                    << actualRes->reservacion->getHuesped()->getDocumento() << "\t"
                    << actualRes->reservacion->getAlojamiento()->getCodigo() << "\t"
                    << fecha.dia << "/" << fecha.mes << "/" << fecha.ano << "\t"
                    << actualRes->reservacion->getDuracion() << "\n";
            actualRes = actualRes->siguiente;
        }

        archivo.close();
        cout << "Datos guardados en " << ARCHIVO_DATOS << endl;
    }

    void mostrarMenuPrincipal() {
        int opcion;
        do {
            cout << "\n=== MENÚ PRINCIPAL UdeAStay ===\n";
            cout << "1. Iniciar sesión\n";
            cout << "2. Guardar datos\n";
            cout << "3. Mostrar estadísticas\n";
            cout << "4. Salir\n";
            cout << "Opción: ";
            cin >> opcion;
            cin.ignore();

            switch(opcion) {
            case 1: {
                char documento[20];
                cout << "Ingrese su documento: ";
                cin.getline(documento, 20);

                Usuario* user = buscarUsuario(documento);
                if(user) {
                    user->mostrarMenu(*this);
                } else {
                    cout << "Usuario no encontrado.\n";
                }
                break;
            }
            case 2:
                guardarTXT();
                break;
            case 3:
                mostrarEstadisticas();
                break;
            case 4:
                cout << "Saliendo del sistema...\n";
                break;
            default:
                cout << "Opción inválida.\n";
            }
        } while(opcion != 4);
    }
};

// Implementaciones de métodos fuera de las clases

// Método para verificar si un alojamiento está disponible en un rango de fechas
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

// Agrega una nueva fecha de reserva a un alojamiento (lista enlazada)
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

// Verifica si una fecha dada cae dentro del rango de una reservación activa
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

// Lógica para que un huésped consulte disponibilidad y realice una reserva
void Huesped::reservarAlojamiento(SistemaUdeAStay& sistema) {
    Fecha fecha;
    int noches;
    char municipio[50] = "";
    float precioMax = 0;
    float puntuacionMin = 0;

    cout << "\n=== NUEVA RESERVA ===\n";
    cout << "Fecha de entrada (dd mm aaaa): ";
    cin >> fecha.dia >> fecha.mes >> fecha.ano;
    cout << "Número de noches: ";
    cin >> noches;
    cin.ignore();
    cout << "Municipio (opcional, dejar vacío para cualquier municipio): ";
    cin.getline(municipio, 50);

    cout << "Filtros adicionales:\n";
    cout << "Precio máximo por noche (0 para cualquier precio): ";
    cin >> precioMax;
    cout << "Puntuación mínima del anfitrión (0-5): ";
    cin >> puntuacionMin;

    if(!sistema.validarDisponibilidad(this, fecha, noches)) {
        cout << "Ya tiene una reserva en esas fechas. No puede realizar otra.\n";
        return;
    }

    sistema.mostrarAlojamientosDisponibles(fecha, noches,
                                           (municipio[0] ? municipio : nullptr),
                                           precioMax, puntuacionMin);

    char codigoAlojamiento[20];
    cout << "Ingrese el código del alojamiento a reservar (o 0 para cancelar): ";
    cin >> codigoAlojamiento;

    if(strcmp(codigoAlojamiento, "0") == 0) {
        return;
    }

    Alojamiento* alojamiento = sistema.buscarAlojamiento(codigoAlojamiento);
    if(!alojamiento || !alojamiento->estaDisponible(fecha, noches)) {
        cout << "Alojamiento no disponible para las fechas seleccionadas.\n";
        return;
    }

    char metodoPago[20];
    char anotaciones[1000];

    cout << "Método de pago (PSE/TC): ";
    cin >> metodoPago;
    cin.ignore();

    cout << "Anotaciones para el anfitrión (hasta 1000 caracteres): ";
    cin.getline(anotaciones, 1000);

    char codigoReserva[20];
    sprintf(codigoReserva, "RES-%03d", rand() % 1000);

    float montoTotal = alojamiento->getPrecio() * noches;

    Reservacion* nuevaReserva = new Reservacion(
        codigoReserva, this, alojamiento, fecha,
        noches, metodoPago, montoTotal, anotaciones
        );

    sistema.agregarReservacion(nuevaReserva);
    this->agregarReserva(nuevaReserva);
    alojamiento->agregarReserva(fecha);

    cout << "\n=== RESERVA CONFIRMADA ===\n";
    nuevaReserva->mostrarComprobante();
}

// Permite al huésped anular una de sus reservaciones activas
void Huesped::anularReservacion(SistemaUdeAStay& sistema) {
    if(!reservaciones) {
        cout << "No tiene reservaciones activas.\n";
        return;
    }

    cout << "\n=== ANULAR RESERVA ===\n";
    mostrarReservaciones();

    char codigo[20];
    cout << "Ingrese el código de reserva a anular: ";
    cin >> codigo;

    NodoReserva* actual = reservaciones;
    NodoReserva* anterior = nullptr;

    while(actual) {
        if(strcmp(actual->reserva->getCodigo(), codigo) == 0) {
            sistema.eliminarReservacion(codigo);

            if(anterior) {
                anterior->siguiente = actual->siguiente;
            } else {
                reservaciones = actual->siguiente;
            }

            delete actual->reserva;
            delete actual;
            cout << "Reserva anulada exitosamente.\n";
            return;
        }
        anterior = actual;
        actual = actual->siguiente;
    }

    cout << "No se encontró la reserva con ese código.\n";
}

void Huesped::mostrarMenu(SistemaUdeAStay& sistema) {
    int opcion;
    do {
        cout << "\n=== MENU HUESPED ===\n";
        cout << "1. Ver mis reservaciones\n";
        cout << "2. Reservar alojamiento\n";
        cout << "3. Anular reservación\n";
        cout << "4. Salir\n";
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore();

        switch(opcion) {
        case 1:
            mostrarReservaciones();
            break;
        case 2:
            reservarAlojamiento(sistema);
            break;
        case 3:
            anularReservacion(sistema);
            break;
        case 4:
            cout << "Saliendo del menú de huésped...\n";
            break;
        default:
            cout << "Opción inválida.\n";
        }
    } while(opcion != 4);
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

void Anfitrion::anularReservacion(SistemaUdeAStay& sistema) {
    char codigo[20];
    cout << "Ingrese el código de reserva a anular: ";
    cin >> codigo;

    NodoAlojamiento* actualAloj = alojamientos;
    while(actualAloj) {
        Reservacion* reserva = sistema.buscarReservacion(codigo);
        if(reserva && reserva->getAlojamiento() == actualAloj->alojamiento) {
            sistema.eliminarReservacion(codigo);
            cout << "Reserva anulada exitosamente.\n";
            return;
        }
        actualAloj = actualAloj->siguiente;
    }

    cout << "No se encontro una reserva valida para anular.\n";
}

// El anfitrión solicita al sistema mover reservaciones pasadas al histórico
void Anfitrion::actualizarHistorico(SistemaUdeAStay& sistema) {
    Fecha fechaCorte;
    cout << "\n=== ACTUALIZAR HISTORICO ===\n";
    cout << "Ingrese fecha de corte (dd mm aaaa): ";
    cin >> fechaCorte.dia >> fechaCorte.mes >> fechaCorte.ano;

    sistema.moverReservacionesAHistorico(fechaCorte);
    cout << "Historico actualizado exitosamente.\n";
}

void Anfitrion::mostrarMenu(SistemaUdeAStay& sistema) {
    int opcion;
    do {
        cout << "\n=== MENÚ ANFITRIÓN ===\n";
        cout << "1. Ver mis alojamientos\n";
        cout << "2. Consultar reservaciones\n";
        cout << "3. Anular reservacion\n";
        cout << "4. Actualizar historico\n";
        cout << "5. Salir\n";
        cout << "Opción: ";
        cin >> opcion;
        cin.ignore();

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
        case 3:
            anularReservacion(sistema);
            break;
        case 4:
            actualizarHistorico(sistema);
            break;
        case 5:
            cout << "Saliendo del menú de anfitrion...\n";
            break;
        default:
            cout << "Opción invalida.\n";
        }
    } while(opcion != 5);
}

// Función principal
// Función principal: inicializa el sistema, carga datos y muestra el menú
int main() {
    srand(time(nullptr)); // Inicializar semilla para códigos de reserva

    SistemaUdeAStay sistema;
    sistema.cargarTXT();
    sistema.mostrarMenuPrincipal();

    return 0;
}
