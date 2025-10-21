#include "utilitis.h"
#include <fstream>
#include <iostream>

void conversionCharBinario(int *bits,unsigned char n)
{
    for (int i = 0; i<8 ; i++)
    {
        bits[i] = (n >> (7 - i)) & 1;
    }
}

int contadorCeros(int *bits, unsigned int n)
{
    int contad = 0;
    for ( unsigned int i = 0; i < n; i++)
    {
        if (!bits[i] ) contad++;
    }
    return contad;
}

void invertirArray(int *bits, int n, int modul)
{
    for (int i = 0; i < n; i++)
    {
        if((i+1) % modul == 0)
        {
            bits[i]^=1;
        }
    }
}

bool esDivisorOMultiploDeN(unsigned short int a, int n)
{
    return a != 0 && ( (n % a == 0) || (a % n == 0) );
}

void leerArchivo(char*& out, const char* nombreArchivo, int &tamano)
{
    std :: ifstream archivo(nombreArchivo, std :: ios :: binary);
    if (!archivo.is_open())
    {
        std :: cout << "Error al abrir el archivo : " << nombreArchivo << std :: endl;
        tamano = 0;
        out = nullptr;
        return ;
    }

    archivo.seekg(0, std :: ios::end);
    tamano = archivo.tellg();
    archivo.seekg(0, std :: ios :: beg);

    out = new char[tamano + 1];

    archivo.read(out, tamano);
    out[tamano] = '\0';

    archivo.close();
}


int elevar_a_n(int a,int n)
{
    int resultado = a;

    if (a == 0)
    {
        if (n == 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else if ( n == 0)
    {
        return 1;
    }
    else
    {
        for(int i = n-1; i>0 ; i--)
        {
            resultado *= a;
        }
    }

    return resultado;
}

int chartoint(const char* in){

    int len_in = 0;
    for(int i = 0; in[i]!= '\0'; i++ ) len_in++;

    int out = 0;
    int exp = 0;
    for(int idx = len_in-1; idx >= 0; idx--){
        int num = in[idx]-'0';
        int ten_exp = elevar_a_n(10,exp);
        out += ten_exp * num;
        exp++;
    }

    return out;
}

void escribirArchivo(const unsigned char* in, const char* nombreArachivo, int len_in){

    std::ofstream file;

    file.open(nombreArachivo);

    for( int idx_in = 0; idx_in < len_in ; idx_in++ ){

        if(file)
            file << in[idx_in];
    }

}

//0100 1
//1 0100

void rotar_derecha(int* in, int bits, int* out, int size) {
    int i = 0;
    out[i] = in[size-1];
    i++;
    for (; i < size; i++) out[i] = in[i-1];
}
