#ifndef UTILITIS_H
#define UTILITIS_H

void conversionCharBinario(int *bits,unsigned char n);
int contadorCeros(int *bits, unsigned int n);
void invertirArray(int *bits, int n, int modul);
void leerArchivo(char*& out, const char* nombreArchivo, int &tamano);
int elevar_a_n(int a,int n);
int chartoint(const char* in);
void escribirArchivo(const unsigned char* in, const char* nombreArachivo, int len_in);
void rotar_derecha(int* in, int bits, int* out, int size);

#endif //UTILITIS_H
