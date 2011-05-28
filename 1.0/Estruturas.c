#include <stdio.h>
#include <stdlib.h>
#include "Estruturas.h"


/*Aloca vetor dinamicamente*/
unsigned char *alocaVet(int tam) {
	unsigned char *tmp;
	tmp = (unsigned char *)malloc((tam+1) * sizeof(char));
        return tmp;
}

//zera matriz A nxm para float
void iniciaf(float **A, int n, int m)
{
    int i, j;

    for (i = 0; i < n; i++) 
    {
	for (j = 0; j < m; j++) 
	{
		A[i][j] = 0; 

	}
   }
}

//zera matriz A nxm para int
void inicializai(int **A, int n, int m)
{
    int i, j;

    for (i = 0; i < n; i++) 
    {
	for (j = 0; j < m; j++) 
	{
		A[i][j] = 0; 

	}
   }

}

//aloca matriz nxm para float
float **alocaMf(int n, int m)
{
	int i;
	float **tmp;

	tmp = (float **)malloc((n + 1)* sizeof(float *));

	for (i = 0; i < n + 1; i++)
		tmp[i] = (float *)malloc((m + 1) * sizeof(float));

	iniciaf(tmp, n, m);

	return tmp;
}

//aloca matriz nxm para int
int **alocaMati(int n, int m)
{
	int i, **tmp;

	tmp = (int **)malloc((n + 1)* sizeof(int *));
	//tmp = (int **)calloc((n + 1), sizeof(int *));

	for (i = 0; i < n + 1; i++)
		tmp[i] = (int *)malloc((m + 1) * sizeof(int));

	inicializai(tmp, n, m);

	return tmp;
}

//zera matriz A nxm para float
void inicializaf(float **A, int n, int m)
{
    int i, j;

    for (i = 0; i < n; i++) 
    {
	for (j = 0; j < m; j++) 
	{
		A[i][j] = 0; 

	}
   }

}

//aloca matriz nxm para float
float **alocaMatf(int n, int m)
{
	int i;
	float **tmp;

	tmp = (float **)malloc((n + 1)* sizeof(float *));

	for (i = 0; i < n + 1; i++)
		tmp[i] = (float *)malloc((m + 1) * sizeof(float));

	inicializaf(tmp, n, m);

	return tmp;
}

//aloca matriz nxm para inteiro
int **alocaMatint(int n, int m)
{
	int i;
	int **tmp;

	tmp = (int **)malloc((n + 1)* sizeof(int *));

	for (i = 0; i < n + 1; i++)
		tmp[i] = (int *)malloc((m + 1) * sizeof(int));


	return tmp;
}


/*Aloca vetor dinamicamente*/
unsigned char *alocaVeti(int tam)
{
	unsigned char *tmp;

	tmp = (char *)malloc((tam+1) * sizeof(char));
    return tmp;
}

float *alocaVetf(int tam)
{
	float *tmp;
	
	tmp = (float *)malloc((tam+1) * sizeof(float));
    return tmp;
}




