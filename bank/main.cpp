#include <iostream>
#include <limits>
#include "fileManage.h"
#include "encodec.h"
#include "decode.h"
#include "userManage.h"

constexpr int MAX_ADM  = 100;
constexpr int MAX_USR  = 100;

int main(int argc, char* argv[]) {
    const char* SUDO_FILE  = "sudo.csv";
    const char* USERS_FILE = "usuarios.csv";
    const char* TX_FILE    = "transacciones.csv";

    int metodo  = 1;
    int semilla = 4;

    if (argc == 3) {
        metodo  = argv[1][0] - '0';
        semilla = 0;
        for (int i = 0; argv[2][i]; ++i) semilla = semilla * 10 + (argv[2][i] - '0');
        if ((metodo != 1 && metodo != 2) || semilla <= 0) {
            std::cerr << "Uso: " << argv[0] << " [metodo=1|2] [semilla>0]\n";
            return 1;
        }
    }

    // Crear archivos con encabezado
    if (!crearSudoConEncabezado(SUDO_FILE, metodo, semilla))
        std::cout << "crearSudoConEncabezado: puede que ya exista.\n";
    if (!crearUsuariosConEncabezado(USERS_FILE, metodo, semilla))
        std::cout << "crearUsuariosConEncabezado: puede que ya exista.\n";
    if (!crearTransaccionesConEncabezado(TX_FILE, metodo, semilla))
        std::cout << "crearTransaccionesConEncabezado: puede que ya exista.\n";

    // ---- Cargar admins ----
    char cedAdm[MAX_ADM][CED_BUF]{};
    char passAdm[MAX_ADM][PASS_BUF]{};
    int nAdmins = 0;

    bool okAdm = cargarAdmins(SUDO_FILE, metodo, semilla,
                              cedAdm, passAdm, MAX_ADM, &nAdmins);

    if (!okAdm) std::cerr << "Error cargando admins\n";

    if (nAdmins == 0) {
        const char* lineaDefaultAdmin = "111222111222111,12345678";
        if (appendLineaCodificada(SUDO_FILE, metodo, semilla, lineaDefaultAdmin)) {
            okAdm = cargarAdmins(SUDO_FILE, metodo, semilla, cedAdm, passAdm, MAX_ADM, &nAdmins);
            std::cout << "Admin por defecto creado. nAdmins=" << nAdmins << "\n";
        } else {
            std::cerr << "No pude crear admin por defecto.\n";
        }
    }

    // ---- Cargar usuarios ----
    char cedUsr[MAX_USR][CED_BUF]{};
    char passUsr[MAX_USR][PASS_BUF]{};
    long saldoUsr[MAX_USR]{};
    int nUsers = 0;

    bool okUsr = cargarUsuarios(USERS_FILE, metodo, semilla,
                                cedUsr, passUsr, saldoUsr, MAX_USR, &nUsers);

    if (!okUsr) std::cerr << "Error cargando usuarios\n";

    // ---- Login ----
    char cedula[CED_BUF] = {};
    char pass[PASS_BUF] = {};
    int idx = -1;
    short rol = 0;

    do {
        if (!leer_cedula(cedula)) return 0;
        if (!leer_pass(pass)) return 0;

        idx = idxAdmin(cedula, pass, cedAdm, passAdm, nAdmins);
        if(idx != -1) rol = 1;
        else{ idx = idxUser(cedula, pass, cedUsr, passUsr, nUsers); rol = 2; }

        std::cout << ((idx == -1) ? "Login invalido\n" : "Login valido\n");

    } while (idx == -1);
    /*
    // --- limpiar buffers sensibles antes de salir de scope ---
    for (size_t i = 0; cedula[i] != '\0'; ++i) cedula[i] = 0;
    for (size_t i = 0; pass[i]   != '\0'; ++i) pass[i]   = 0;
    */

    // Mensaje de saldo bajo (solo clientes)
    if (rol == 2 && saldoUsr[idx] < 1000) {
        std::cout << "Saldo insuficiente (< 1000)\n";
    }

    // ---- Menú según rol ----
    bool logout = false;
    while (!logout) {
        switch (rol) {
        case 1:
            menu_admin(cedula,
                        cedUsr, passUsr, saldoUsr,
                        &nUsers, MAX_USR,
                        USERS_FILE,
                        metodo, semilla);
            logout = true;
            break;
        case 2:
            menu_usuario(cedula,cedUsr,passUsr, saldoUsr,nUsers,USERS_FILE,TX_FILE,metodo,semilla,1000L);
            logout = true;
            break;
        default:
            logout = true;
            break;
        }
    }

    return 0;
}
