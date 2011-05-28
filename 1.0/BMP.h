#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Estruturas.h"

typedef struct /**** header do arquivo bmp ****/
{
    unsigned short bfType; /* Magic number for file */
    unsigned int bfSize; /* Size of file  cabe�a"fixo" + dados"var"*/ 
    unsigned short bfReserved1; /* Reserved */
    unsigned short bfReserved2; /* ... */
    unsigned int bfOffBits; /* Offset to bitmap data */
} BMPFILEHEADER;

/* bfType deve ser = "BM" */
typedef struct /**** BMP file info structure ****/
{
    unsigned int biSize; /* Size of info header */
    int biWidth; /* Width of image */
    int biHeight; /* Height of image */
    unsigned short biPlanes; /* Number of color planes */
    unsigned short biBitCount; /* Number of bits per pixel */
    unsigned int biCompression; /* Type of compression to use */
    unsigned int biSizeImage; /* Size of image data */
    int biXPelsPerMeter; /* X pixels per meter */
    int biYPelsPerMeter; /* Y pixels per meter */
    unsigned int biClrUsed; /* Number of colors used */
    unsigned int biClrImportant; /* Number of important colors */
} BMPINFOHEADER;

//cria uma novo bmp setando o BM inicial
int BmpNew(char* path);

//abre arquivo Bmp
int BmpOpen(char* path);

//fecha arquivo Bmp
int BmpOldClose();

//fecha arquivo Bmp
int BmpNewClose();

//testa arquivo Se arquivo ja aberto e bmp
int BmpTester();

//atualiza tamanho do arquivo
void BmpFileSize();

/*Descobre profudindade do pixel*/
void BmpBitPixel();

/*Atualiza campo reservado*/
void BmpReservado();

/*Atualiza MagicNumber*/
void BmpMagicNumber();

/*Atualiza OffsetDados*/
void BmpOffsetDados();

//seta tamanho do arquivo atraves do arquivo dado
void setBfSize(unsigned int BfSize);

//seta offBItd do arquivo atraves do arquivo dado
void setBfOffBits(unsigned int BfOffBits);

//seta Width do arquivo atraves do arquivo dado
void setBfBiWidth(int BiWidth);

//seta Height do arquivo atraves do arquivo dado
void setBfBiHeight(int BiHeight);

//pega o tamanho
unsigned int  getBfSize();

//pega o offBItd
unsigned int  getBfOffBits();

//pega o offBItd
unsigned short  getBitCount();

//pega o offBItd
void  setBitCount(unsigned short BitCount);

//pega o Width
int getBfBiWidth();

//pega o Height
int getBfBiHeight();

//verifica se a imagem eh multiplo de 4
int mult4(int a);

//escreve cabecalho do arquivo de saida
void BmpHeadOut();

//Salva Matriz RGB no arquivo BMP
void BmpMatrixSave();

//coloca em Matriz R,G,B a partir do arquivo
void BmpMatrixRead();

//Coloca as informacoes da imagem nas matrizes RGB
int BmpRead(char* path,char *path2,int*** R,int ***g, int ***b);

//Monta a imagem de saida
int BmpSave(char * path, int **R, int **G, int **B);

//
void setbiSizeImage(unsigned int biSizeImage);
