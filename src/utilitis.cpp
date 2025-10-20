
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
