#ifndef USER_MANAGE_H
#define USER_MANAGE_H

#ifndef CED_BUF
#define CED_BUF 16
#endif

#ifndef PASS_BUF
#define PASS_BUF 17
#endif

bool leer_cedula(char out[CED_BUF]);
bool leer_pass(char out[PASS_BUF]);
bool leer_long_pos(const char* prompt, long& out);

// Reescribe usuarios.csv completo desde los arreglos en RAM (para retiros).
bool reescribirUsuariosDesdeRAM(const char* usuariosFile, int metodo, int semilla,
                                char ced[][CED_BUF], char pass[][PASS_BUF], const long* saldo,
                                int nUsers);

// Registra una transacción como línea CSV con append codificado.
// tipo: TX_CONSULTA / TX_RETIRO / TX_FEE

bool registrarTx(const char* txFile, int metodo, int semilla,
                 long id, const char* ced, const char* tipo,
                 long costo, long monto, long saldoAntes, long saldoDespues);


// ========================
// CONSTRUCTORES DE LÍNEAS CSV
// (devuelven número de bytes escritos en 'out' o -1 si no alcanzó)
// ========================

int construirLineaAdmin(char* out, int outCap,
                        const char* ced, const char* pass);

int construirLineaUsuario(char* out, int outCap,
                          const char* ced, const char* pass, long saldo);

int construirLineaTx(char* out, int outCap,
                     long id, const char* ced, const char* tipo,
                     long costo, long monto, long saldoAntes, long saldoDespues);


// ==================
// BÚSQUEDAS / ÍNDICES
// ==================

// Retorna índice (>=0) o -1 si no existe.
int idxAdmin(const char* cedula, const char* clave,
             char ced[][CED_BUF], char pass[][PASS_BUF], int nAdmins);

int idxUser(const char* cedula, const char* clave,
            char ced[][CED_BUF], char pass[][PASS_BUF], int nUsers);

int idxUserByCed(const char* cedula,
                 char ced[][CED_BUF], int nUsers);

// ==================
//Lectores de consola
// ==================

bool leer_cedula(char out[CED_BUF]);
bool leer_pass(char out[PASS_BUF]);
bool leer_long_pos(const char* prompt, long& out);

// ==================
//menus
// ==================

bool menu_admin(const char* cedula,
                char userCed[][CED_BUF], char userPass[][PASS_BUF], long* userSaldo,
                int* nUsers, int maxUsers,
                const char* usuariosFile,
                int metodo, int semilla);

bool menu_usuario(const char* cedula,
                  char userCed[][CED_BUF], char userPass[][PASS_BUF], long* userSaldo,
                  int nUsers,
                  const char* usuariosFile, const char* transFile,
                  int metodo, int semilla,
                  long tarifaIngresoCOP);

// ==================
//menu - admin - funciones
// ==================

int  construirLineaUsuario(char* out, int outCap,
                          const char* ced, const char* pass, long saldo);

//bool appendLineaCodificada(const char* nombre, int metodo, int semilla,
//                           const char* linea);

bool agregarUsuarioRAM(const char* ced, const char* pass, long saldo,
                       char userCed[][CED_BUF], char userPass[][PASS_BUF], long* userSaldo,
                       int* nUsers, int maxUsers);

bool crear_usuario_admin(char userCed[][CED_BUF], char userPass[][PASS_BUF], long* userSaldo,
                         int* nUsers, int maxUsers,
                         const char* usuariosFile,
                         int metodo, int semilla);

#endif // USER_MANAGE_H
