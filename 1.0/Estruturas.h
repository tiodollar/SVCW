
#define Type unsigned float

unsigned char *alocaVet(int tam);


unsigned char *alocaVeti(int tam);
float *alocaVetf(int tam);


//Type **alocaMat(int n,int m);

//aloca matriz nxm para float
float **alocaMf(int n, int m);

//aloca matriz nxm para int
int **alocaMati(int n, int m);

//zera matriz A nxm para float
void iniciaf(float **A, int n, int m);

//zera matriz A nxm para int
void inicializai(int **A, int n, int m);

//zera matriz A nxm para float
void inicializaf(float **A, int n, int m);

//aloca matriz nxm para float
float **alocaMatf(int n, int m);

//aloca matriz nxm para inteiro
int **alocaMatint(int n, int m);

