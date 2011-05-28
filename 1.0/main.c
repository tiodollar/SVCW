#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "BMP.h"
#include "Estruturas.h"

#define true 1
#define false 0
#define shift 128
#define fat 1.0f
#define TypeT Type
#define V1 8.0f
#define V2 12.0f
#define V3 16.0f
#define V4 32.0f
#define V5 64.0f
#define FOURCC(c0,c1,c2,c3) \
    ((unsigned int)c0 | (unsigned int)c1<<8 | (unsigned int)c2<<16 | (unsigned int)c3<<24)

typedef struct MainAVIHEADER{
      int DWSIZE;	
      int MicroSecPerFrame;
      int MaxBytePerSec;	
      int PaddingGranularity;
      int Flags;	
      int TotalFrames;
      int InitialFrames;
      int Streams;
      int SuggestedBufferSize;
      int Width;
      int Height;	
} MainAVIHEADER;


typedef struct AVISTREAMHEADER{
      int DWSIZE;	
      int fccType;
      int fccHandler;	
      int Flags;
      short int wPriority;	
      short int wLanguage;
      int InitialFrames;
      int Scale;
      int Rate;
      int Start;
      int Length;
      int SuggestedBufferSizer;
      int Quality;
      int SampleSize;
      short int Frame[4];			
} AVISTREAMHEADER;



typedef struct BITMAPINFOHEADER{
      int DWSIZE;	
      int biSize;
      int biWidth;	
      int biHeight;
      short int biPlanes;	
      short int biBitCount;
      int biCompression;
      int biSizeImage;
      int biXPelsPerMeter;
      int biYPelsPerMeter;
      int biClrUsed;
      int biClrImportant;
}BITMAPINFOHEADER;


typedef struct OFFSETS{
      int RIFF;
      int AVI0;
      int LIST1;
      int hdrl; 
      int avih;
      int LIST2;
      int STRL;
      int STRH;
      int STRF;
      int JUNK;
      int LIST3;
      int odml;
      int dmlh;
      int JUNK2;
      int LIST4;
      int MOVI;
      int IDX1;
}OFFSETS;


typedef struct INDEX{
  int ckid;
  int dwFlags;
  int dwChunkOffset;
  int dwChunkLength;
}INDEX;





int  _waveLength = 2, tjan;

float* _coeffs, _scales;

float **MQ, fator1q = -1, fator2q = -1, fatorf = -1,
      sqrt_3, denom, h0, h1, h2, h3, g0, g1, g2, g3, Ih0, Ih1, Ih2, Ih3, Ig0, Ig1, Ig2, Ig3,
      h[2], g[2], Ig[2], fator;



void impr_arq_origem(void)
{
	printf("\narquivo de origem\n\tarquivo imagem de origem, extensÃ£o bitmap (.avi ou .AVI)\n");

}//void impr_tam_origem(void)

void impr_arq_dest(void)
{
	printf("\narquivo de destino\n\tarquivo imagem de destino, extensÃ£o bitmap (.avi ou .AVI)\n");

}//void impr_tam_origem(void)

void impr_tam_bloco(void)
{
	printf("\ntamanho do macrobloco\n\tprecisa ser uma potencia de 2, variando de 8 a 256.\n\tvalores possÃ­veis: 8, 16, 32, 64, 128 e 256.\n");

}//void impr_tam_origem(void)

void impr_amostragem(void)
{
	printf("\namostragem YUV\n\t444 - processa 3 canais\n\t422 - processa 2 canais (Y e U/V) - reducao de U e V horizontal\n\t440 - processa 2 canais (Y e U/V) - reducao de U e V vertical\n");

}//void impr_tam_origem(void)

void impr_perfis(void)
{
	printf("\nperfis de compressao\nnq - nÃ£o utiliza a matriz de quantizaÃ§Ã£o - maior qualidade e menor compressÃ£o\n\tUtilize: nq=x\n\tonde x varia de 0 Ã  100.\n");
	printf("\nq - utiliza a matriz de quantizaÃ§Ã£o - menor qualidade e maior compressÃ£o\n\tUtilize: q=y\n\tonde y varia de 0 Ã  100.\n");
	printf("\n\t0   - qualidade mÃ­nima (maior compressao)\n\t100 - qualidade maxima (menor compressao)\n");

}//void impr_tam_origem(void)

void impr_uso(void)
{
	printf("\nUtilize:\nhaar_comp <arquivo de origem> <arquivo de destino> <tamanho do macrobloco> <amostragem YUV> <perfil de compressao>\n");
	impr_arq_origem();
	impr_arq_dest();
	impr_tam_bloco();
	impr_amostragem();
	impr_perfis();
	printf("\n");

}//void impr_uso(void)



void MatrizQ()
{
	int	i, j, x, y, py,
		jan = tjan / 4,
		cont = 1;

	float	passo = 1,
		val1 = V1,
		val2 = V2,
		val3 = V3,
		val4 = V4,
		val5 = V5;

	for (i = 1; i <= jan + 1; i++)
	{
    		x = i - 1;
    		y = 0;
    
    		if (cont == 2)
        		val1 = val1 - 1;
        
    		if (cont == 3)
        		val1 = val1 + 0.5;
    
    		if (cont == 4)
        		val1 = val1 - 0.75;//- 0.25
    
    		while (x >= 0)
		{
        		MQ[x][y] = val1;
        		x--;
        		y++;

		}//while (x > 0)
        
	    	if (cont == 4)
		{
       			cont = 1;
		        val1 = V1 - 0.25 * passo; //8 + 0.25 * passo
		        passo = passo + 1;
		}
		else
		        cont = cont + 1;

	}//for (i = 1; i < jan; i++)

	py = 1;

	for (i = jan + 1; i <= jan * 2; i++)
	{
    		x = jan;
    		y = py;
    		py = py + 1;
    
    		if (cont == 2)
        		val1 = val1 - 1;
        
		if (cont == 3)
        		val1 = val1 + 0.5;
    
		if (cont == 4)
			val1 = val1 - 0.75;// - 0.25
    
	    	while (y <= jan)
		{
        		MQ[x][y] = val1;
		        x--;
		        y++;

		}//while (y <= jan)
        
		if (cont == 4)
		{
        		cont = 1;
        		val1 = V1 - 0.25 * passo;//8 + 0.25 * passo
        		passo = passo + 1;
		}
		else
			cont = cont + 1;
    
	}//for (i = jan + 1; i < jan * 2; i++)

	//montando bandas baixas e baixas frequencias
	for (i = 0; i <= jan; i++)
    		for (j = jan; j < 2 * jan; j++)
		{
        		MQ[i][j] = val2;
        		MQ[j][i] = val2;

		}//for (j = jan: j < 2 * jan: j++)

	for (i = jan; i <= 2 * jan; i++)
    		for (j = jan; j < 2 * jan; j++)
        		MQ[i][j] = val3;

	//montando altas frequencias e mix
	for (i = 0; i <= 2 * jan; i++)
    		for (j = 2 * jan; j < 4 * jan; j++)
		{
        		MQ[i][j] = val4;
        		MQ[j][i] = val4;

    		}//for (j = 2 * jan; i < 4 * jan; j++)

	for (i = 2 * jan; i <= 4 * jan; i++) 
		for (j = 2 * jan; j < 4 * jan; j++)
        		MQ[i][j] = val5;

}//void MatrizQ(float **m)


//funcao que retorna em um float o tempo em milisegundos
float dbtime()
{
	float time;

	time = (float)clock() / (float)CLOCKS_PER_SEC * 1000;
	return time;

}//float dbtime()



//atualiza altura e largura do bmp
void sets(int alt,int larg,int *ni,int *nj)
{	
	int temp_ni;		//temporario linha
	int temp_nj;		//Temporario coluna
        
	temp_ni = alt  % tjan;
	temp_nj = larg % tjan;

	if (temp_ni == 0)
		*ni = alt / tjan;
	else
		*ni = (alt - temp_ni) / tjan + 1;

	if (temp_nj == 0)
		*nj = larg / tjan;
	else
		*nj = (larg - temp_nj) / tjan + 1;




	
}//void BmpSize()

float* haar(float* a, int n, int isign,float*out)
{
	int i, j, nh;
	//float *out;
	
	//out = (float*)malloc(sizeof(float)*n);
        
        if(out == NULL)printf("AQUI\n");
	nh = n / 2;
	
	if(n >= 4)
	{
		if(isign >= 0)
		{
			for(i = 0,j=0; i < n / 2; j+=2,i++){

         			out[i] = a[j] * h[1] + a[j+1] * h[0];
	 			out[i+nh] = a[j] * g[0] + a[j+1] * g[1];

        		}//for(i = 0,j=0; i < n / 2; j+=2,i++)

		}//if (isign >= 0)

		else
		{
			for(i = 0, j = 0; i < n;j++)
			{
	
				out[i++] = a[j] * Ig[0] + a[j + nh] * Ig[1];
				out[i++] = a[j] * h[0] + a[j + nh] * h[1];
	
			}//for(i = 0, j = 0; i < n;j++)

		}//else if (isign >= 0)
	
		return out;

	}//if(n >= 4)
	else
		{ return a;}

}//float* haar(float* a, int n, int isign)


float* daub2(float* a, int n, int isign,float *out)
{
	int i, j, nh;
	
        
	nh = n / 2;
	
	if(n >= 4)
	{
		if(isign >= 0)
		{
			for (i = 0, j = 0; j < n - 3; j += 2, i++)
			{
				out[i]    = a[j]*h0 + a[j+1]*h1 + a[j+2]*h2 + a[j+3]*h3;
				out[i+nh] = a[j]*g0 + a[j+1]*g1 + a[j+2]*g2 + a[j+3]*g3;
			
			}//for (i = 0, j = 0; j < n-2; j += 2, i++)

			out[i]    = a[n-2]*h0 + a[n-1]*h1 + a[0]*h2 + a[1]*h3;
			out[i+nh] = a[n-2]*g0 + a[n-1]*g1 + a[0]*g2 + a[1]*g3;

		}//if (isign >= 0)

		else
		{
			out[0] = a[nh-1]*Ih0 + a[nh]*Ih3 + a[0]*Ih2 + a[n-1]*Ih1;
			out[1] = a[nh-1]*Ig0 + a[nh]*Ig3 + a[0]*Ig2 + a[n-1]*Ig1;

			for (i = 0, j = 2; i < nh - 1; i++)
			{
				out[j++] = a[i]*Ih0 + a[i+nh]*Ih1 + a[i+1]*Ih2 + a[i+nh+1]*Ih3;
				out[j++] = a[i]*Ig0 + a[i+nh]*Ig1 + a[i+1]*Ig2 + a[i+nh+1]*Ig3;
			
			}//for (i = 0, j = 2; i < nh-1; i++) 

		}//else if (isign >= 0)
	
		return out;

	}//if(n >= 4)
	else
		{ return a; }

}//float* daub2(float* a, int n, int isign)


float* wt1d(float* in, int n, int isign,float*out2,float*aux,float*out)
{
      int i, j;
     // float  *out;

	 
	  //out = alocaVetf(n);
	
      if (n >= 4)
      {
         if (isign >= 0)
         {               
			aux = daub2(in, n, isign,out2);
			//aux = haar(in,n,isign,out2);
			//aux = forward(in,n);
			for (j = 0; j < n; j++)
				out[j] = aux[j];


			
			for (i = n / 2; i >= 4; i /= 2)
			{
				aux = daub2(out, i, isign,out2);
				//aux = haar(out,i,isign,out2);
			       //aux =  forward(out,i);
				for(j = 0; j < i ; j++)
					out[j] = aux[j];
			}
         
		 }//if (isign >= 0)  
  
         else
         {   
             aux = daub2(in, 4, isign,out2);
	    // aux = haar(in,4,isign,out2);
	   //aux = reverse(in,4);		
             for (j = 0; j < 4; j++)
                in[j] = aux[j];
	 		
			 for (i = 8; i <= n; i *= 2)
			 {
			    aux = daub2(in, i, isign,out2);
			    //aux = haar(in,i,isign,out2);	 
			    //aux = reverse(in,i);					
				for(j = 0; j < i ; j++)
				   in[j] = aux[j];
				 
			 }//for (nn=4; nn<n; nn<<=1)

			 for (j = 0; j < n; j++)
				 out[j] = in[j];
			 
            }//else if (isign >= 0)

        }//if (n >= 4)
      //  free(aux);
	//aux = NULL;
	return out;

}//float* wt1d(float* in, float* out, int n, int isign)


float** wt2d (float** in, int n, int isign,float** out,float* out2,float*aux)
{
	int l, i, j;
	
	
	
	for(l = 0; l < n; l++)
		 wt1d(in[l], n, isign,out2,aux,out[l]);
	
	//transpoe
	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			in[j][i] = out[i][j];
	

	for (l = 0; l < n; l++)
       	 wt1d(in[l], n, isign,out2,aux,out[l]);
	//transpoe
	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			in[j][i] = out[i][j];

	for(i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			out[i][j] = in[i][j];
	
	return out;

}//void wt2d (float** in, int n, int isign)


float arredondamento(float x){

	float y = x - ceil(x);
	float z = x - floor(x);
	if( y == 0)return x;
	else if(fabs(y) > fabs(z)) return floor(x);
		else return ceil(x);

}

void Macroblocos(float****YT,float****UT,float****VT,int****MYint,int****MUint,int****MVint,int ni,int nj){

 int i,j;
 YT = (float ****)malloc((nj + 1) * sizeof(float ****));
  UT = (float ****)malloc((nj + 1) * sizeof(float ****));
  VT = (float ****)malloc((nj + 1) * sizeof(float ****));

  MYint = (int ****)malloc((nj + 1) * sizeof(int ****));
  MUint = (int ****)malloc((nj + 1) * sizeof(int ****));
  MVint = (int ****)malloc((nj + 1) * sizeof(int ****));

  for(i = 0; i <= nj; i++)//passar ni e nj
   {
     YT[i] = (float ***)malloc((ni + 1) * sizeof(float ***));
     UT[i] = (float ***)malloc((ni + 1) * sizeof(float ***));
     VT[i] = (float ***)malloc((ni + 1) * sizeof(float ***));

     MYint[i] = (int ***)malloc((ni + 1) * sizeof(int ***));
     MUint[i] = (int ***)malloc((ni + 1) * sizeof(int ***));
     MVint[i] = (int ***)malloc((ni + 1) * sizeof(int ***));

  }//for(i = 0; i < ni + 1; i++)
 for(j = 0; j < nj; j++)
  for(i = 0; i < ni; i++)
   {
     YT[j][i] = alocaMatf(tjan, tjan);
     UT[j][i] = alocaMatf(tjan, tjan);
     VT[j][i] = alocaMatf(tjan, tjan);
     MYint[j][i] = alocaMatint(tjan, tjan);
     MUint[j][i] = alocaMatint(tjan, tjan);
     MVint[j][i] = alocaMatint(tjan, tjan);
  }//for(i = 0; i < ni; i++)

}


void RGBYUV(int larg,int alt,int** R,int **G,int**B,float** Y,float **U,float **V){
  int i,j;
    for(j = 0; j < alt; j++)
	for(i = 0; i < larg; i++)
			{     
    				Y[j][i] = 0.299f * ((float)(R[j][i] - shift)) / 255.0f  + 0.587f * ((float)(G[j][i] - shift)) / 255 + 0.114f * ((float)(B[j][i] - shift)) / 255.0f;
	    			U[j][i] = 0.565f * (((float)(B[j][i] - shift)) / 255.0f - Y[j][i]);
				V[j][i] = 0.713f * (((float)(R[j][i] - shift)) / 255.0f - Y[j][i]);
                               

			}//for(j = 0; j < file_in.larg; j++)
}


//funcao principal
void db2_compressao(char** argv,int**R,int**G,int**B,int alt,int larg,int ni,int nj,float** Y, float** U,float** V,float** UV,float** OUTY,float** OUTU,float** OUTV,float** OUTUV,float**** YT,float****UT,float****VT,int****MYint,int****MUint,int****MVint,float **tempY, float**tempU,float **tempV, float**tempY2,float **tempU2,float **tempV2,float** OR,float**OG,float** OB,float**out,float* out2,float* aux)
{

  float time_ini, time_end;
  int i, j, a, l, k;
  long int kY, kU, kV;

  
  FILE* arq_temp = NULL;
  FILE* arq_comp = NULL;



  
 

  


  fator = sqrt(2) / 2.0f;		
  h[0] = (1.0f)*fator;
  h[1] = (1.0f)*fator;
  g[0] = (-1.0f)*fator;
  g[1] = (1.0f)*fator;
  Ig[0] = (1.0f)*fator;
  Ig[1] = (-1.0f)*fator;
  

  sqrt_3 = sqrt( 3 );
  denom = 4 * sqrt( 2 );
  h0 = (1 + sqrt_3)/denom;
  h1 = (3 + sqrt_3)/denom;
  h2 = (3 - sqrt_3)/denom;
  h3 = (1 - sqrt_3)/denom;
  g0 =  h3;
  g1 = -h2;
  g2 =  h1;
  g3 = -h0;
  Ih0 = h2;
  Ih1 = g2;  // h1
  Ih2 = h0;
  Ih3 = g0;  // h3
  Ig0 = h3;
  Ig1 = g3;  // -h0
  Ig2 = h1;
  Ig3 = g1;  // -h2



 

 
  for(j = 0; j < alt; j++)
	for(i = 0; i < larg; i++)
			{     
    				Y[j][i] = 0.299f * ((float)(R[j][i] - shift)) / 255.0f  + 0.587f * ((float)(G[j][i] - shift)) / 255 + 0.114f * ((float)(B[j][i] - shift)) / 255.0f;
	    			U[j][i] = 0.565f * (((float)(B[j][i] - shift)) / 255.0f - Y[j][i]);
				V[j][i] = 0.713f * (((float)(R[j][i] - shift)) / 255.0f - Y[j][i]);
                               
};



	//printf("Realizou conversao YUV...\n");
	
   if(!strcmp("444", argv[4]))
    {		
      for(j = 0; j < nj; j++)
	for(i = 0; i < ni; i++)
	  for(l = 0; l < tjan; l++)
	   for(a = 0; a < tjan; a++)
	     {
	       YT[j][i][l][a] = Y[l + j * tjan][a + i * tjan];
	       UT[j][i][l][a] = U[l + j * tjan][a + i * tjan];
	       VT[j][i][l][a] = V[l + j * tjan][a + i * tjan];
             }
    //    printf("aplicou transformada...\n");
       //aplicacao da transformada
       for(j = 0; j < nj; j++)
	for(i = 0; i < ni; i++)
	 {
	   for(l = 0; l < tjan; l++)
	    {
	      for(a = 0; a < tjan; a++)
	       {
		 tempY[l][a] = YT[j][i][l][a];
		 tempU[l][a] = UT[j][i][l][a];
		 tempV[l][a] = VT[j][i][l][a];
		 //printf("(%f %f %f)\t", tempY[l][a], tempU[l][a], tempV[l][a]);
                }//for(a = 0; a < tjan; a++)
              //printf("\n");
	    }
 //printf("aplicou transformada...aui?\n");
	wt2d(tempY, tjan, 1,tempY2,out2,aux);
	wt2d(tempU, tjan, 1,tempU2,out2,aux);
	wt2d(tempV, tjan, 1,tempV2,out2,aux);
 //printf("aplicou transformada...\n");
          for(l = 0; l < tjan; l++)
	    { // printf("aqui...\n...");
	      for(a = 0; a < tjan; a++)
		{  
		  YT[j][i][l][a] = tempY2[l][a];
		  UT[j][i][l][a] = tempU2[l][a];
		  VT[j][i][l][a] = tempV2[l][a];
		  //printf("(%f %f %f)\t", tempY[l][a], tempU[l][a], tempV[l][a]);					
		}//for(a = 0; a < tjan; a++)
           //   printf("\n");
            }

	}//for(j = 0; a < nj; a++)
      
        printf(".");
	if (fator1q != -1)
	 {
	   //quantizacao
	   for(j = 0; j < nj; j++)
    	    for(i = 0; i < ni; i++)
	     for(l = 0; l < tjan; l++)
	      for(a = 0; a < tjan; a++)
	       {
		 MYint[j][i][l][a]  = (int)(YT[j][i][l][a] * fator1q / MQ[l][a]);
		 MUint[j][i][l][a]  = (int)(UT[j][i][l][a] * fator2q / MQ[l][a]);
		 MVint[j][i][l][a]  = (int)(VT[j][i][l][a] * fator2q / MQ[l][a]);		
		}//for(a = 0; a < tjan; a++)    
	    //passa pra float
            for(j = 0; j < nj; j++)
	     for(i = 0; i < ni; i++)
	      for(l = 0; l < tjan; l++)
	       for(a = 0; a < tjan; a++)
		{
		  YT[j][i][l][a]  = (float)MYint[j][i][l][a] * MQ[l][a] / fator1q;
		  UT[j][i][l][a]  = (float)MUint[j][i][l][a] * MQ[l][a] / fator2q;
		  VT[j][i][l][a]  = (float)MVint[j][i][l][a] * MQ[l][a] / fator2q;
                }//for(a = 0; a < tjan; a++)
	 }//if (!strcmp("q", argv[4]))
	else
	 {
           
  //      printf("Nao quantizado...\n");
	   //nao quantizados
	   for(j = 0; j < nj; j++)
	    for(i = 0; i < ni; i++)
	     for(l = 0; l < tjan; l++)
	      for(a = 0; a < tjan; a++)
		{
		  MYint[j][i][l][a]  = (int)(YT[j][i][l][a] * fatorf);
		  MUint[j][i][l][a]  = (int)(UT[j][i][l][a] * fatorf);
		  MVint[j][i][l][a]  = (int)(VT[j][i][l][a] * fatorf);
		}//for(a = 0; a < tjan; a++)
           //passa pra float
	   for(j = 0; j < nj; j++)
	    for(i = 0; i < ni; i++)
	     for(l = 0; l < tjan; l++)
	      for(a = 0; a < tjan; a++)
		{
		  YT[j][i][l][a]  = (float)MYint[j][i][l][a] / fatorf;	
		  UT[j][i][l][a]  = (float)MUint[j][i][l][a] / fatorf;
		  VT[j][i][l][a]  = (float)MVint[j][i][l][a] / fatorf;
		}//for(a = 0; a < tjan; a++)

	}//else if (!strcmp("q", argv[4]))

       //printf("Realizou transformada...\n");
	//printf("aqui\n");			
	//aplicacao da transformada inversa
	for(j = 0; j < nj; j++)
	 for(i = 0; i < ni; i++)
	  {
	    for(l = 0; l < tjan; l++)
	     {
		for(a = 0; a < tjan; a++)
		 {
		   tempY[l][a] = YT[j][i][l][a];
		   tempU[l][a] = UT[j][i][l][a];
		   tempV[l][a] = VT[j][i][l][a];
		   //printf("(%f %f %f)\t", tempY[l][a], tempU[l][a], tempV[l][a]);
                }//for(a = 0; a < tjan; a++)			
	      }//for(l = 0; l < tjan; l++)

	 //printf("\n");

	 wt2d(tempY, tjan, -1,tempY2,out2,aux);
	 wt2d(tempU, tjan, -1,tempU2,out2,aux);
	 wt2d(tempV, tjan, -1,tempV2,out2,aux);

	for(l = 0; l < tjan; l++)
	 for(a = 0; a < tjan; a++)
	  {
	   //printf("mad");
	   YT[j][i][l][a] = tempY2[l][a];
	   UT[j][i][l][a] = tempU2[l][a];
	   VT[j][i][l][a] = tempV2[l][a];
	   //printf("%f %f %f\n", temp2R[l][a], temp2G[l][a], temp2B[l][a]);
          }//for(a = 0; a < tjan; a++)
	}//for(i = 0; i < ni; i++)
          //             printf("Realizou inversa...\n");
	for(j = 0; j < nj; j++)
	  for(i = 0; i < ni; i++)
	   for(l = 0; l < tjan; l++)
	    for(a = 0; a < tjan; a++)
	      { 
		  Y[l + j * tjan][a + i * tjan] = YT[j][i][l][a];
		  U[l + j * tjan][a + i * tjan] = UT[j][i][l][a];
		  V[l + j * tjan][a + i * tjan] = VT[j][i][l][a];
              }

	}//if(strcmp("444", argv[3])

		else
		if(!strcmp("440", argv[4]) || !strcmp("422", argv[4]))
		{

			if(!strcmp("440", argv[4]))
			{		
				for(j = 0; j < nj * tjan; j++)
				{
					k = 0;
					for(i = 0; i < ni * tjan / 2; i++)
					{

						UV[j][i] = (U[j][k] + U[j][k + 1]) / 2;
						UV[j][i + ni * tjan / 2] = (V[j][k] + V[j][k + 1]) / 2;
						k += 2;

					}//for(j = 0; j < file_in.larg; j++)

				}//for(j = 0; j < file_in.larg; j++)

			}//if(!strcmp("440", argv[4]))

			else //422
			{
				for(i = 0; i < ni * tjan; i++)
				{
					k = 0;
					for(j = 0; j < nj * tjan / 2; j++)
					{
						UV[j][i] = (U[k][i] + U[k + 1][i]) / 2;
						UV[j + nj * tjan / 2][i] = (V[k][i] + V[k + 1][i]) / 2;
						k += 2;

					}//for(j = 0; j < file_in.larg; j++)

				}//for(j = 0; j < file_in.larg; j++)

			}//else if(!strcmp("440", argv[4]))

			for(j = 0; j < nj; j++)
				for(i = 0; i < ni; i++)
					for(l = 0; l < tjan; l++)
						for(a = 0; a < tjan; a++)
						{
								YT[j][i][l][a] = Y[l + j * tjan][a + i * tjan];
								UT[j][i][l][a] = UV[l + j * tjan][a + i * tjan];
	
						}//if((l + i * tjan) < file_in.larg && (a + j * tjan)...

//			printf("\nAntes da transf\n");

			//aplicacao da transformada
			for(j = 0; j < nj; j++)
				for(i = 0; i < ni; i++)
				{
					for(l = 0; l < tjan; l++)
						for(a = 0; a < tjan; a++)
						{
							tempY[l][a] = YT[j][i][l][a];
							tempU[l][a] = UT[j][i][l][a];
							//if (l == 0)printf("a(%f %f)\t", tempY[l][a], tempU[l][a]);

						}//for(a = 0; a < tjan; a++)
					
					//printf("\n");
					
					wt2d(tempY, tjan, 1,tempY2,out2,aux);
					wt2d(tempU, tjan, 1,tempU2,out2,aux);

					for(l = 0; l < tjan; l++)
						for(a = 0; a < tjan; a++)
						{
							YT[j][i][l][a] = tempY2[l][a];
							UT[j][i][l][a] = tempU2[l][a];
							//if (l == 0)printf("d(%f %f)\t", tempY2[l][a], tempU2[l][a]);

						}//for(a = 0; a < tjan; a++)
					
					//printf("\n");

				}//for(j = 0; a < nj; a++)
			
			if (fator1q != -1)
			{
				//passa pra int
				for(j = 0; j < nj; j++)
		      		for(i = 0; i < ni; i++)
						for(l = 0; l < tjan; l++)
							for(a = 0; a < tjan; a++)
							{
								MYint[j][i][l][a]  = (int)(YT[j][i][l][a] * fator1q / MQ[l][a]);
								MUint[j][i][l][a]  = (int)(UT[j][i][l][a] * fator2q / MQ[l][a]);

							}//for(a = 0; a < tjan; a++)
		
				//passa pra float
				for(j = 0; j < nj; j++)
			      	for(i = 0; i < ni; i++)
						for(l = 0; l < tjan; l++)
							for(a = 0; a < tjan; a++)
							{
								YT[j][i][l][a] = (float)MYint[j][i][l][a] * MQ[l][a] / fator1q;
								UT[j][i][l][a] = (float)MUint[j][i][l][a] * MQ[l][a] / fator2q;

							}//for(a = 0; a < tjan; a++)

			}//if (!strcmp("q", argv[4]))
			else
			{
				//passa pra int
				for(j = 0; j < nj; j++)
		      		for(i = 0; i < ni; i++)
						for(l = 0; l < tjan; l++)
							for(a = 0; a < tjan; a++)
							{
								MYint[j][i][l][a]  = (int)(YT[j][i][l][a] * fatorf);
								MUint[j][i][l][a]  = (int)(UT[j][i][l][a] * fatorf);

							}//for(a = 0; a < tjan; a++)
		
				//passa pra float
				for(j = 0; j < nj; j++)
			      	for(i = 0; i < ni; i++)
						for(l = 0; l < tjan; l++)
							for(a = 0; a < tjan; a++)
							{
								YT[j][i][l][a]  = (float)MYint[j][i][l][a] / fatorf;
								UT[j][i][l][a]  = (float)MUint[j][i][l][a] / fatorf;

							}//for(a = 0; a < tjan; a++)

			}//else if (!strcmp("q", argv[4]))

	//		printf("\nAntes da transf inversa\n");

			//aplicacao da transformada inversa
			for(j = 0; j < nj; j++)
				for(i = 0; i < ni; i++)
				{
					for(l = 0; l < tjan; l++)
						for(a = 0; a < tjan; a++)
						{
							tempY[l][a] = YT[j][i][l][a];
							tempU[l][a] = UT[j][i][l][a];
							//if (l == 0)printf("a(%f %f)\t", tempY[l][a], tempU[l][a]);

						}//for(a = 0; a < tjan; a++)

					//printf("\n");

					wt2d(tempY, tjan, -1,tempY2,out2,aux);
					wt2d(tempU, tjan, -1,tempU2,out2,aux);

					for(l = 0; l < tjan; l++)
						for(a = 0; a < tjan; a++)
						{
							YT[j][i][l][a] = tempY2[l][a];
							UT[j][i][l][a] = tempU2[l][a];
							//if (l == 0)printf("d(%f %f)\t", tempY[l][a], tempU[l][a]);

						}//for(a = 0; a < tjan; a++)

					//printf("\n");

				}//for(i = 0; i < ni; i++)

			for(j = 0; j < nj; j++)
				for(i = 0; i < ni; i++)
					for(l = 0; l < tjan; l++)
						for(a = 0; a < tjan; a++)
						{ 
							Y[l + j * tjan][a + i * tjan] = YT[j][i][l][a];
							UV[l + j * tjan][a + i * tjan] = UT[j][i][l][a];

						}//if ((l + i  * tjan) < file_out.larg && (a + j * tjan) < file_out.alt)

			if(!strcmp(argv[4], "440"))
			{
			  for(j = 0; j < nj * tjan; j++)
			  {
				k = 0;
				for (i = 0; i < ni * tjan / 2; i++)
				{
					U[j][k] = U[j][k + 1] = UV[j][i];
					V[j][k] = V[j][k + 1] = UV[j][i + ni * tjan / 2];
					k += 2;

				}//for (i = 0; i < file_in.alt; i++)

			  }//for(j = 0; j < file_in.larg; j++)

			}// if(strcmp(argv[4], "444")

			else //422
			{
			k = 0;
			for (j = 0; j < nj * tjan / 2; j++)  
			  {
				
				for(i = 0; i < ni * tjan; i++)
				{
				   U[k][i]  = U[k+1][i] = UV[j][i];
				   V[k][i]  = V[k+1][i] =  UV[j + nj * tjan / 2][i];
				   k+=2;

				}//for (i = 0; i < file_in.alt; i++)

			  }//for(j = 0; j < file_in.larg; j++)

			}//else if(strcmp(argv[4], "444")
			
		}//else if(strcomp("444", argv[3])

    	//	printf("Retornando para RGB...\n");
  
		for(j = 0; j < larg; j++)
	      	for(i = 0; i < alt; i++)
			{
    			OR[j][i] = 255.0f* (Y[j][i] + 1.403f * V[j][i]) + shift;
	    		OG[j][i] = 255.0f* (Y[j][i] - 0.344f * U[j][i] - 0.714f * V[j][i]) + shift;
			OB[j][i] = 255.0f* (Y[j][i] + 1.77f  * U[j][i]) + shift;

				if(OR[j][i] < 0.0f)
					OR[j][i] = 0.0f;//abs(file_out.R[j][i]);

				if(OB[j][i] < 0.0f)
					OB[j][i] = 0.0f;//abs(file_out.B[j][i]);

				if(OG[j][i] < 0.0f)
					OG[j][i] = 0.0f;//abs(file_out.G[j][i]);

				if(OR[j][i] > 255.0f)
					OR[j][i] = 255.0f;
				
				if(OB[j][i] > 255.0f)
					OB[j][i] = 255.0f;

				if(OG[j][i] > 255.0f)
					OG[j][i] = 255.0f;

				R[j][i] =arredondamento(OR[j][i]);
				G[j][i] = arredondamento(OG[j][i]);
				B[j][i] = arredondamento(OB[j][i]);

			
			}//for(j = 0; j < file_in.alt; j++)


		

	    

}//void db2_compressao(char** argv)



void fail(char* str){

 printf("%s\n",str);
 exit(0);

}


char * FOURCC2str( int fourcc )
{
    char * mystr=(char*)malloc(5);
    mystr[0]=(char)((fourcc    )&255);
    mystr[1]=(char)((fourcc>> 8)&255);
    mystr[2]=(char)((fourcc>>16)&255);
    mystr[3]=(char)((fourcc>>24)&255);
    mystr[4]=0;
    return mystr;
}

void printBitMapHeader(BITMAPINFOHEADER BITMAPHEADER){
    	printf("------BITMAP HEADER------\n");
	printf("DWSIZE:%d\n",BITMAPHEADER.DWSIZE);
	printf("biSize:%d\n",BITMAPHEADER.biSize);
	printf("biWidth:%d\n", BITMAPHEADER.biWidth);
	printf("biHeight:%d\n",BITMAPHEADER.biHeight);
	printf("biPlanes:%d\n",BITMAPHEADER.biPlanes);
	printf("biBitCount:%d\n",BITMAPHEADER.biBitCount);
	printf("biCompression:%d\n",BITMAPHEADER.biCompression);
	printf("biSizeImage:%d\n",BITMAPHEADER.biSizeImage);
	printf("biXPelsPerMeter:%d\n",BITMAPHEADER.biXPelsPerMeter);
	printf("biYPelsPerMeter:%d\n",BITMAPHEADER.biYPelsPerMeter);
	printf("biClrUsed:%d\n",BITMAPHEADER.biClrUsed);
	printf("biClrImportant:%d\n",BITMAPHEADER.biClrImportant);
}


void printStreamHeader(AVISTREAMHEADER STREAMHEADER){
	printf("------STRH------\n");
	printf("DWSIZE:%d\n",STREAMHEADER.DWSIZE);
	printf("fccType:%s\n",FOURCC2str(STREAMHEADER.fccType));
	printf("fccHandler:%s\n",FOURCC2str(STREAMHEADER.fccHandler));
	printf("Flags:%d\n",STREAMHEADER.Flags);
	printf("wPriority:%d\n",STREAMHEADER.wPriority);
	printf("wLanguages:%d\n",STREAMHEADER.wLanguage);
	printf("InitialFrames:%d\n",STREAMHEADER.InitialFrames);
	printf("Scale:%d\n",STREAMHEADER.Scale);
	printf("Rate:%d\n",STREAMHEADER.Rate);
	printf("Start:%d\n",STREAMHEADER.Start);
	printf("Length:%d\n",STREAMHEADER.Length);
	printf("SuggestedBufferSizer:%d\n",STREAMHEADER.SuggestedBufferSizer);
	printf("Quality:%d\n",STREAMHEADER.Quality);
	printf("SampleSize:%d\n",STREAMHEADER.SampleSize);
        
}

void printHEADER(MainAVIHEADER AVIHEADERINFO){
	printf("------MAIN AVI------\n");
	printf("DWSIZE:%d\n",AVIHEADERINFO.DWSIZE);
	printf("MicroSecPerFrame:%d\n",AVIHEADERINFO.MicroSecPerFrame);
	printf("MaxBytePerSec:%d\n",AVIHEADERINFO.MaxBytePerSec);
	printf("PaddingGranularity:%d\n",AVIHEADERINFO.PaddingGranularity);
	printf("Flags:%d\n",AVIHEADERINFO.Flags);
	printf("TotalFrames:%d\n",AVIHEADERINFO.TotalFrames);
	printf("InitialFrames:%d\n",AVIHEADERINFO.InitialFrames);
	printf("Streams:%d\n",AVIHEADERINFO.Streams);
	printf("SuggestedBufferSize:%d\n",AVIHEADERINFO.SuggestedBufferSize);
	printf("Width:%d\n",AVIHEADERINFO.Width);
	printf("Height:%d\n",AVIHEADERINFO.Height);
}

int search(FILE* AVIFILE,int ID){
int teste = 0;
int buffer;
while(teste == 0 && !feof(AVIFILE)){
 
 fread (&buffer,sizeof(int),1,AVIFILE);
 

 if(ID == buffer)	
	{
	//printf("TESTE:%s\n",FOURCC2str(buffer));
	return 1;
	}
  }
 return 0;
}

void writeChunk(FILE* AVIFILE,int ID,int offset){
 //incrementar funcao 
fseek(AVIFILE,offset,SEEK_SET);
fwrite(&ID,sizeof(int),1,AVIFILE);

}

void writeAllChunk(FILE* OUTFILE,OFFSETS OFFSETOUT, MainAVIHEADER HEADERINFO, AVISTREAMHEADER STREAMHEADER, BITMAPINFOHEADER BITMAPHEADER,int tam,int TAMLIST1,int TAMLIST2,int TAMLIST3,int TAMLIST4,int TAMLH,int TOTALFRAMES,int TAMJUNK,int TAMJUNK2){

  writeChunk(OUTFILE,FOURCC('R','I','F','F'),OFFSETOUT.RIFF);
  fwrite(&tam,sizeof(int),1,OUTFILE); 
  writeChunk(OUTFILE,FOURCC('A','V','I',' '),OFFSETOUT.AVI0);
  writeChunk(OUTFILE,FOURCC('L','I','S','T'),OFFSETOUT.LIST1);
  fwrite(&TAMLIST1,sizeof(int),1,OUTFILE); 
  writeChunk(OUTFILE,FOURCC('h','d','r','l'),OFFSETOUT.hdrl);
  writeChunk(OUTFILE,FOURCC('a','v','i','h'),OFFSETOUT.avih);
  fwrite(&HEADERINFO,sizeof(int),11,OUTFILE);
  writeChunk(OUTFILE,FOURCC('L','I','S','T'),OFFSETOUT.LIST2);
   fwrite(&TAMLIST2,sizeof(int),1,OUTFILE); 
  writeChunk(OUTFILE,FOURCC('s','t','r','l'),OFFSETOUT.STRL);
  writeChunk(OUTFILE,FOURCC('s','t','r','h'),OFFSETOUT.STRH);
  fwrite(&STREAMHEADER,sizeof(int),15,OUTFILE); 
  writeChunk(OUTFILE,FOURCC('s','t','r','f'),OFFSETOUT.STRF);
  fwrite(&BITMAPHEADER,sizeof(int),11,OUTFILE);
  writeChunk(OUTFILE,FOURCC('J','U','N','K'),OFFSETOUT.JUNK);
  fwrite(&TAMJUNK,sizeof(int),1,OUTFILE);
  writeChunk(OUTFILE,FOURCC('L','I','S','T'),OFFSETOUT.LIST3);
   fwrite(&TAMLIST3,sizeof(int),1,OUTFILE); 
  writeChunk(OUTFILE,FOURCC('o','d','m','l'),OFFSETOUT.odml);
  writeChunk(OUTFILE,FOURCC('d','m','l','h'),OFFSETOUT.dmlh);
   fwrite(&TAMLH, sizeof(int), 1, OUTFILE);
   fwrite(&TOTALFRAMES, sizeof(int), 1, OUTFILE);
  writeChunk(OUTFILE,FOURCC('J','U','N','K'),OFFSETOUT.JUNK2);
   fwrite(&TAMJUNK2,sizeof(int),1,OUTFILE);
  writeChunk(OUTFILE,FOURCC('L','I','S','T'),OFFSETOUT.LIST4);
   fwrite(&TAMLIST4,sizeof(int),1,OUTFILE); 
  writeChunk(OUTFILE,FOURCC('m','o','v','i'),OFFSETOUT.MOVI);

}

int openAVI(FILE* AVIFILE,FILE* OUTFILE,char* filename,char**argv){

 MainAVIHEADER HEADERINFO;
 AVISTREAMHEADER STREAMHEADER;
 BITMAPINFOHEADER BITMAPHEADER;
 OFFSETS          OFFSETOUT;
 int a=0,b=0;
 int TAMTOTAL,TAMLIST1,TAMLIST2,TAMLIST3,TAMLIST4,TAMJUNK,TAMJUNK2,TAMLH;
 a  =  ftell(AVIFILE);
 if(!search(AVIFILE,FOURCC('R','I','F','F')))fail("NÃ£o Ã© RIFF FILE?");
 b =  ftell(AVIFILE);
 OFFSETOUT.RIFF = b - sizeof(int);
 fread(&TAMTOTAL, sizeof(int), 1, AVIFILE);
 printf("T:%d\n",TAMTOTAL);	
 a = ftell(AVIFILE);
 if(!search(AVIFILE,FOURCC('A','V','I',' ')))fail("NÃ£o Ã© AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.AVI0 = b - sizeof(int);
 if(!search(AVIFILE,FOURCC('L','I','S','T')))fail("Cade o arquivo?");
 b = ftell(AVIFILE);
 OFFSETOUT.LIST1 = b - sizeof(int);
 fread(&TAMLIST1, sizeof(int), 1, AVIFILE);
 
 if(!search(AVIFILE,FOURCC('h','d','r','l')))fail("TEm certeza que Ã© AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.hdrl = b - sizeof(int);

 if(!search(AVIFILE,FOURCC('a','v','i','h')))fail("NÃ£o tem Header?");
 b = ftell(AVIFILE);
 OFFSETOUT.avih = b - sizeof(int);
 fread (&HEADERINFO,sizeof(int),12,AVIFILE);//LE AVIHEADER
 printHEADER(HEADERINFO);


 if(!search(AVIFILE,FOURCC('L','I','S','T')))fail("CADE ERRO DENOVO?");
 b = ftell(AVIFILE);
 OFFSETOUT.LIST2 = b - sizeof(int);
 fread(&TAMLIST2, sizeof(int), 1, AVIFILE);


 if(!search(AVIFILE,FOURCC('s','t','r','l')))fail("TEm certeza que Ã© AVI?");
 b = ftell(AVIFILE);
OFFSETOUT.STRL = b - sizeof(int);


 if(!search(AVIFILE,FOURCC('s','t','r','h')))fail("TEm certeza que Ã© AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.STRH = b - sizeof(int);

 fread (&STREAMHEADER,sizeof(int),15,AVIFILE);//LE STREAMHEADER
 printStreamHeader(STREAMHEADER);

 if(!search(AVIFILE,FOURCC('s','t','r','f')))fail("TEm certeza que Ã© AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.STRF = b - sizeof(int);

 fread (&BITMAPHEADER,sizeof(int),11,AVIFILE);//LE BitMAPSTREAMHEADER
 printBitMapHeader(BITMAPHEADER);


 if(!search(AVIFILE,FOURCC('J','U','N','K')))fail("Olhar direito, certeza que Ã© AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.JUNK = b - sizeof(int);
 fread(&TAMJUNK, sizeof(int), 1, AVIFILE);
 
 if(!search(AVIFILE,FOURCC('L','I','S','T')))fail("MErda certeza que Ã© AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.LIST3 = b - sizeof(int);
 fread(&TAMLIST3, sizeof(int), 1, AVIFILE);
 printf("%d\n",TAMLIST3);
 
 if(!search(AVIFILE,FOURCC('o','d','m','l')))fail("POha certeza que Ã© AVI?");
  b = ftell(AVIFILE);
 OFFSETOUT.odml = b - sizeof(int);

 if(!search(AVIFILE,FOURCC('d','m','l','h')))fail("Caramba certeza que Ã© AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.dmlh = b - sizeof(int);
 fread(&TAMLH, sizeof(int), 1, AVIFILE);

 if(!search(AVIFILE,FOURCC('J','U','N','K')))fail("Ish certeza que Ã© AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.JUNK2 = b - sizeof(int);
 fread(&TAMJUNK2, sizeof(int), 1, AVIFILE);

 if(!search(AVIFILE,FOURCC('L','I','S','T')))fail("Oras, nÃ£o deve ser AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.LIST4 = b - sizeof(int);
 fread(&TAMLIST4, sizeof(int), 1, AVIFILE);

 if(!search(AVIFILE,FOURCC('m','o','v','i')))fail("Oras, nÃ£o deve ser AVI?");
 b = ftell(AVIFILE);
 OFFSETOUT.MOVI = b - sizeof(int);

 //Salva dados em novo AVIFILE
 writeAllChunk(OUTFILE,OFFSETOUT,HEADERINFO,STREAMHEADER,BITMAPHEADER,TAMTOTAL,TAMLIST1,TAMLIST2,TAMLIST3,TAMLIST4,TAMLH,HEADERINFO.TotalFrames,TAMJUNK,TAMJUNK2);

 //Extrai Quadros...
 setBfOffBits(54);//SetOffSet
 setBfBiWidth(BITMAPHEADER.biWidth);//SetLargura
 setBfBiHeight(BITMAPHEADER.biHeight);//SetAltura
 setBitCount(BITMAPHEADER.biBitCount);//SetBitCount
 setbiSizeImage(BITMAPHEADER.biSizeImage);//SetSizeImage
 int temp = mult4(getBfBiWidth());
 int** R = alocaMatint(temp, getBfBiHeight());		
 int** G = alocaMatint(temp, getBfBiHeight());		
 int** B = alocaMatint(temp, getBfBiHeight());
 
 float** OR = alocaMatf(temp, getBfBiHeight());		
 float** OG = alocaMatf(temp, getBfBiHeight());		
 float** OB = alocaMatf(temp, getBfBiHeight());
 
 unsigned int bt, gt, rt;
 int tam,forcc;
 unsigned char cbuf[4];
 char *buffer;
  buffer = (char*) malloc(sizeof(char)*32);
 int i,j,k;
 float****YT;float****UT;float****VT;int****MYint;int****MUint;int****MVint;int ni;int nj;
 float **Y;float **U;float **V;float **UV;float **OUTY;float **OUTU;float **OUTV;float **OUTUV;
 float **tempY, **tempU, **tempV, **tempY2, **tempU2, **tempV2,**out;
 float* out2 =  alocaVetf(tjan);
 float* aux  = alocaVetf(tjan);
 sets(getBfBiHeight(),getBfBiWidth(),&ni,&nj);
 	out = alocaMatf(tjan, tjan);		
	Y = alocaMatf(nj * tjan, ni * tjan);		//Aloga Matriz Y
	U = alocaMatf(nj * tjan, ni * tjan);		//Aloga Matriz U
	V = alocaMatf(nj * tjan, ni * tjan);		//Aloga Matriz V
	UV = alocaMatf(nj * tjan, ni * tjan);		//Aloga Matriz UV
       
	OUTY = alocaMatf(nj * tjan, ni * tjan);		//Aloga Matriz Y
	OUTU = alocaMatf(nj * tjan, ni * tjan);		//Aloga Matriz U
	OUTV = alocaMatf(nj * tjan, ni * tjan);		//Aloga Matriz V
	OUTUV = alocaMatf(nj * tjan, ni * tjan);		//Aloga Matriz UV

	YT = (float ****)malloc((nj + 1) * sizeof(float ****));
  UT = (float ****)malloc((nj + 1) * sizeof(float ****));
  VT = (float ****)malloc((nj + 1) * sizeof(float ****));

  MYint = (int ****)malloc((nj + 1) * sizeof(int ****));
  MUint = (int ****)malloc((nj + 1) * sizeof(int ****));
  MVint = (int ****)malloc((nj + 1) * sizeof(int ****));

  for(i = 0; i <= nj; i++)//passar ni e nj
   {
     YT[i] = (float ***)malloc((ni + 1) * sizeof(float ***));
     UT[i] = (float ***)malloc((ni + 1) * sizeof(float ***));
     VT[i] = (float ***)malloc((ni + 1) * sizeof(float ***));

     MYint[i] = (int ***)malloc((ni + 1) * sizeof(int ***));
     MUint[i] = (int ***)malloc((ni + 1) * sizeof(int ***));
     MVint[i] = (int ***)malloc((ni + 1) * sizeof(int ***));

  }//for(i = 0; i < ni + 1; i++)
 for(j = 0; j < nj; j++)
  for(i = 0; i < ni; i++)
   {
     YT[j][i] = alocaMatf(tjan, tjan);
     UT[j][i] = alocaMatf(tjan, tjan);
     VT[j][i] = alocaMatf(tjan, tjan);
     MYint[j][i] = alocaMatint(tjan, tjan);
     MUint[j][i] = alocaMatint(tjan, tjan);
     MVint[j][i] = alocaMatint(tjan, tjan);
  }//for(i = 0; i < ni; i++)
	//Macroblocos(YT,UT,VT,MYint,MUint,MVint,ni,nj);

 MatrizQ();//
 printf("Matrix quantizacao...\n");
 tempY = alocaMatf(tjan, tjan);
  tempU = alocaMatf(tjan, tjan);
  tempV = alocaMatf(tjan, tjan);
tempY2 = alocaMatf(tjan, tjan);
  tempU2 = alocaMatf(tjan, tjan);
  tempV2 = alocaMatf(tjan, tjan);

  
  printf("COMPRIMINDO...\n");

 for(k = 0; k<HEADERINFO.TotalFrames;k++) { //mudei para adiantar os testes
  if(!search(AVIFILE,FOURCC('0','0','d','b')))fail("EstÃ¡s a me enganar tem compressao?");   
  fread(&tam, sizeof(int), 1, AVIFILE);		//Divide pelo tamanho de 1 byte= 8 bits 
        setBfSize(tam); 
  for(i=0;i<getBfBiWidth();i++)
   for(j=0;j<getBfBiHeight();j++){
        fread(&cbuf, sizeof(char), BITMAPHEADER.biBitCount/8, AVIFILE);		//Divide pelo tamanho de 1 byte= 8 bits
    
	//coloca os valores lidos fazendo cast para int depois para float
	bt = (unsigned int)(cbuf[0]);
	gt = (unsigned int)(cbuf[1]);
	rt = (unsigned int)(cbuf[2]);
	
	
	R[i][j] = (int)rt;
	G[i][j] = (int)gt;
	B[i][j] = (int)bt;
   }
   if(k<10){sprintf(buffer,"%d",k);
   strcat(buffer,"_");
   strcat(buffer,filename);	
   strcat(buffer,"_frame.bmp");
   BmpNew(buffer);
   BmpSave(buffer, R, G, B);}
   forcc = FOURCC('0','0','d','b');
   fwrite(&forcc,sizeof(int),1,OUTFILE);
   fwrite(&tam,sizeof(int),1,OUTFILE);
   db2_compressao(argv,R,G,B,getBfBiWidth(),getBfBiWidth(),ni,nj,Y,U,V,UV,OUTY,OUTU,OUTV,OUTUV,YT,UT,VT,MYint,MUint,MVint,tempY,tempU,tempV,tempY2,tempU2,tempV2,OR,OG,OB,out,out2,aux);
  for(i=0;i<getBfBiWidth();i++)
    for(j=0;j<getBfBiHeight();j++){
       
	
        //pega as variaveis faz cast para unsigned char
	bt = (unsigned int) B[i][j];
	gt = (unsigned int) G[i][j];
	rt = (unsigned int) R[i][j];
        
        cbuf[0] = (unsigned char) bt;
        cbuf[1] = (unsigned char) gt;
        cbuf[2] = (unsigned char) rt;    

        //grava no arquivo        
	fwrite(cbuf, sizeof(char),BITMAPHEADER.biBitCount/8, OUTFILE);	//Divide pelo tamanho de 1 byte= 8 bits
	
      
   }


   
  }

  if(!search(AVIFILE,FOURCC('i','d','x','1')))fail("Oras, nÃ£o deve ser AVI?");
   b = ftell(AVIFILE);
   OFFSETOUT.IDX1 = b - sizeof(int);
   writeChunk(OUTFILE,FOURCC('i','d','x','1'),OFFSETOUT.IDX1);
  INDEX *INDEXS;
  INDEXS =  (INDEX*) malloc((HEADERINFO.TotalFrames)*sizeof(INDEX));
  for(i=0;i<HEADERINFO.TotalFrames;i++)fread(&(INDEXS[i]), sizeof(int), 4, AVIFILE);	
  for(i=0;i<HEADERINFO.TotalFrames;i++)fwrite(&(INDEXS[i]), sizeof(int), 4, OUTFILE);
  fwrite(&tam, sizeof(int), 1, OUTFILE);
 

}




/*int main(int argc, char *argv[]){
 int  buffer;
 char *infilename = argv[1];
 char *outfilename = argv[2];
 FILE *infile = fopen(infilename, "rb");
 FILE *outfile = fopen(outfilename, "wb");
 if (!infile) printf("Cannot open input file");
 printf("AQUI:%ld\n",ftell(infile));
 openAVI(infile,outfile,argv[1]);
  
 printf("AQUI:%ld\n",ftell(infile));
 fclose(infile);
 fclose(outfile);
 return 0;
}*/

int main(int argc, char** argv)
{
  int tam_macro, ecYUV, quant = -1, n_quant = -1, i, j;
   float time_ini, time_end;
  char str_t[4];

  if(argc != 6)
    impr_uso();
  else
    if((strstr(argv[1], ".avi") != NULL) || (strstr(argv[1], ".AVI") != NULL))
	if((strstr(argv[2], ".avi") != NULL) || (strstr(argv[2], ".AVI") != NULL))
	{
	  tam_macro = atoi(argv[3]);

	  if(tam_macro == 8 || tam_macro == 16 || tam_macro == 32 || tam_macro == 64 || tam_macro == 128 || tam_macro == 256)
	  {
	    tjan = tam_macro;
	    MQ = alocaMatf(tjan, tjan);
	    ecYUV = atoi(argv[4]);

	    if(ecYUV == 444 || ecYUV == 422 || ecYUV == 440)
	      if(!strncmp(argv[5], "nq=", 3) || !strncmp(argv[5], "q=", 2))
	      {

		if(!strncmp(argv[5], "nq=", 3))
		{
		  i = 3;
		  j = 0;
		  while(&argv[5][i] != NULL && j < 3)
		    str_t[j++] = argv[5][i++];

		  str_t[j] = '\0';
		  n_quant = atoi(str_t);

		}//if(!strncmp(argv[5], "nq=", 3))
		else
        	{
		  i = 2;
		  j = 0;
		  while(&argv[5][i] != NULL && j < 3)
		    str_t[j++] = argv[5][i++];

		  str_t[j] = '\0';
		  quant = atoi(str_t);

		}//if(!strncmp(argv[5], "nq=", 3))

		if((n_quant >= 0 && n_quant <= 100) || (quant >= 0 && quant <= 100))
		{
		   if(n_quant != -1)
		   {
		     fatorf = 496 * n_quant / 100 + 16;
		     printf("\nfatorf = %f\n", fatorf);

		   }//if(n_quant != -1)

		   if(quant != -1)
		   {
		     fator1q = fator2q = 7168 * quant / 100 + 1024;
			 //fator1q = fator2q = 982 * quant / 100 + 32;
		     printf("\nfatorq = %f\n", fator1q);

		   }//if(n_quant != -1)
		   time_ini = dbtime();
                 
                  FILE *infile = fopen(argv[1], "rb");
                  FILE *outfile = fopen(argv[2], "wb"); 
                  openAVI(infile,outfile,argv[1],argv);
                  fclose(infile);
 		  fclose(outfile);	
                  time_end = dbtime();
                  printf("Tempo para %s :%lf(s)\n", argv[1], (time_end - time_ini) / 1000.0f);
		  // db2_compressao(&argv[0]);

		}//if((n_quant >= 0 && n_quant <= 100) || (quant >= 0 && quant <= 100))

		else
		  impr_perfis();

	      }//if(!strncmp(argv[5], "nq=", 3) || !strncmp(argv[5], "q=", 2)

	    else
		  impr_perfis();

	    else
	      impr_amostragem();

	  }//if(tam_macro == 8 || ...
	  else
	    impr_tam_bloco();

	}//if((strstr(argv[2], ".bmp") != NULL) || (strstr(argv[2], ".BMP") != NULL))
	else
	  impr_arq_dest();

    else
      impr_arq_origem();

}//void main(int argc, char** argv)
