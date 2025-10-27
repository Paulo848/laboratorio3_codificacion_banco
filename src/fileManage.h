#ifndef FILE_MANAGE_H
#define FILE_MANAGE_H

// ===== Tamaños fijos (incluyen el '\0') =====
#ifndef CED_BUF
#define CED_BUF 16
#endif

#ifndef PASS_BUF
#define PASS_BUF 17
#endif

// ===== Tipos de transacción (como texto en el CSV) =====
#define TX_CONSULTA "CONSULTA"
#define TX_RETIRO   "RETIRO"
#define TX_FEE      "FEE"

// ============================================
// I/O CODIFICADA (decode -> procesar -> encode)
// ============================================

        // Decodifica archivo codificado a un buffer plano (binario/texto).
        // outBuf: se reserva internamente con new[], el caller la libera.
        // Retorna true en éxito.
        bool leerPlanoCodificado(const char* nombre, int metodo, int semilla,
                                 unsigned char*& outBuf, int& outLen);

        // Decodifica, agrega '\n' si hace falta, concatena "linea",
        // vuelve a codificar y sobrescribe. Retorna true en éxito.
        bool appendLineaCodificada(const char* nombre, int metodo, int semilla,
                                   const char* linea);

        // Codifica y sobrescribe un archivo con el buffer plano dado.
        // Retorna true en éxito.
        bool escribirPlanoCodificado(const char* nombre, int metodo, int semilla,
                                     const unsigned char* inBuf, int inLen);

// =========================
// Generacion de archivos inexistentes con encabezado
// =========================

        // Crea el archivo con el encabezado CSV si está vacío/inesxistente (ya codificado).
        bool crearSudoConEncabezado(const char* nombreSudo, int metodo, int semilla);
        bool crearUsuariosConEncabezado(const char* nombreUsuarios, int metodo, int semilla);
        bool crearTransaccionesConEncabezado(const char* nombreTx, int metodo, int semilla);

// =========================
// CARGA A ESTRUCTURAS RAM
// =========================

        // Carga admins desde CSV decodificado "cedula,clave" (omite encabezado).
        // Arrays: [n][tam_fijo]; nAdmins devuelto por referencia.
        bool cargarAdmins(const char* nombreSudo, int metodo, int semilla,
                          char ced[][CED_BUF], char pass[][PASS_BUF],
                          int maxAdmins, int* nAdmins);

        // Carga usuarios desde CSV "cedula,clave,saldo" (omite encabezado).
        bool cargarUsuarios(const char* nombreUsuarios, int metodo, int semilla,
                            char ced[][CED_BUF], char pass[][PASS_BUF], long* saldo,
                            int maxUsers, int* nUsers);

// =========================
// helper para " construirLineaTx "
// =========================

        // Lee el último id de transacción y retorna nextId (si vacío, 1).
        long cargarNextIdTx(const char* nombreTx, int metodo, int semilla);

// =========================
// helpers
// =========================

        int contarLineas(const unsigned char* buf, int len);

#endif // FILE_MANAGE_H

