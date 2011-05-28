#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "BMP.h"
    
#define shift 0
#define false 0
#define true 1

FILE* image_in = NULL;		//ponteiro para imagem a ser lida
FILE* image_out = NULL;     //ponteiro para imagem a ser gravada

BMPFILEHEADER FILEHEADER;
BMPINFOHEADER INFOHEADER;

int **R3;			//Matrix R
int **G3;			//Matrix G
int **B3;			//Matrix B
int **R2;			//Matrix R
int **G2;			//Matrix G
int **B2;			//Matrix B
int **RI;			//Matrix R
int **GI;			//Matrix G
int **BI;			//Matrix B

//cria uma novo bmp setando o BM inicial
int BmpNew(char* path)
{
	image_out = fopen(path, "wb+");	//cria arquivo

	if(image_out == NULL)
		return false;

	fputc('B', image_out);
	fputc('M', image_out);

	return true;

}//int BmpNew(char* path)


//abre arquivo Bmp
int BmpOpen(char* path)
{
	image_in = fopen(path,"rb+");	//Abre o arquivo

	if(image_in == NULL)
		return false;

	return true;

}//int BmpOpen(char* path)

//fecha arquivo Bmp
int BmpOldClose()
{
	fclose(image_in);	//fecha arquivo de entrada


	return true;
	
}//int BmpClose()

//pega o offBItd
unsigned short  getBitCount(){     
         return INFOHEADER.biBitCount;
}

//pega o offBItd
void  setBitCount(unsigned short BitCount){
      INFOHEADER.biBitCount = BitCount;
}
//fecha arquivo Bmp
int BmpNewClose()
{

	fclose(image_out);	//fecha arquivo de saida

	return true;
	
}//int BmpClose()

//testa arquivo Se arquivo ja aberto e bmp
int BmpTester()
{
	char* buffer=(char*) malloc(sizeof(char)*2);	//aloca string para verificacao

	fread(buffer, sizeof(char), 2, image_in);	//le string do arquivo

	if(!strcmp(buffer, "BM"))
		return true;				//Retorna true se é um arquivo BMP
	else 
		return false;				//Se não for BMP

}//int BmpTester()

//atualiza tamanho do arquivo
void BmpFileSize()
{
	rewind(image_in);				//joga ponteiro no inicio do arquivo
	fseek(image_in, 2 * sizeof(char), 1);		//caminha no arquivo para ver tamanho
	fread(&(FILEHEADER.bfSize), 4, 1, image_in);	//seta tamanho

}//void BmpFileSize()

/*Descobre profudindade do pixel*/
void BmpBitPixel()
{
	rewind(image_in);				//Joga ponteiro no inicio do arquivo
	fseek(image_in, 28 * sizeof(char), 1);		//profundidade do pixel
	fread(&(INFOHEADER.biBitCount), 2, 1, image_in);		//seta largura


}//void BmpBitPixel()

/*Atualiza campo reservado*/
void BmpReservado()
{
	rewind(image_in);				//joga ponteiro no inicio do arquivo
	fseek(image_in, 6 * sizeof(char), 1);		//caminha no arquivo para ver campo reservado
	fread(&(FILEHEADER.bfReserved1), 2, 1, image_in);	//seta reservado1
	fread(&(FILEHEADER.bfReserved2), 2, 1, image_in);	//seta reservado2


}//void BmpReservado()

/*Atualiza MagicNumber*/
void BmpMagicNumber() 
{
	rewind(image_in);				//joga ponteiro no inicio do arquivo
	fread(&(FILEHEADER.bfType), 2, 1, image_in);	//seta reservado1


}//void BmpMagicNumber()


/*Atualiza OffsetDados*/
void BmpOffsetDados()
{
	rewind(image_in);				//joga ponteiro no inicio do arquivo
	fseek(image_in, 10 * sizeof(char), 1);		//caminha no arquivo para ver OffsetDados
	fread(&(FILEHEADER.bfOffBits), 4, 1, image_in);	//seta reservado1


}//void BmpReservado()

//verifica se a imagem eh multiplo de 4
int mult4(int a){
    if(a%4 != 0 ) return (a + 4 - a%4);
    else return a;    
}


//leitura do header da imagem
void ReadInfoHeader() {
     
    rewind(image_in);				//joga ponteiro no inicio do arquivo 
	fseek(image_in, 14, SEEK_SET);		//caminha no arquivo para ver OffsetDados
	fread(&INFOHEADER, 40, 1, image_in); //le infohead
	int temp = mult4(INFOHEADER.biWidth);
    //aloca matrix RGB
	RI = alocaMatint(temp, INFOHEADER.biHeight);		
	GI = alocaMatint(temp, INFOHEADER.biHeight);		
	BI = alocaMatint(temp, INFOHEADER.biHeight);	
    
    
}

//seta tamanho do arquivo atraves do arquivo dado
void setBfSize(unsigned int BfSize){
         FILEHEADER.bfSize = BfSize;
}

//seta offBItd do arquivo atraves do arquivo dado
void setBfOffBits(unsigned int BfOffBits){
         FILEHEADER.bfOffBits = BfOffBits;
}

//seta Width do arquivo atraves do arquivo dado
void setBfBiWidth(int BiWidth){
         INFOHEADER.biWidth = BiWidth;
}

//seta Height do arquivo atraves do arquivo dado
void setBfBiHeight(int BiHeight){
         INFOHEADER.biHeight = BiHeight;
}

//seta ImageSIze
void setbiSizeImage(unsigned int biSizeImage){
         INFOHEADER.biSizeImage = biSizeImage;
}

//pega o tamanho
unsigned int  getBfSize(){
         return FILEHEADER.bfSize;
}

//pega o offBItd
unsigned int  getBfOffBits(){
         return FILEHEADER.bfOffBits;
}

//pega o Width
int getBfBiWidth(){
         return INFOHEADER.biWidth;
}

//pega o Height
int getBfBiHeight(){
         return INFOHEADER.biHeight;
}

//escreve cabecalho do arquivo de saida
void BmpHeadOut()
{
	signed int cbuf[2];
	cbuf[1] = 0;
	
	cbuf[0] = FILEHEADER.bfSize; //tamanho
	fwrite(cbuf, 4, 1, image_out);		//2
	
	cbuf[0] = 0;
	fwrite(cbuf, 4, 1, image_out);		//6
	
	cbuf[0] = FILEHEADER.bfOffBits; 
	fwrite(cbuf, 4, 1, image_out);		//10
	
	cbuf[0] = 40;				//tamanho do head
	fwrite(cbuf, 4, 1, image_out);		//14
	
	cbuf[0] = INFOHEADER.biWidth;
	fwrite(cbuf, 4, 1, image_out);		//18
	
	cbuf[0] = INFOHEADER.biHeight;
	fwrite(cbuf, 4, 1, image_out);		//22
	
	cbuf[0] = 1;				//planos
	fwrite(cbuf, 2, 1, image_out);		//26
	
	cbuf[0] = 24;				//bits por pixel
	fwrite(cbuf, 2, 1, image_out);		//28
	
	cbuf[0] = 0;				//compressao
	fwrite(cbuf, 4, 1, image_out);		//32
	
	cbuf[0] =INFOHEADER.biSizeImage;
	fwrite(cbuf, 4, 1, image_out);		//36
	
	cbuf[0] = 0;				//resol hor
	fwrite(cbuf, 4, 1, image_out);		//40
	
	cbuf[0] = 0;				//resol ver
	fwrite(cbuf, 4, 1, image_out);		//44
	
	cbuf[0] = 0;				//num max cores
	fwrite(cbuf, 4, 1, image_out);		//48
	
	cbuf[0] = 0;				//num cores importantes
	fwrite(cbuf, 4, 1, image_out);		//52

}//escreve cabecalho do arquivo de saida

//funcionando para 24 bits e para 32 funciona - porem ignora a matriz de transparencia
void PixelReader(int *b, int *g, int *r)
{
	unsigned int bt, gt, rt;
	unsigned char cbuf[4];

	fread(&cbuf, sizeof(char), INFOHEADER.biBitCount/8, image_in);		//Divide pelo tamanho de 1 byte= 8 bits
    
	//coloca os valores lidos fazendo cast para int depois para float
	bt = (unsigned int)(cbuf[0]);
	gt = (unsigned int)(cbuf[1]);
	rt = (unsigned int)(cbuf[2]);
	
	//depois para float
	*b = (int)bt;
	*g = (int)gt;
	*r = (int)rt;
	
	*b -= shift;
	*g -= shift;
	*r -= shift;
}//void PixelReader(Type *b,Type *g,Type *r)

//coloca em Matriz R,G,B a partir do arquivo
void BmpMatrixRead()
{
	int i, j;
	
	rewind(image_in);						//coloca ponteiro no inicio
	fseek(image_in, FILEHEADER.bfOffBits * sizeof(char), SEEK_SET);		//anda para os dados do arquivo
    
    
    int temp = mult4(INFOHEADER.biWidth);
    
    //Le a partir do arquivo matrix RGB
	for (i = 0; i < temp; i++)
	      	for (j = 0; j < INFOHEADER.biHeight; j++) 
			PixelReader(&BI[i][j], &GI[i][j], &RI[i][j]);

	
}//void BmpMatrix()



//funcionando para 24 bits e para 32 funciona porem ignora a matriz de transparencia
void PixelWrite(int*b, int *g, int *r)
{
	unsigned int lbuf[4];
	unsigned char cbuf[4];
	
        *b += shift;
	*g += shift;
	*r += shift;
	
    //pega as variaveis faz cast para int         
	lbuf[0] = (unsigned int)(*b);
	lbuf[1] = (unsigned int)(*g);
	lbuf[2] = (unsigned int)(*r);
	
	//pega as variaveis faz cast para unsigned char
	cbuf[0] = (unsigned char)(lbuf[0]);
	cbuf[1] = (unsigned char)(lbuf[1]);
	cbuf[2] = (unsigned char)(lbuf[2]);

    //grava no arquivo        
	fwrite(cbuf, sizeof(char),INFOHEADER.biBitCount/8, image_out);	//Divide pelo tamanho de 1 byte= 8 bits
	
}//void PixelWrite(Type *b,Type *g,Type *r)


//Salva Matriz RGB no arquivo BMP
void BmpMatrixSave()
{
	int i, j;
	int temp = mult4(INFOHEADER.biWidth);
          for (i = 0; i < temp; i++)
	      	for (j = 0; j < INFOHEADER.biHeight; j++) 
	      		PixelWrite(&BI[i][j], &GI[i][j], &RI[i][j]);	  // Salva pixels

}//void BmpSave()

//Coloca as informacoes da imagem nas matrizes RGB
int BmpRead(char* path,char *path2,int*** R,int ***G, int ***B){
    
   if(!BmpOpen(path)) {printf("Caminho invalido\n");return false;}
     else {
            if(0) {printf("Arquivo nao eh um BMP\n");return false;}
             else { 
                    
                    BmpFileSize(); // tamanho do arquivo
                    BmpReservado(); //reservado
                    BmpOffsetDados();  //offdados  
                    ReadInfoHeader(); //inforead
                    BmpMatrixRead(); //le as matrizes
                    BmpOldClose();
                    *R = RI;
                    *G = GI;
                    *B = BI;
                    };
                    }
  return true;   
}

//Monta a imagem de saida
int BmpSave(char * path , int **R, int **G,int **B){
    int i;
    if(!BmpNew(path)){printf("Nao foi possivel criar arquivo!\n"); return false;}
     else { 
          RI = R;
          GI = G;
          BI = B;
            BmpHeadOut();
          BmpMatrixSave();
            BmpNewClose();
          }
          
          return true;

}
