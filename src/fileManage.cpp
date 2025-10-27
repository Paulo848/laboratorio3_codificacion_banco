#include "fileManage.h"
#include "decode.h"
#include "encodec.h"
#include <fstream>

// ---- helpers locales ----
static int c_strlen(const char* s) {
    if (!s) return 0;
    int n = 0;
    while (s[n] != '\0') ++n;
    return n;
}

static int c_isdigit(char c) {
    return c >= '0' && c <= '9';
}
//---

bool leerPlanoCodificado(const char* nombre, int metodo, int semilla,
                         unsigned char*& outBuf, int& outLen)
{
    outBuf = nullptr;
    outLen = 0;

    if (!nombre || semilla <= 0 || (metodo != 1 && metodo != 2))
        return false;

    std::ifstream f(nombre, std::ios::binary);
    if (!f) return false;

    f.seekg(0, std::ios::end);
    const std::streamsize sz = f.tellg();
    if (sz < 0) return false;
    f.seekg(0, std::ios::beg);

    const int len = static_cast<int>(sz);
    if (len == 0) {
        outBuf = nullptr;
        outLen = 0;
        return true;
    }

    unsigned char* cod = new unsigned char[len];
    f.read(reinterpret_cast<char*>(cod), len);
    const bool okRead = (f.gcount() == sz);
    f.close();
    if (!okRead) { delete[] cod; return false; }

    unsigned char* plano = nullptr;
    if (metodo == 1) plano = de_metodo_1(semilla, cod, len);
    else             plano = de_metodo_2(semilla, cod, len);

    delete[] cod;

    if (!plano) return false;

    outBuf = plano;
    outLen = len;
    return true;
}

bool escribirPlanoCodificado(const char* nombre, int metodo, int semilla,
                             const unsigned char* inBuf, int inLen)
{
    if (!nombre || !inBuf || inLen < 0 || semilla <= 0 || (metodo != 1 && metodo != 2))
        return false;

    unsigned char* cod = nullptr;
    if (metodo == 1) cod = metodo_1(semilla, inBuf, static_cast<std::size_t>(inLen));
    else             cod = metodo_2(semilla, inBuf, static_cast<std::size_t>(inLen));

    if (!cod) return false;

    std::ofstream f(nombre, std::ios::binary);
    if (!f) { delete[] cod; return false; }

    f.write(reinterpret_cast<const char*>(cod), inLen);
    const bool okWrite = f.good();
    f.close();

    delete[] cod;
    return okWrite;
}

bool appendLineaCodificada(const char* nombre, int metodo, int semilla,
                           const char* linea)
/*
 * appendLineaCodificada: 1.decodifica 2.agrega \n si falta 3.junta todo 4.codifica y sobrescribe
 */
{
    if (!nombre || !linea || semilla <= 0 || (metodo != 1 && metodo != 2))
        return false;

    // Leer plano decodificado (si no existe, tratamos como vacío)
    unsigned char* plano = 0;
    int lenPlano = 0;
    {
        std::ifstream test(nombre, std::ios::binary);
        if (test.good()) {
            test.close();
            if (!leerPlanoCodificado(nombre, metodo, semilla, plano, lenPlano)) {
                // podría estar vacío/corrupto; seguimos como vacío
                plano = 0;
                lenPlano = 0;
            }
        } else {
            // no existe: lo trataremos como vacío
            test.close();
            plano = 0;
            lenPlano = 0;
        }
    }

    // Preparar nueva longitud:

    const int lenLinea = c_strlen(linea);
    // si hay salto de linea al final de transacciones.csv
    int extraSaltoLinea = 0;
    if (lenPlano > 0) {
        unsigned char last = plano[lenPlano - 1];
        if (last != '\n') extraSaltoLinea = 1;
    }
    const int nuevoLen = lenPlano + extraSaltoLinea + lenLinea;

    unsigned char* nuevo = new unsigned char[nuevoLen];
    int pos = 0;

    // Copiar el archivo viejo
    for (int i = 0; i < lenPlano; ++i) nuevo[pos++] = plano[i];

    // Agregar '\n' si faltaba y el archivo no estaba vacío
    if (extraSaltoLinea) nuevo[pos++] = '\n';

    // Copiar linea nueva
    for (int i = 0; i < lenLinea; ++i) nuevo[pos++] = (unsigned char)linea[i];

    const bool resultado = escribirPlanoCodificado(nombre, metodo, semilla, nuevo, nuevoLen);

    if (plano) delete[] plano;
    delete[] nuevo;

    return resultado;
}

bool crearSudoConEncabezado(const char* nombreSudo, int metodo, int semilla)
{
    if (!nombreSudo || semilla <= 0 || (metodo != 1 && metodo != 2)) return false;

    // Si existe y tiene contenido, no hacemos nada.
    std::ifstream f(nombreSudo, std::ios::binary);
    if (f) {
        f.seekg(0, std::ios::end);
        if (f.tellg() > 0) return true; // ya existe con contenido
    }
    // Crear con encabezado
    const char* header = "cedula,clave";
    const int len = c_strlen(header);

    // Copia a unsigned char para escribir codificado
    unsigned char* buf = new unsigned char[len];
    for (int i = 0; i < len; ++i) buf[i] = (unsigned char)header[i];

    const bool ok = escribirPlanoCodificado(nombreSudo, metodo, semilla, buf, len);
    delete[] buf;
    return ok;
}

bool crearUsuariosConEncabezado(const char* nombreUsuarios, int metodo, int semilla)
{
    if (!nombreUsuarios || semilla <= 0 || (metodo != 1 && metodo != 2)) return false;

    std::ifstream f(nombreUsuarios, std::ios::binary);
    if (f) {
        f.seekg(0, std::ios::end);
        if (f.tellg() > 0) return true;
    }
    const char* header = "cedula,clave,saldo";
    const int len = c_strlen(header);

    unsigned char* buf = new unsigned char[len];
    for (int i = 0; i < len; ++i) buf[i] = (unsigned char)header[i];

    const bool ok = escribirPlanoCodificado(nombreUsuarios, metodo, semilla, buf, len);
    delete[] buf;
    return ok;
}

bool crearTransaccionesConEncabezado(const char* nombreTx, int metodo, int semilla)
{
    if (!nombreTx || semilla <= 0 || (metodo != 1 && metodo != 2)) return false;

    std::ifstream f(nombreTx, std::ios::binary);
    if (f) {
        f.seekg(0, std::ios::end);
        if (f.tellg() > 0) return true;
    }
    const char* header = "id,cedula,tipo,costo,monto,saldoAntes,saldoDespues";
    const int len = c_strlen(header);

    unsigned char* buf = new unsigned char[len];
    for (int i = 0; i < len; ++i) buf[i] = (unsigned char)header[i];

    const bool ok = escribirPlanoCodificado(nombreTx, metodo, semilla, buf, len);
    delete[] buf;
    return ok;
}

bool cargarAdmins(const char* nombreSudo, int metodo, int semilla,
                  char ced[][CED_BUF], char pass[][PASS_BUF],
                  int maxAdmins, int* nAdmins)
{
    if (!nombreSudo || !nAdmins || maxAdmins <= 0 || semilla <= 0 || (metodo != 1 && metodo != 2))
        return false;

    *nAdmins = 0;

    unsigned char* plano = 0;
    int len = 0;
    if (!leerPlanoCodificado(nombreSudo, metodo, semilla, plano, len)) {
        if (plano) delete[] plano;
        return false;
    }
    if (len <= 0) { if (plano) delete[] plano; return true; }

    int i = 0;

    // Saltar encabezado (hasta '\n' si existe)
    while (i < len && plano[i] != '\n') ++i;
    if (i < len && plano[i] == '\n') ++i;

    bool truncated = false;

    // Parseo línea por línea
    while (i < len) {
        if (plano[i] == '\n') { ++i; continue; }

        // Campo 1: cedula (hasta ',')
        char cedTmp[CED_BUF];
        int cedLen = 0; bool cedOverflow = false; bool lineaInvalida = false;

        while (i < len && plano[i] != ',' && plano[i] != '\n') {
            unsigned char ch = plano[i];
            if (ch != '\r') {
                if (cedLen < (CED_BUF - 1)) cedTmp[cedLen++] = (char)ch;
                else cedOverflow = true;
            }
            ++i;
        }
        cedTmp[cedLen] = '\0';

        if (i >= len || plano[i] != ',') {
            // línea sin segunda columna -> descartar hasta fin de línea
            while (i < len && plano[i] != '\n') ++i;
            if (i < len && plano[i] == '\n') ++i;
            continue;
        }
        ++i; // saltar ','

        // Campo 2: clave (hasta fin de línea o EOF)
        char passTmp[PASS_BUF];
        int passLen = 0; bool passOverflow = false;

        while (i < len && plano[i] != '\n') {
            unsigned char ch = plano[i];
            if (ch == ',') { lineaInvalida = true; } // solo 2 columnas válidas
            if (ch != '\r') {
                if (passLen < (PASS_BUF - 1)) passTmp[passLen++] = (char)ch;
                else passOverflow = true;
            }
            ++i;
        }
        if (i < len && plano[i] == '\n') ++i;
        passTmp[passLen] = '\0';

        if (cedLen == 0 || passLen == 0 || cedOverflow || passOverflow || lineaInvalida) {
            // descartar registro inválido
            continue;
        }

        if (*nAdmins < maxAdmins) {
            // copiar a RAM
            for (int k = 0; k <= cedLen; ++k)  ced[*nAdmins][k]  = cedTmp[k];
            for (int k = 0; k <= passLen; ++k) pass[*nAdmins][k] = passTmp[k];
            (*nAdmins)++;
        } else {
            truncated = true; // capacidad agotada
            // seguimos consumiendo por si quieres contabilizar errores, etc.
        }
    }

    if (plano) delete[] plano;
    return !truncated; // false si truncó por capacidad
}

bool cargarUsuarios(const char* nombreUsuarios, int metodo, int semilla,
                    char ced[][CED_BUF], char pass[][PASS_BUF], long* saldo,
                    int maxUsers, int* nUsers)
{
    if (!nombreUsuarios || !nUsers || !saldo || maxUsers <= 0 || semilla <= 0 || (metodo != 1 && metodo != 2))
        return false;

    *nUsers = 0;

    unsigned char* plano = 0;
    int len = 0;
    if (!leerPlanoCodificado(nombreUsuarios, metodo, semilla, plano, len)) {
        if (plano) delete[] plano;
        return false;
    }
    if (len <= 0) { if (plano) delete[] plano; return true; }

    int i = 0;

    // Saltar encabezado (hasta '\n' si existe)
    while (i < len && plano[i] != '\n') ++i;
    if (i < len && plano[i] == '\n') ++i;

    bool truncated = false;

    while (i < len) {
        if (plano[i] == '\n') { ++i; continue; }

        // Campo 1: cedula (hasta ',')
        char cedTmp[CED_BUF];
        int cedLen = 0; bool cedOverflow = false; bool lineaInvalida = false;

        while (i < len && plano[i] != ',' && plano[i] != '\n') {
            unsigned char ch = plano[i];
            if (ch != '\r') {
                if (cedLen < (CED_BUF - 1)) cedTmp[cedLen++] = (char)ch;
                else cedOverflow = true;
            }
            ++i;
        }
        cedTmp[cedLen] = '\0';
        if (i >= len || plano[i] != ',') {
            while (i < len && plano[i] != '\n') ++i;
            if (i < len && plano[i] == '\n') ++i;
            continue;
        }
        ++i; // saltar ','

        // Campo 2: clave (hasta ',')
        char passTmp[PASS_BUF];
        int passLen = 0; bool passOverflow = false;

        while (i < len && plano[i] != ',' && plano[i] != '\n') {
            unsigned char ch = plano[i];
            if (ch != '\r') {
                if (passLen < (PASS_BUF - 1)) passTmp[passLen++] = (char)ch;
                else passOverflow = true;
            }
            ++i;
        }
        passTmp[passLen] = '\0';
        if (i >= len || plano[i] != ',') {
            while (i < len && plano[i] != '\n') ++i;
            if (i < len && plano[i] == '\n') ++i;
            continue;
        }
        ++i; // saltar ','

        // Campo 3: saldo (hasta fin de línea o EOF) -> solo dígitos
        long saldoVal = 0; bool hasDigits = false; bool saldoInvalido = false;
        while (i < len && plano[i] != '\n') {
            unsigned char ch = plano[i];
            if (ch == ',') { lineaInvalida = true; } // demasiadas columnas
            else if (ch != '\r') {
                if (c_isdigit((char)ch)) {
                    hasDigits = true;
                    // acumulación simple; sin control de overflow para el lab
                    saldoVal = saldoVal * 10 + (long)((char)ch - '0');
                } else {
                    saldoInvalido = true;
                }
            }
            ++i;
        }
        if (i < len && plano[i] == '\n') ++i;

        if (cedLen == 0 || passLen == 0 || !hasDigits || cedOverflow || passOverflow || saldoInvalido || lineaInvalida) {
            // descartar línea inválida
            continue;
        }

        if (*nUsers < maxUsers) {
            // copiar a RAM
            for (int k = 0; k <= cedLen;  ++k) ced[*nUsers][k]  = cedTmp[k];
            for (int k = 0; k <= passLen; ++k) pass[*nUsers][k] = passTmp[k];
            saldo[*nUsers] = saldoVal;
            (*nUsers)++;
        } else {
            truncated = true;
        }
    }

    if (plano) delete[] plano;
    return !truncated; // false si truncó por capacidad
}

long cargarNextIdTx(const char* nombreTx, int metodo, int semilla)
/*
 * cargarNextIdTx: lee transacciones.csv decodificado y devuelve maxId+1 (o 1 si vacío)
 * Formato esperado por línea:  id,cedula,tipo,costo,monto,saldoAntes,saldoDespues
 */
{
    if (!nombreTx || semilla <= 0 || (metodo != 1 && metodo != 2))
        return 1;

    unsigned char* plano = 0;
    int len = 0;
    {
        std::ifstream test(nombreTx, std::ios::binary);
        if (!test.good()) { test.close(); return 1; }
        test.close();
    }
    if (!leerPlanoCodificado(nombreTx, metodo, semilla, plano, len) || len <= 0) {
        if (plano) delete[] plano;
        return 1;
    }

    long maxId = 0;
    int i = 0;

    // Saltar encabezado (primera línea). Busco el primer '\n'
    while (i < len && plano[i] != '\n') ++i;
    // mover al inicio de la primera línea de datos
    if (i < len && plano[i] == '\n') ++i;

    // Recorrer líneas y parsear el primer campo (id) hasta la coma
    while (i < len) {
        // Si es fin de archivo o línea vacía
        if (plano[i] == '\n') { ++i; continue; }

        // Parsear ID: secuencia de dígitos al inicio hasta coma
        // almacena el valor del iD si es uno valido
        long id = 0;
        // bandera si existen digitos
        bool hasDigits = false;
        // auxiliar para recorrer el arreglo sin alterar inoportunamente i
        int j = i;

        while (j < len && plano[j] != '\n' && plano[j] != ',') {
            if (!c_isdigit((char)plano[j]))
            {
                /*línea inválida:
                 * si no es un digito, sigue hasta el final de la linea
                 * procede a salir del ciclo sin entrar a hasDigits = true
                */
                while (j < len && plano[j] != '\n') ++j;
                break;
            }
            hasDigits = true;
            id = id * 10 + (long)((char)plano[j] - '0');
            ++j;
        }

        // Si terminó por coma y hubo dígitos, considerar este id
        if (j < len && plano[j] == ',' && hasDigits)
        {
            // si el id es el maximo hasta el momento lo guarda
            if (id > maxId) maxId = id;

            // -- Saltamos a fin de linea porque ya no sirve la demas info
            while (j < len && plano[j] != '\n') ++j;
            if (j < len && plano[j] == '\n') ++j;
            i = j;
        } else {
            // -- Id invalido y saltamos a fin de linea porque ya no sirve la demas info

            while (j < len && plano[j] != '\n') ++j;
            if (j < len && plano[j] == '\n') ++j;
            i = j;
        }
    }

    if (plano) delete[] plano;
    return (maxId <= 0) ? 1 : (maxId + 1);
}
