#include <cstddef>
#include "../src/encodec.h"
#include "../src/utilitis.h"
#include <iostream>

/*
 * Acerca de los archivos:
 *      Todos los archivos de entrada deben estar en formato .txt
 *      Todos los archivos de salida van a estar en forma .txt
 *
 * Para los argumentos pasados por consola se tendra la siguiente convencion:
 *     Numero de argumentos: argc igual a 4
 *     Contenido de argv:
 *          posicion 1: Nombre del archivo de entrada - name - sin el sufijo .txt
 *          posicion 2: Nombre del archivo de salida - name - sin el sufijo .txt
 *          posicion 3: Semilla de codificacion, sin espacios y solo caracteres numericos
 *          posicion 4: Metodo de codificacion:
 *              Un solo caracter numerico que puede ser 1 o 2 nada mas, siguiendo la siguiente correspondencia:
 *                  Metodo 1: Ingrasar '1'
 *              Metodo 2: Ingresar '2'
 *          Cualquier entrada fuera del formato indicado provocara un cierre del programa
 *
 *
*/

int main(int argc, char *argv[])
{

    if (argc != 5) {
        std::cerr << "Uso: encode <in> <out> <semilla> <metodo(1|2)>\n";
        return 1;
    }

    char* name_in = argv[1];

    char* name_out = argv[2];

    char* semilla_in = argv[3];
    int semilla = chartoint(semilla_in);

    char* metodo_in = argv[4];
    int metodo = chartoint(metodo_in);

    //std::cout << "name in : |" << name_in << "| name out : |" << name_out << "| semilla: |" << semilla << "| metodo :|" << metodo << "|" << std::endl;

    char* datos = nullptr;
    int lenDatos = 0;

    leerArchivo(datos,name_in,lenDatos);

    //std::cout << "Leídos " << lenDatos << " bytes: " << (datos ? datos : "") << "\n";

    unsigned char* datos_puros = new unsigned char [lenDatos];

    //for(std::size_t i = 0; i < lenDatos; i++) datos_puros[i] = datos[i];

    unsigned char* decodificado = metodo_1(semilla,datos_puros,lenDatos);

    //for(std::size_t i = 0; i < lenDatos; i++) std::cout << static_cast<char>(decodificado[i]);

    escribirArchivo(decodificado,name_out,lenDatos);

    delete[] decodificado;
    delete[] datos_puros;
    delete[] datos;

    return 0;
}
