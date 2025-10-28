#include "decode.h"
#include "utilitis.h"

unsigned char* de_metodo_1 ( int semilla, const unsigned char* input_, std::size_t lenIn){

    int n = semilla;

    std::size_t totalBits = lenIn * 8;

    std::size_t sobrante = totalBits % n;
    int* comodin = nullptr;
    if( sobrante ) comodin = new int [sobrante]();

    std::size_t limite = totalBits - sobrante;

    std::size_t nBloques  = limite / n;

    if (nBloques == 0) {
        unsigned char* out = new unsigned char[lenIn];
        for (std::size_t i = 0; i < lenIn; i++) out[i] = input_[i];
        if(sobrante) delete[] comodin;
        return out;
    }

    int** bloques_cod = new int*[nBloques];
    for (std::size_t i = 0; i < (nBloques) ; i++) bloques_cod[i] = new int[n];
    std::size_t contaBloqu = 0;
    std::size_t bit_global = 0;
    std::size_t count_modin = 0;

    for( std::size_t idx = 0; idx < lenIn ; idx++)
    {

        char character = input_[idx];
        int bitsArra[8];
        conversionCharBinario(bitsArra,character);

        for (int bit = 0; bit < 8; bit++)
        {
            int bit_act = bitsArra[bit];

            if(bit_global < limite){
                int bitblock = bit_global % n;
                bloques_cod[contaBloqu][bitblock] = bit_act;
                if(bitblock == n-1)contaBloqu++;
            }
            else comodin[count_modin++] = bit_act;

            bit_global++;

        }
    }


    int *act_transf = new int[n];
    int modul = 1;
    int *bloq_reference = nullptr;
    int** bloques_dec = new int*[nBloques];
    for (std::size_t i = 0; i < (nBloques) ; i++) bloques_dec[i] = new int[n];

    // transformacion establecimiento del primer bloque de referencia
    // Caso base:
    for(int j=0 ; j<n ; j++)  act_transf[j] = bloques_cod[0][j];
    invertirArray(act_transf, n, modul);
    for (int k = 0; k < n; k++) bloques_dec[0][k] = act_transf[k];
    bloq_reference = bloques_dec[0];


    int ceros = contadorCeros(bloq_reference,n);
    int unos = n-ceros;

    for(std::size_t  i = 1; i < nBloques ; i++)
    {
        for(int carac = 0 ; carac < n ; carac++)  act_transf[carac] = bloques_cod[i][carac];

        if (ceros == unos) modul = 1;
        else if (ceros > unos)  modul = 2;
        else if (unos > ceros)  modul = 3;

        invertirArray(act_transf, n, modul);
        for (int k = 0; k < n; k++) bloques_dec[i][k] = act_transf[k];

        bloq_reference = bloques_dec[i];
        ceros = contadorCeros(bloq_reference,n);
        unos = n-ceros;

    }


    unsigned char* archivo_codificado = new unsigned char[lenIn]();
    std::size_t conBiGl = 0;
    for( std::size_t i = 0; i < nBloques; i++)
    {
        for(int i_bl = 0; i_bl < n; i_bl++){
            char bit = bloques_dec[i][i_bl];
            std::size_t posByte = conBiGl / 8;
            std::size_t bitPos  = 7 - (conBiGl % 8);
            archivo_codificado[posByte] |= (bit) << bitPos;
            conBiGl++;
        }
    }

    delete[] act_transf;

    for (std::size_t i_ = 0; i_ < sobrante; i_++, conBiGl++)
    {
        std::size_t posByte = conBiGl / 8;
        std::size_t bitPos  = 7 - (conBiGl % 8);
        unsigned char bit = (unsigned char)comodin[i_];
        archivo_codificado[posByte] |= (bit << bitPos);
    }

    delete[] comodin;

    for (std::size_t i = 0; i < nBloques; i++)
        delete[] bloques_cod[i];
    delete[] bloques_cod;

    for (std::size_t i = 0; i < nBloques; i++)
        delete[] bloques_dec[i];
    delete[] bloques_dec;


    return archivo_codificado;

}

unsigned char* de_metodo_2 ( int semilla, const unsigned char* input_, std::size_t lenIn){

    int n = semilla;

    std::size_t totalBits = lenIn * 8;

    std::size_t sobrante = totalBits % n;
    int* comodin = nullptr;
    if( sobrante ) comodin = new int [sobrante]();

    std::size_t limite = totalBits-sobrante;

    std::size_t nBloques  = limite / n;

    int** bloques = new int*[nBloques];
    for (std::size_t i = 0; i < (nBloques) ; i++) bloques[i] = new int[n];
    std::size_t contaBloqu = 0;
    std::size_t bit_global = 0;
    std::size_t count_modin = 0;

    for( std::size_t idx = 0; idx < lenIn ; idx++)
    {

        char character = input_[idx];
        int bitsArra[8];
        conversionCharBinario(bitsArra,character);

        for (int bit = 0; bit < 8; bit++)
        {
            int bit_act = bitsArra[bit];

            if(bit_global < limite){
                int bitblock = bit_global % n;
                bloques[contaBloqu][bitblock] = bit_act;
                if(bitblock == n-1)contaBloqu++;
            }
            else comodin[count_modin++] = bit_act;

            bit_global++;

        }
    }

    int** bloquesTrans = new int*[nBloques];
    for (std::size_t i = 0; i < (nBloques) ; i++) bloquesTrans[i] = new int[n];

    for(std::size_t i = 0; i < nBloques; i++) rotar_izquierda(bloques[i],1,bloquesTrans[i],n);

    unsigned char* archivo_codificado = new unsigned char[lenIn]();
    std::size_t conBiGl = 0;
    for( std::size_t i = 0; i < nBloques; i++)
    {
        for(int i_bl = 0; i_bl < n; i_bl++){
            char bit = bloquesTrans[i][i_bl];
            std::size_t posByte = conBiGl / 8;
            std::size_t bitPos  = 7 - (conBiGl % 8);
            archivo_codificado[posByte] |= (bit) << bitPos;
            conBiGl++;
        }
    }

    for (std::size_t i_ = 0; i_ < sobrante; i_++, conBiGl++)
    {
        std::size_t posByte = conBiGl / 8;
        std::size_t bitPos  = 7 - (conBiGl % 8);
        unsigned char bit = (unsigned char)comodin[i_];
        archivo_codificado[posByte] |= (bit << bitPos);
    }

    if (sobrante) delete[] comodin;

    for (std::size_t i = 0; i < nBloques; i++)
        delete[] bloques[i];
    delete[] bloques;

    for (std::size_t i = 0; i < nBloques; i++)
        delete[] bloquesTrans[i];
    delete[] bloquesTrans;


    return archivo_codificado;

}

