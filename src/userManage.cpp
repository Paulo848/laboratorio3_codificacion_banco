#include <iostream>
#include <climits>
#include "fileManage.h"
#include "userManage.h"

static inline bool is_space(char c){ return c==' ' || c=='\t' || c=='\r' || c=='\n' || c=='\v' || c=='\f'; }
static inline bool is_digit(char c){ return c>='0' && c<='9'; }
static inline bool is_upper(char c){ return c>='A' && c<='Z'; }
static inline bool is_lower(char c){ return c>='a' && c<='z'; }
static inline bool is_alpha(char c){ return is_upper(c) || is_lower(c); }
static inline bool is_printable(char c){ return c >= 32 && c <= 126; }
static int fm_strlen(const char* s) {
    if (!s) return 0;
    int n = 0; while (s[n] != '\0') ++n; return n;
}

static int fm_contains_forbidden(const char* s) {
    if (!s) return 0;
    for (int i = 0; s[i] != '\0'; ++i) {
        char c = s[i];
        if (c == ',' || c == '\n' || c == '\r') return 1;
    }
    return 0;
}


static int fm_write_uint(unsigned long v, char* dst, int cap) {
    if (cap <= 0) return -1;
    if (v == 0) {
        if (cap < 1) return -1;
        dst[0] = '0';
        return 1;
    }
    char tmp[32]; int t = 0;
    while (v > 0 && t < 32) { tmp[t++] = (char)('0' + (v % 10)); v /= 10; }
    if (t > cap) return -1;
    for (int i = 0; i < t; ++i) dst[i] = tmp[t - 1 - i];
    return t;
}

static int fm_append_str(char* out, int outCap, int* ppos, const char* s) {
    int pos = *ppos;
    int len = fm_strlen(s);
    if (pos + len >= outCap) return 0;
    for (int i = 0; i < len; ++i) out[pos++] = s[i];
    *ppos = pos;
    return 1;
}

static int fm_append_char(char* out, int outCap, int* ppos, char c) {
    int pos = *ppos;
    if (pos + 1 >= outCap) return 0;
    out[pos++] = c;
    *ppos = pos;
    return 1;
}

static int fm_append_uint(char* out, int outCap, int* ppos, unsigned long v) {
    char buf[32];
    int w = fm_write_uint(v, buf, 32);
    if (w < 0) return 0;
    int pos = *ppos;
    if (pos + w >= outCap) return 0;
    for (int i = 0; i < w; ++i) out[pos++] = buf[i];
    *ppos = pos;
    return 1;
}

template <size_t N>
bool read_line_into(char (&buf)[N]) {
    if (N == 0) return false;
    buf[0] = '\0';

    size_t i = 0;
    for (;;) {
        int ch = std::cin.get();
        if (ch == EOF) {
            if (i == 0) {
                std::cin.clear();
                return false;
            }
            break;
        }
        if (ch == EOF) {
            if (i == 0) return false;
            break;
        }
        if (ch == '\n') break;

        if (i + 1 < N) {
            buf[i++] = static_cast<char>(ch);
        } else {
            while (true) {
                int c2 = std::cin.get();
                if (c2 == '\n' || c2 == EOF) break;
            }
            throw "Entrada demasiado larga para el buffer.";
        }
    }
    buf[i] = '\0';
    return true;
}

static inline void strip(char* s) {
    if (!s) return;
    size_t start = 0;
    while (s[start] && is_space(s[start])) ++start;

    size_t end = start;
    while (s[end]) ++end;
    while (end > start && is_space(s[end-1])) --end;

    size_t j = 0;
    for (size_t i = start; i < end; ++i) s[j++] = s[i];
    s[j] = '\0';
}

static inline void copy_(char* dst, const char* src, size_t N) {
    if (!dst || !src || N == 0) throw "Buffer inválido.";
    size_t i = 0;
    for (; src[i] != '\0'; ++i) {
        if (i + 1 >= N) throw "Dato no cabe en el buffer destino.";
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

bool leer_cedula(char out[CED_BUF]) {
    try {
        for (;;) {
            std::cout << "Ingrese cedula (solo dígitos): ";
            std::cout.flush();

            char tmp[CED_BUF];
            if (!read_line_into(tmp)) return false;
            strip(tmp);

            if (tmp[0] == '\0') { std::cerr << "Error: vacío.\n"; continue; }

            int len = 0;
            while (tmp[len] != '\0') ++len;
            if (len < 5) { std::cerr << "Error: demasiado corta.\n"; continue; }

            for (int i = 0; tmp[i] != '\0'; ++i) {
                if (!is_digit(tmp[i])) { std::cerr << "Error: solo dígitos.\n"; goto next_attempt_ced; }
            }

            copy_(out, tmp, CED_BUF);
            return true;

        next_attempt_ced:
            continue;
        }
    } catch (const char* msg) {
        std::cerr << "Excepción: " << msg << "\n";
        return false;
    } catch (...) {
        std::cerr << "Excepción desconocida leyendo cédula.\n";
        return false;
    }
}

bool leer_pass(char out[PASS_BUF]) {
    try {
        for (;;) {
            std::cout << "Ingrese contraseña (>=6, alfanumerica, puede incluir simbolos imprimibles): ";
            std::cout.flush();

            char tmp[PASS_BUF];
            if (!read_line_into(tmp)) return false;
            strip(tmp);

            int len = 0;
            bool has_digit = false, has_alpha = false, all_printable = true;
            for (; tmp[len] != '\0'; ++len) {
                char c = tmp[len];
                if (is_digit(c)) has_digit = true;
                if (is_alpha(c)) has_alpha = true;
                if (!is_printable(c) || is_space(c)) all_printable = false;
            }
            if (len < 6)            { std::cerr << "Error: mínima longitud 6.\n"; continue; }
            if (!all_printable)     { std::cerr << "Error: caracteres no imprimibles o espacios.\n"; continue; }

            copy_(out, tmp, PASS_BUF);
            return true;
        }
    } catch (const char* msg) {
        std::cerr << "Excepción: " << msg << "\n";
        return false;
    } catch (...) {
        std::cerr << "Excepción desconocida leyendo contraseña.\n";
        return false;
    }
}

bool leer_long_pos(const char* prompt, long& out) {
    try {
        for (;;) {
            if (prompt && *prompt) { std::cout << prompt; }
            else                   { std::cout << "Ingrese entero positivo: "; }
            std::cout.flush();

            char buf[64];
            if (!read_line_into(buf)) return false;
            strip(buf);

            if (buf[0] == '\0') { std::cerr << "Error: vacío.\n"; continue; }

            size_t i = 0;
            if (!is_digit(buf[i])) { std::cerr << "Error: debe iniciar con dígito.\n"; continue; }

            long outTmp = 0;
            for (; buf[i] != '\0'; ++i) {
                if (!is_digit(buf[i])) { std::cerr << "Error: solo dígitos.\n"; goto next_attempt_long; }
                int d = buf[i] - '0';

                if (outTmp > (LONG_MAX - d) / 10) {
                    std::cerr << "Error: overflow del tipo long.\n";
                    goto next_attempt_long;
                }
                outTmp = outTmp * 10 + d;
            }

            if (outTmp <= 0) { std::cerr << "Error: debe ser > 0.\n"; continue; }

            out = outTmp;
            return true;

        next_attempt_long:
            continue;
        }
    } catch (const char* msg) {
        std::cerr << "Excepción: " << msg << "\n";
        return false;
    } catch (...) {
        std::cerr << "Excepción desconocida leyendo entero.\n";
        return false;
    }
}

int idxAdmin(const char* cedula, const char* clave,
             char ced[][CED_BUF], char pass[][PASS_BUF], int nAdmins)
{
    if (!cedula || !clave || !ced || !pass || nAdmins <= 0) return -1;

    for (int i = 0; i < nAdmins; ++i) {
        int a = 0;
        while (ced[i][a] != '\0' && cedula[a] != '\0' && ced[i][a] == cedula[a]) ++a;
        bool cedMatch = (ced[i][a] == '\0' && cedula[a] == '\0');
        if (!cedMatch) continue;

        int b = 0;
        while (pass[i][b] != '\0' && clave[b] != '\0' && pass[i][b] == clave[b]) ++b;
        bool passMatch = (pass[i][b] == '\0' && clave[b] == '\0');
        if (passMatch) return i;
    }
    return -1;
}

int idxUserByCed(const char* cedula,
                 char ced[][CED_BUF], int nUsers)
{
    if (!cedula || !ced || nUsers <= 0) return -1;

    for (int i = 0; i < nUsers; ++i) {
        int a = 0;
        while (ced[i][a] != '\0' && cedula[a] != '\0' && ced[i][a] == cedula[a]) ++a;

        if (ced[i][a] == '\0' && cedula[a] == '\0') return i;
    }
    return -1;
}

int idxUser(const char* cedula, const char* clave,
            char ced[][CED_BUF], char pass[][PASS_BUF], int nUsers)
{
    if (!cedula || !ced || nUsers <= 0) return -1;

    for (int i = 0; i < nUsers; ++i) {
        int a = 0;
        while (ced[i][a] != '\0' && cedula[a] != '\0' && ced[i][a] == cedula[a]) ++a;
        bool cedMatch = (ced[i][a] == '\0' && cedula[a] == '\0');
        if (!cedMatch) continue;

        int b = 0;
        while (pass[i][b] != '\0' && clave[b] != '\0' && pass[i][b] == clave[b]) ++b;
        bool passMatch = (pass[i][b] == '\0' && clave[b] == '\0');
        if (passMatch) return i;
    }
    return -1;
}

bool leer_opcion_0_9(int& out) {
    try {
        for (;;) {
            std::cout << "Opcion [0-9]: ";
            std::cout.flush();

            char buf[8];
            if (!read_line_into(buf))
                return false;

            strip(buf);
            if (buf[0] == '\0' || buf[1] != '\0') {
                std::cerr << "Ingrese un unico digito (0-9).\n";
                continue;
            }
            if (!is_digit(buf[0])) {
                std::cerr << "Solo se permiten digitos (0-9).\n";
                continue;
            }

            out = buf[0] - '0';
            return true;
        }
    } catch (const char* msg) {
        std::cerr << "Excepcion: " << msg << "\n";
        return false;
    } catch (...) {
        std::cerr << "Excepcion desconocida leyendo opcion.\n";
        return false;
    }
}

bool menu_admin(const char* cedulaAdmin,
                char userCed[][CED_BUF], char userPass[][PASS_BUF], long* userSaldo,
                int* nUsers, int maxUsers,
                const char* usuariosFile,
                int metodo, int semilla)
{
    if (!cedulaAdmin || !userCed || !userPass || !userSaldo || !nUsers ||
        !usuariosFile || maxUsers <= 0 || semilla <= 0 || (metodo != 1 && metodo != 2)) {
        std::cout << "Parametros invalidos en menu_admin.\n";
        return false;
    }

    for (;;) {
        std::cout << "\n____ MENU ADMIN ____\n";
        std::cout << "1) Crear usuario\n";
        std::cout << "2) Salir\n";
        std::cout << "Opción: ";

        int op = 0;
        if(!leer_opcion_0_9(op)) { std::cout << "Entrada cancelada por EOF \n"; break; }

        if (op == 1) {
            bool ok = crear_usuario_admin(userCed, userPass, userSaldo,
                                          nUsers, maxUsers,
                                          usuariosFile, metodo, semilla);
            if (ok) {
                std::cout << "Usuario creado. Total usuarios: " << *nUsers << "\n";
            } else {
                std::cout << "No se pudo crear el usuario.\n";
            }
        } else if (op == 2) {
            std::cout << "Saliendo del menu admin...\n";
            return true;
        } else {
            std::cout << "Opción no valida.\n";
        }
    }
    return false;
}

int construirLineaUsuario(char* out, int outCap,
                          const char* ced, const char* pass, long saldo)
{
    if (!out || outCap <= 0 || !ced || !pass || saldo < 0) return -1;

    int lenCed = 0;
    while (ced[lenCed] != '\0') {
        if (ced[lenCed] == ',' || ced[lenCed] == '\n' || ced[lenCed] == '\r') return -1;
        ++lenCed;
    }
    int lenPass = 0;
    while (pass[lenPass] != '\0') {
        if (pass[lenPass] == ',' || pass[lenPass] == '\n' || pass[lenPass] == '\r') return -1;
        ++lenPass;
    }

    char saldoBuf[32];
    int  saldoLen = 0;
    if (saldo == 0) {
        saldoBuf[0] = '0';
        saldoLen = 1;
    } else {
        unsigned long u = (unsigned long)saldo;
        char tmp[32];
        int  t = 0;
        while (u > 0 && t < 31) {
            tmp[t++] = (char)('0' + (u % 10));
            u /= 10;
        }
        for (int i = 0; i < t; ++i) saldoBuf[i] = tmp[t - 1 - i];
        saldoLen = t;
    }

    int total = lenCed + 1 + lenPass + 1 + saldoLen;
    if (total + 1 > outCap) return -1;

    int pos = 0;
    for (int i = 0; i < lenCed;  ++i) out[pos++] = ced[i];
    out[pos++] = ',';
    for (int i = 0; i < lenPass; ++i) out[pos++] = pass[i];
    out[pos++] = ',';
    for (int i = 0; i < saldoLen; ++i) out[pos++] = saldoBuf[i];

    out[pos] = '\0';
    return pos;
}

bool agregarUsuarioRAM(const char* ced, const char* pass, long saldo,
                       char userCed[][CED_BUF], char userPass[][PASS_BUF], long* userSaldo,
                       int* nUsers, int maxUsers)
{
    if (!ced || !pass || !userCed || !userPass || !userSaldo || !nUsers) return false;
    if (maxUsers <= 0 || *nUsers < 0 || *nUsers >= maxUsers) return false;
    if (saldo < 0) return false;

    int lenCed = 0;
    while (ced[lenCed] != '\0') {
        if (lenCed >= CED_BUF - 1) return false;
        ++lenCed;
    }
    int lenPass = 0;
    while (pass[lenPass] != '\0') {
        if (lenPass >= PASS_BUF - 1) return false;
        ++lenPass;
    }

    int idx = *nUsers;

    for (int i = 0; i < lenCed; ++i) userCed[idx][i] = ced[i];
    userCed[idx][lenCed] = '\0';

    for (int i = 0; i < lenPass; ++i) userPass[idx][i] = pass[i];
    userPass[idx][lenPass] = '\0';

    userSaldo[idx] = saldo;

    (*nUsers)++;
    return true;
}

bool crear_usuario_admin(char userCed[][CED_BUF], char userPass[][PASS_BUF], long* userSaldo,
                         int* nUsers, int maxUsers,
                         const char* usuariosFile,
                         int metodo, int semilla)
{
    if (!userCed || !userPass || !userSaldo || !nUsers || !usuariosFile ||
        maxUsers <= 0 || semilla <= 0 || (metodo != 1 && metodo != 2)) {
        std::cout << "Parámetros inválidos en crear_usuario_admin.\n";
        return false;
    }

    if (*nUsers >= maxUsers) {
        std::cout << "Capacidad de usuarios llena (" << maxUsers << ").\n";
        return false;
    }

    char cedTmp[CED_BUF];
    char passTmp[PASS_BUF];
    long saldoTmp = 0;

    if (!leer_cedula(cedTmp)) {
        std::cout << "Cédula inválida.\n";
        return false;
    }
    if (!leer_pass(passTmp)) {
        std::cout << "Clave inválida.\n";
        return false;
    }
    if (!leer_long_pos("Saldo inicial (>=0): ", saldoTmp)) {
        std::cout << "Saldo inicial inválido.\n";
        return false;
    }

    if (idxUserByCed(cedTmp, userCed, *nUsers) >= 0) {
        std::cout << "Ya existe un usuario con esa cédula.\n";
        return false;
    }

    char linea[256];
    int wrote = construirLineaUsuario(linea, (int)sizeof(linea), cedTmp, passTmp, saldoTmp);
    if (wrote < 0) {
        std::cout << "No se pudo construir la línea CSV (buffer insuficiente).\n";
        return false;
    }

    if (!appendLineaCodificada(usuariosFile, metodo, semilla, linea)) {
        std::cout << "Falló append codificado al archivo de usuarios.\n";
        return false;
    }

    if (!agregarUsuarioRAM(cedTmp, passTmp, saldoTmp, userCed, userPass, userSaldo, nUsers, maxUsers)) {
        std::cout << "No se pudo agregar en RAM (capacidad?).\n";
        return false;
    }

    return true;
}

bool menu_usuario(const char* cedulaUsuario,
                  char userCed[][CED_BUF], char userPass[][PASS_BUF], long* userSaldo,
                  int nUsers,
                  const char* usuariosFile, const char* txFile,
                  int metodo, int semilla, long tarifaIngresoCOP)
{

    if (!cedulaUsuario || !userCed || !userPass || !userSaldo ||
        !usuariosFile || !txFile ||
        nUsers <= 0 || semilla <= 0 || (metodo != 1 && metodo != 2) ||
        tarifaIngresoCOP < 0) {
        std::cout << "Parametros invalidos en menu_usuario.\n";
        return false;
    }

    int idx = idxUserByCed(cedulaUsuario, userCed, nUsers);
    if (idx < 0) {
        std::cout << "Usuario no encontrado en RAM.\n";
        return false;
    }

    if (tarifaIngresoCOP > 0) {
        if (userSaldo[idx] < tarifaIngresoCOP) {
            std::cout << "Saldo insuficiente para cubrir la tarifa de ingreso ("
                      << tarifaIngresoCOP << ").\n";
            return false;
        }

        long saldoAntes   = userSaldo[idx];
        long saldoDespues = saldoAntes - tarifaIngresoCOP;
        userSaldo[idx]    = saldoDespues;

        long id = cargarNextIdTx(txFile, metodo, semilla);
        if (id < 1) {
            userSaldo[idx] = saldoAntes;
            std::cout << "No se pudo obtener nextId de transaccion.\n";
            return false;
        }

        if (!registrarTx(txFile, metodo, semilla,
                         id, cedulaUsuario, TX_FEE,
                         tarifaIngresoCOP,0,
                         saldoAntes, saldoDespues)) {
            userSaldo[idx] = saldoAntes;
            std::cout << "No se pudo registrar TX_FEE.\n";
            return false;
        }

        if (!reescribirUsuariosDesdeRAM(usuariosFile, metodo, semilla,
                                        userCed, userPass, userSaldo, nUsers)) {
            userSaldo[idx] = saldoAntes;
            std::cout << "[ERR] No se pudo persistir usuarios tras cobrar tarifa.\n";
            return false;
        }
    }

    for (;;) {
        std::cout << "\n____ MENU USUARIO ____(" << cedulaUsuario << ") ____\n";
        std::cout << "1) Consultar saldo\n";
        std::cout << "2) Retirar dinero\n";
        std::cout << "3) Salir\n";
        std::cout << "Opcion: ";

        int op = 0;
        if(!leer_opcion_0_9(op)) { continue; }

        if (op == 1) {
            std::cout << "Saldo actual: " << userSaldo[idx] << " COP\n";

        } else if (op == 2) {
            long monto = 0;
            if (!leer_long_pos("Monto a retirar (>=1): ", monto)) {
                std::cout << "Monto invalido.\n";
                continue;
            }
            if (monto < 1) {
                std::cout << "Monto debe ser >= 1.\n";
                continue;
            }
            if (monto > userSaldo[idx]) {
                std::cout << "Saldo insuficiente.\n";
                continue;
            }

            long saldoAntes   = userSaldo[idx];
            long saldoDespues = saldoAntes - monto;
            userSaldo[idx]    = saldoDespues;

            long id = cargarNextIdTx(txFile, metodo, semilla);
            if (id < 1) {
                userSaldo[idx] = saldoAntes;
                std::cout << "No se pudo obtener nextId de transaccion.\n";
                continue;
            }

            if (!registrarTx(txFile, metodo, semilla,
                             id, cedulaUsuario, TX_RETIRO,
                             0, monto,
                             saldoAntes, saldoDespues)) {
                userSaldo[idx] = saldoAntes;
                std::cout << "No se pudo registrar TX_RETIRO.\n";
                continue;
            }

            if (!reescribirUsuariosDesdeRAM(usuariosFile, metodo, semilla,
                                            userCed, userPass, userSaldo, nUsers)) {
                userSaldo[idx] = saldoAntes;
                std::cout << " No se pudo persistir usuarios tras retiro.\n";
                continue;
            }

            std::cout << "Retiro exitoso. Nuevo saldo: " << userSaldo[idx] << " COP\n";

        } else if (op == 3) {
            std::cout << "Saliendo del menu de usuario...\n";
            return true;
        } else {
            std::cout << " Opcion no valida.\n";
        }
    }
}

int construirLineaTx(char* out, int outCap,
                     long id, const char* ced, const char* tipo,
                     long costo, long monto, long saldoAntes, long saldoDespues)
{
    if (!out || outCap <= 0 || !ced || !tipo) return -1;
    if (id < 0 || costo < 0 || monto < 0 || saldoAntes < 0 || saldoDespues < 0) return -1;
    if (fm_contains_forbidden(ced) || fm_contains_forbidden(tipo)) return -1;

    int pos = 0;
    if (!fm_append_uint(out, outCap, &pos, (unsigned long)id))             return -1;
    if (!fm_append_char(out, outCap, &pos, ','))                           return -1;
    if (!fm_append_str(out, outCap, &pos, ced))                            return -1;
    if (!fm_append_char(out, outCap, &pos, ','))                           return -1;
    if (!fm_append_str(out, outCap, &pos, tipo))                           return -1;
    if (!fm_append_char(out, outCap, &pos, ','))                           return -1;
    if (!fm_append_uint(out, outCap, &pos, (unsigned long)costo))          return -1;
    if (!fm_append_char(out, outCap, &pos, ','))                           return -1;
    if (!fm_append_uint(out, outCap, &pos, (unsigned long)monto))          return -1;
    if (!fm_append_char(out, outCap, &pos, ','))                           return -1;
    if (!fm_append_uint(out, outCap, &pos, (unsigned long)saldoAntes))     return -1;
    if (!fm_append_char(out, outCap, &pos, ','))                           return -1;
    if (!fm_append_uint(out, outCap, &pos, (unsigned long)saldoDespues))   return -1;

    if (pos >= outCap) return -1;
    out[pos] = '\0';
    return pos;
}


bool registrarTx(const char* txFile, int metodo, int semilla,
                 long id, const char* ced, const char* tipo,
                 long costo, long monto, long saldoAntes, long saldoDespues)
{
    if (!txFile || !ced || !tipo) return false;
    if (semilla <= 0 || (metodo != 1 && metodo != 2)) return false;
    if (id < 1 || costo < 0 || monto < 0 || saldoAntes < 0 || saldoDespues < 0) return false;

    char linea[256];
    int wrote = construirLineaTx(linea, (int)sizeof(linea),
                                 id, ced, tipo, costo, monto, saldoAntes, saldoDespues);
    if (wrote < 0) return false;

    return appendLineaCodificada(txFile, metodo, semilla, linea);
}

bool reescribirUsuariosDesdeRAM(const char* usuariosFile, int metodo, int semilla,
                                char ced[][CED_BUF], char pass[][PASS_BUF], const long* saldo,
                                int nUsers)
{
    if (!usuariosFile || !ced || !pass || !saldo) return false;
    if (nUsers < 0 || semilla <= 0 || (metodo != 1 && metodo != 2)) return false;

    const char* header = "cedula,clave,saldo";

    int headerLen = 0; while (header[headerLen] != '\0') ++headerLen;
    int cap = headerLen + (nUsers > 0 ? 1 : 0) + nUsers * 256;
    if (cap <= 0) cap = 1;

    unsigned char* plano = new unsigned char[cap];
    if (!plano) return false;

    int pos = 0;

    for (int i = 0; i < headerLen; ++i) plano[pos++] = (unsigned char)header[i];

    if (nUsers > 0) {
        plano[pos++] = (unsigned char)'\n';
    }

    for (int i = 0; i < nUsers; ++i) {
        char linea[256];
        int wrote = construirLineaUsuario(linea, (int)sizeof(linea),
                                          ced[i], pass[i], saldo[i]);
        if (wrote < 0) { delete[] plano; return false; }

        for (int k = 0; k < wrote; ++k) {
            if (pos >= cap) { delete[] plano; return false; }
            plano[pos++] = (unsigned char)linea[k];
        }

        if (i < nUsers - 1) {
            if (pos >= cap) { delete[] plano; return false; }
            plano[pos++] = (unsigned char)'\n';
        }
    }
    bool ok = escribirPlanoCodificado(usuariosFile, metodo, semilla, plano, pos);

    delete[] plano;
    return ok;
}

