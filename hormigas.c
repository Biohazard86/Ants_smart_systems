
// DAVID BARRIOS
// davidbarrios@usal.es
// Algoritmo de hormigas
//-----------------------

//           COMPILE>>>
//           gcc -o hormigas hormigas.c -lm

//          RUN>>>
//          ./hormigas att48.tsp 




// ALGORITMO:
/*

		INICIO
		    t=0
		    hacer Tij(t)=T0(i,j)
		MIENTRAS
		    Elegir la ciudad de partida de cada hormiga (aleatoria)
		    Determinar las sucesivas paradas de cada hormiga, según la regla probabilistica
		    Aplicar la actualización local de la feromona.
		    Aplicar la actualizacion global de la feromona
		    t=t+1
		FIN-MIENTRAS
		FIN

*/


/*

    PEQUENIO RESUMEN    --------------------------------------------------------------------------------------------------------------



    Si una hormiga detecta feromona, es muy probable que decida moverse por ese camino. En caso de no encontrarla, se movera de forma aleatoria.
    De esta forma emerge un comportamiento llamado AUTOCATALITICO: cuanto mayor sea el numero de hormigas que sigan cierto trayecto, mas atractivo se hara para las hormigas.

    Los caminos seleccionados por menos hormigas, cada vez iran perdiendo feromonas y se haran menos deseables.

    Se considera un grafo TSP con n ciudades conectados mediante aristas, las cuales tienen una distancia (de la cual obtenemos un coste.)

    Tenemos una matriz de distancias de FIL X COL, donde las filas y las columnas son el mismo numero, y representa ciudad de inicio y ciudad de destino. La diagonal principal siempre debe ser igual a 0, ya que de la ciudad 3 a la 3 la distancia es 0.
    M es el conjunto de hormigas.

    la matriz de feromonas es igual que la matriz de distancias en cuanto a dimensiones. Inicialmente se ponen todos los caminos con el mismo valor de feromona.

    Para saber a que ciudades hemos ido creamos la matriz de ciudades visitadas, la cual tiene el numero de hormigas como filas y el numero de ciudades como columnas.

    En ella si la hemos visitado ponemos la ciudad, y si es ella misma ponemos un numero menor a 0

    De esa forma podemos ver facilmente cual hemos visitado.

    Despues de cada "camino" de todas las hormigas actualizamos la matriz de feromonas, ya que si una hormiga ha elegido un buen camino la feromona dejada sera mayor.



*/


// leemos un archivo de texto y eliminamos lo que no nos interesa
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
//------
#define TAM_LINEA 64 //Tamaño maximo de la linea, por defecto le he axignado 64 porque se espera que ningun arhivo tenga ninguna linea de mas de 64 caracteres
#define M 10 //Numero de hormigas 
#define T 0.1 // Numero inicial de la feromona.
#define T_MAX 100    // Numero maximo de iteraciones que realizara el algoritmo.
#define NULO -1 // Definimos NULO para usarlo en un vector, ya que solo tenemos en cuenta numeros  positivos. 
#define INFINITO 9999   // Definimos un numero muy alto para asi no volver a ir nunca a esa ciudad.
#define P 0.95   //Mayor que cero 0 e igual o menor que 1
#define A 2     // ALPHA
#define B 4     //
#define maximo_numero_datos_CTE  100 



// PROTOTIPOS DE LAS FUNCIONES ---------------------------------------------------
int contar_espacios(char linea[], int *max_num_datos);
double calculo_dinstancia_euclidia(int x1, int y1, int x2, int y2);
void liberar_memoria_matriz_double(double matriz_d, int maximo_numero_datos);
void liberar_memoria_matriz_int(int matriz_i, int maximo_numero_datos);
int genera_aleatorio(int maximo_numero_datos);
int siguiente_iteraccion(double **matriz_distancia_euclidea,int **matriz_ciudades_visitadas,double **matriz_feromonas, int iteraccion, int maximo_numero_datos, double *coste_acumulado);
int mejor_siguiente_ciudad(int ciudad_actual, double **matriz_distancia_euclide, double **matriz_feromonas, int maximo_numero_datos, int **matriz_ciudades_visitadas, double *vector_coste_acumulado, int numero_hormiga, int iteraccion);
void actualizacion_feromona(int **matriz_ciudades_visitadas, double **matriz_feromonas, int iteraccion, int maximo_numero_datos, double *vector_costes_acumulados);
void actualizacion_costes_acumulados(double *vector_costes_acumulados, double **matriz_distancia_euclidea, int **matriz_ciudades_visitadas, int maximo_numero_datos, int iteraccion);

// MAIN -------------------------------------------------------------


int main (int argc, char *argv[])
{   
    FILE *fich; // Puntero que apunta al archivo

    char linea [TAM_LINEA] ;  //Cada linea leida se guadara en esta cadena de TAM_LINEA de longitud
    int espacios_linea, contador_lineas=0, longitud_linea_leida, maximo_numero_datos=0,i, vector_temporal1[60], vector_temporal2[60];
    int num1, num2, indice, f, j,auxiliar_coordenada, t,q;
    int **matriz_datos;  // Matriz de memoria dinamica para guardar los datos en funcion de su tamaño.
    double **matriz_distancia_euclidea;   //Matriz de momoria dinamica donde guardamos las distancias euclideas.
    double **matriz_feromonas;     // Matriz de memoria dinamica en la cual guardamos las feromonas de todas las hormigas.
    int **matriz_ciudades_visitadas; // Cada vector que forma la matriz corresponde a una hormiga. En el ejemplo es de FIL X COL 10X48
    double *vector_costes_acumulados,  auxiliar, mejor_coste_global, coste_actual;
    int auxiliar_3, e;
    double coste_parcial=99999999, coste_total=99999999, mejor_coste_parcial; //, mejor_coste_global;     // los inicializamos a un valor muy alto para luego poder compararlos.


    time_t tiempo; 

    //Semilla para aleatorios:
    srand((unsigned) time(&tiempo)); //  srand (time(NULL)); 
    
   



    // Estructura de control para identificar que el usuario ha introducido al menos un dato en la llamada al programa.
    if(argc < 2){
        printf("\n No se han introducido parametros suficientes. \n Recuerde que tiene que introducir el nombre del archivo a leer.\n");
        exit(1);
    }


    
    //-------------------------
     // Pasamos el nombre del archivo a leer mediante parametro al llamar al programa
    fich=fopen(argv[1],"r");    // abrimos el archivo en modo lectura.

    if(fich==NULL){
       printf("\n No se ha podido abrir el archivo!\n");    // En caso de que el puntero apunte a NULL significa que el archivo no se ha podido abrir
       exit(1); //saldriamos del programa.
   }
    
    //Leemos linea a linea y la guardamos en dos vectores 
   if( fgets (linea, 64, fich)!=NULL ) {

        
        while(fgets(linea, 64, (FILE*) fich)){      // Mientras haya lineas, seguimos leyendolas
            espacios_linea = contar_espacios(linea, &maximo_numero_datos);  // llamamos a la funcion que cuenta los espacios de cada linea 
            longitud_linea_leida=strlen(linea); //Vemos la lingutid de la linea leida.
            contador_lineas++;  // Almacenamos el numero total de lineas leidas (Aumentamos el contador para saberlo al final)
        }

        printf("\nEl numero util de datos segun la cabecera del archivo es de:  %d\n", maximo_numero_datos);    // Imprimimos el numero que obtenemos de la cabecera

        printf("\nSe han leido un total de %d lineas \n", contador_lineas); // Imprimimos el contador anterior con el numero total de lineas leidas (Este dato no es util, es solo por curiosidad)

        if(maximo_numero_datos>contador_lineas){
            //Si se produce este caso, se trata de un error.
            printf("\nError al leer el archivo.\n");
            return 1;

        }
        
    }

 fclose ( fich );        // Cerramos el fichero ya que de momento hemos terminado con el


 //reservamos memoria para los datos:

	//  OJO: estos arrays no pueden tener como tamaño una VARIABLE
    //  QUITO int mejor_ruta_parcial[maximo_numero_datos], mejor_ruta_global[maximo_numero_datos]; 
	// sustituye a lo anterior. Deberías comprobar que: maximo_numero_datos <= maximo_numero_datos_CTE
    int mejor_ruta_parcial[maximo_numero_datos_CTE], mejor_ruta_global[maximo_numero_datos_CTE]; 
	// o tendrias que usar matrices dinámicas

    if(maximo_numero_datos <= maximo_numero_datos_CTE){
        
    }else{
        printf("\nERROR. TAMANIO DE DATOS.\n");
        exit(3);
    }

    matriz_datos = (int **)malloc (maximo_numero_datos *sizeof(int *)); // dependiendo del valor de la variable maximo_numero_datos
    
    if(matriz_datos == NULL){       // Sin la matriz de datos es nula, entonces algo ha fallado mostramos un mensaje por pantalla 
        printf("\n\n  NO SE HA PODIDO RESERVAR MEMORIA! ");
        return 0;

    }
    else{
        printf("\nSe ha reservado memoria correctamente. (matriz_datos)\n");   // Si sale todo bien entonces mostramos este mensaje por pantalla
    }

    //  en cada iteración también deberías comprobar que la llamada a malloc no retorna NULL
    for (i=0;i<maximo_numero_datos;i++)
    if(NULL== (matriz_datos[i] = (int *) malloc (2*sizeof(int))) ){
        printf("\n Se ha producido un error al reservar memoria.");
    }   // siempre será una matriz deint
    


    //--------------------------------------------------------------------------------------------------------------------
    //Volvemos a abrir el archivo para guardar los datos.
    
    fich=fopen(argv[1],"r");        //Abrimos el archivo en modo lectura (r)
    i=0;  //Ponemos i a 0

    if(fich==NULL){
       printf("\n No se ha podido abrir el archivo!\n");    // En caso de que el puntero apunte a NULL significa que el archivo no se ha podido abrir
       exit(1); //saldriamos del programa.
   }

    //void rewind(FILE *fich);        // Situamos el puntero del archivo al principio de este. Esto no es mas que una precaucion por si acaso el puntero ha quedado apuntando al final del archo (EOF)

    
    while(fgets(linea, 64, fich)){  //mientras sea posible leemos la linea 

        espacios_linea = contar_espacios(linea, &maximo_numero_datos); //Pasamos la linea leida a la funcion de contar espacios. 
        fscanf(fich, "%d %d %d", &indice, &num1, &num2);    //leemos 3 enteros de la linea. Sea o no datos utiles

        if(espacios_linea!=0){
            if(i<maximo_numero_datos){      //este if controla no salirnos de la matriz.
                matriz_datos[i][0]=num1;    //guardamos el numero 1 y el 2
                matriz_datos[i][1]=num2;
            }
            
            i++;    // Incrementamos el indice que usamos para escribir en la matriz
        }
    
    }
     // Cerramos el archivo ya que tenemos los datos guardados en la matriz.

    fclose ( fich );   

    // Imprimimos la matriz dinamica para comprobar:
    printf("\n ----------------------");
    printf("\n MATRIZ ALMACENADA EN LA MEMORIA DINAMICA");
    i=0;
    printf("\n INDICE |  X  |   Y ");
    for(i=0;i<maximo_numero_datos;i++){
        
        printf("\n   %d", i);
        printf("    %d \t", matriz_datos[i][0]);
        printf("%d \t", matriz_datos[i][1]);
    }
    printf("\n ----------------------\n");
 // Ya contamos con la MATRIZ DE DATOS almacenada en memoria.
 // Pasamos a calcular la distancia euclidea. Creamos una matriz de maximo_numero_datos X maximo_numero_datos
 
    matriz_distancia_euclidea = (double **)malloc (maximo_numero_datos *sizeof(double *)); // dependiendo del valor de la variable maximo_numero_datos

    for (i=0;i<maximo_numero_datos;i++)
    matriz_distancia_euclidea[i] = (double *) malloc (maximo_numero_datos *sizeof(double));   // siempre será una matriz de maximo_numero_datos X 2 dimensiones

	// OJO: la matriz sólo se ha creado si tanto matriz_distancia_euclidea como cada matriz_distancia_euclidea[i] son disintos de NULL
	// este comentario se aplica a todos los arrays dinámicos de dos dimensiones que creas
    // comprobamos que se haya reservado la memoria.
    if(matriz_distancia_euclidea == NULL){       // Sin la matriz de datos es nula, entonces algo ha fallado mostramos un mensaje por pantalla 
        printf("\n\n  NO SE HA PODIDO RESERVAR MEMORIA! \n (matriz_distancia_euclidea) ");
        return 0;

    }
    else{
        printf("\nSe ha reservado memoria correctamente.  (matriz_distancia_euclidea)\n");   // Si sale todo bien entonces mostramos este mensaje por pantalla
    }



// Ya tenemos la matriz de distancia euclidea creada, ahora procedemos a calcular los datos.

// recorremos la matriz de distancia euclidea. Sabemos que la diagonal principal será simpre 0, ya que la distancia de una ciudad a ella misama es 0.
    for(i=0; i<maximo_numero_datos;i++){
        for(j=0;j<maximo_numero_datos;j++){
            matriz_distancia_euclidea[i][j]= calculo_dinstancia_euclidia (matriz_datos[i][0], matriz_datos[i][1], matriz_datos[j][0],matriz_datos[j][1]);
            //Vamos recorremos la matriz de la distancia euclidea. A la vez recorremos la matriz de datos siendo i la ciudad de inicia y j la de destino.
        }


    }

	/*
    printf("\n ----------------------");
    printf("\n MATRIZ DE DISTANCIAS");
    i=0;
    printf("\n ");
    for(i=0;i<maximo_numero_datos;i++){
        printf("\n|||||||||||||||||||||||||||||||||||||||||");
        for(j=0;j<maximo_numero_datos;j++){

            printf("\n Ciudad %d - ciudad %d  %f",i, j, matriz_distancia_euclidea[i][j]);
        }
    }
    printf("\n ----------------------\n");
*/



// Reservamos memoria para la matriz de feromonas de las hormigas.

    matriz_feromonas = (double **)malloc (maximo_numero_datos *sizeof(double *)); // dependiendo del valor de la variable maximo_numero_datos

    for (i=0;i<maximo_numero_datos;i++)
    matriz_feromonas[i] = (double *) malloc (maximo_numero_datos *sizeof(double));   // siempre será una matriz de maximo_numero_datos x maximo_numero_datos

    // comprobamos que se haya reservado la memoria.
    if(matriz_feromonas == NULL){       // Sin la matriz de datos es nula, entonces algo ha fallado mostramos un mensaje por pantalla 
        printf("\n\n  NO SE HA PODIDO RESERVAR MEMORIA! \n (matriz_feromonas) ");
        return 0;

    }
    else{
        printf("\nSe ha reservado memoria correctamente. (matriz_feromonas) \n");   // Si sale todo bien entonces mostramos este mensaje por pantalla
    }


    for(i=0;i<maximo_numero_datos;i++){
        for(j=0;j<maximo_numero_datos;j++){
            matriz_feromonas[i][j]=T;                 // ponemos a T toda la matriz, ya que inicialmente no hay feromonas. 
        }
    }


    /*

    printf("\n ----------------------");
    printf("\n MATRIZ DE FEROMONAS");
    
    printf("\n ");
    for(i=0;i<maximo_numero_datos;i++){
        printf("\n ");
        for(j=0;j<maximo_numero_datos;j++){

            printf("\nFIL: %d COL: %d - %f ",i,j, matriz_feromonas[i][j]);
        }
    }
    printf("\n ----------------------\n");
*/


// Necesitamos un vector por cada hormiga para guardar las ciudad las que visita.

    matriz_ciudades_visitadas = (int **)malloc (M *sizeof(int *)); // dependiendo del valor de la variable maximo_numero_datos

	 /*  ERROR: en matriz_ciudades_visitadas tienes M punteros para apuntar a M filas de enteros, pero 
	 tu bucle va hasta maximo_numero_datos*/
    // QUITO  for (i=0;i<maximo_numero_datos;i++)
	 for (i=0;i<M;i++) //  añado
      matriz_ciudades_visitadas[i] = (int *) malloc (maximo_numero_datos *sizeof(int));   // siempre será una matriz de maximo_numero_datos x maximo_numero_datos

//Comprobamos que se ha reservado memoria correctamente
    if(matriz_ciudades_visitadas == NULL){       // Sin la matriz de datos es nula, entonces algo ha fallado mostramos un mensaje por pantalla 
        printf("\n\n  NO SE HA PODIDO RESERVAR MEMORIA! \n (matriz_feromonas) ");
        return 0;

    }
    else{
        printf("\nSe ha reservado memoria correctamente. (Matriz ciudades visitadas)\n");   // Si sale todo bien entonces mostramos este mensaje por pantalla
    }

  
    for(i=0;i<M;i++){
        for(j=0;j<maximo_numero_datos;j++){
            if(j==0){
                matriz_ciudades_visitadas[i][j]=genera_aleatorio(maximo_numero_datos);
            }
            else{
                matriz_ciudades_visitadas[i][j]=NULO;   // A excepcion del primer elemento el resto deben ser NULOS ( menores a 0)
            }
        }
    }
/*
    printf("\n");
    printf("--------------------------------------------------------------------------------------");
    printf("\n Matriz de ciudades visitadas ");
    printf("\n");
    for(i=0;i<M;i++){
        printf("\n");
        printf("\n");
        for(j=0;j<maximo_numero_datos;j++){
            printf("  %d", matriz_ciudades_visitadas[i][j]);
        }
    }

    printf("\n");
*/
// ------------------------------------------------------------------------------------------------------------------------------------------

//Reservvamos memoria para el vector de costes acumulados. Tendra una dimension de hormigas (M) 


vector_costes_acumulados = (double *)malloc (M *sizeof(double *)); // dependiendo del valor de la variable maximo_numero_datos

// Comprobamos que se haya reservado memoria correctamente.
if( vector_costes_acumulados == NULL){
    printf("\n   NO SE HA PODIDO RESERVAR MEMORIA! \n (vector_costes_acumulados)  \n");
}










//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
    auxiliar=999999;
    mejor_coste_parcial=8888888;  // para que el coste siempre sea menor.

    t=0;
    do{ 
        f=0;
        do{ // Hago las Maximo_numero_datos iteracciones para completar un camino.

            siguiente_iteraccion(matriz_distancia_euclidea,matriz_ciudades_visitadas,matriz_feromonas,f, maximo_numero_datos, vector_costes_acumulados);
            actualizacion_costes_acumulados(vector_costes_acumulados, matriz_distancia_euclidea, matriz_ciudades_visitadas, maximo_numero_datos, f);
          
            f++; // Esta es la iteraccion del algoritmo. Es decir,con esto podemos saber el numero de ciudades que nos faltan o hemos recorrido ya 
        
        }while(f< (maximo_numero_datos-1) ); //  (f<48);

        for(i=0;i<M;i++){
            if(vector_costes_acumulados[i] < mejor_coste_parcial){

                mejor_coste_parcial=vector_costes_acumulados[i];        // Guardamos el coste del camino para tenerlo de referencia y saber si hemos encontrado otro camino mejor.
                printf("\n NUEVO MEJOR COSTE ENCONTRADO    --->  %f\n En la iteraccion %d / %d \n", mejor_coste_parcial, t, T_MAX);        // mostramos un mensaje de que hemos encontrado un comino con menos coste

                //Recorremos el vector y lo copiamos en otro.
                printf("RUTA: \n");
                for(j=0;j<maximo_numero_datos;j++){
                    mejor_ruta_parcial[j]=matriz_ciudades_visitadas[i][j];
                    printf(" %d ", mejor_ruta_parcial[j]);
                }
                printf("\n");
            }
        }
          
        

        // Ahora vamos a ver cual de esta iteraccion es mejor y ver si supera a la mejor global.

		 
        // Ya he obtenido los (en este caso) 10 caminos de cada hormiga, cada uno con un coste total.

        //Vemos cual es el mejor coste y lo guardamos, asi como su indice para compararlo con los futuros costes.

/*
        printf("\n--------------------------------------------------------------------------------------\n");

        for(i=0;i<M;i++){
            if(vector_costes_acumulados[i]<auxiliar){       // Si alguna hormiga ha hecho un coste menor a AUXILIAR
                auxiliar=vector_costes_acumulados[i];
                auxiliar_coordenada=i;
                for(j=0;j<maximo_numero_datos;j++){     //copiamos la ruta para tenerla almacenada.
                    mejor_ruta_parcial[j]=matriz_ciudades_visitadas[auxiliar_coordenada][j];
                }
            }
        }
*/
        //QUITO  printf("\n El mejor de la iteraccion %d es (coordenada-valor_coste): %d - %lf", t, auxiliar_coordenada, auxiliar);
        // queda mas claro poner 'hormiga' que 'coordenada', pues las ciudades tienen 2 coordenadas y parece que has puesto 1 sólo
        //printf("\n El mejor de la iteraccion %d es (Hormiga-valor_coste): %d - %lf", t, auxiliar_coordenada, auxiliar);

        //despues de tener un camino de cada hormiga, actualizamos la matriz de feromonas.
        actualizacion_feromona(matriz_ciudades_visitadas, matriz_feromonas, f, maximo_numero_datos, vector_costes_acumulados);
    t++;
    
    }while(t<T_MAX);

    // Ya hemos solucinoado el problema.


    printf("\n\n\n\n\n\t=========================================");
    printf("\n\t|                SOLUCION               |");
    printf("\n\t=========================================\n");
    
                printf("\nLa mejor ruta pasa por las ciudades:\n");
                for(j=0;j<maximo_numero_datos;j++){    
                    printf("  %d", mejor_ruta_parcial[j]);
                }
    


//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================





/*
    auxiliar=vector_costes_acumulados[0];
    auxiliar_coordenada=0;
    for(i=0;i<M;i++){
        if(auxiliar>vector_costes_acumulados[i]){
            auxiliar=vector_costes_acumulados[i];
            auxiliar_coordenada=i;
        }
        printf("%f,  ", vector_costes_acumulados[i]);
    }
 */ 
  
    printf("\n");
    printf("--------------------------------------------------------------------------------------");
    printf("\n");
    printf("MEJOR COSTE >>> \n");
    printf("\n\t   %f", mejor_coste_parcial);
    printf("\n\n");
    printf("--------------------------------------------------------------------------------------");

    //--------------------------------------------




    printf("\n");
    printf("\tSOLUCION COORDENADAS:\n\n");

    printf("  COORDENADA X  >>>\n\n");

    for(i=0;i<maximo_numero_datos;i++){
        //auxiliar=matriz_ciudades_visitadas[auxiliar_coordenada][i];
        printf("%d\n", matriz_datos[mejor_ruta_parcial[i]][0] );        // Imprimimos de la matriz_datos la coordenada de la posicion 
    }

    printf("\n");

    printf("\n");

    printf("  COORDENADA Y  >>>\n\n");
    for(i=0;i<maximo_numero_datos;i++){
        // auxiliar=matriz_ciudades_visitadas[auxiliar_coordenada][i];
        printf("%d\n", matriz_datos[mejor_ruta_parcial[i]][1] );         
    }

    printf("\n");
    printf("\n");


    //--------------------------------------------
    //Liberamos la memoria
    // Matriz de datos: (Enteros)
        for(i = 0; i < maximo_numero_datos; i++){
            free(matriz_datos[i]);
        }
        free(matriz_datos);

    //Matriz de distancias euclideas. (double)
        for(i = 0; i < maximo_numero_datos; i++){
            free(matriz_distancia_euclidea[i]);
        }
        free(matriz_distancia_euclidea);

    //Matriz de feromonas (double)
        for(i = 0; i < maximo_numero_datos; i++){
            free(matriz_feromonas[i]);
        }
        free(matriz_feromonas);

    // Vector costes acumulados
    free(vector_costes_acumulados);
    
    
    //-------------------------------------------

}   
// FIN MAIN.




//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||




// Con esta funcion conseguimos leer la cabecera del archivo. 



int contar_espacios(char linea[], int *max_num_datos){

//Variables para el for y para contar los espacios dentro de una linea.
    int i, contador_espacios=0,j, numero, multiplicador=1,k=30, tamanio_linea_actual,z=0, temporal=0, bandera=0;
    char numero_char[30], numero1[30]; //Podremos manejar hasta numeros de máximo 30 cifras.
    //Ponemos en todas las componentes a 0 (variable, caracter que queremos incluir, dimension del vector)
    memset(numero_char, 0, 30);
    

    //Obetenemos el tamaño de la linea que vamos a analizar
    tamanio_linea_actual=strlen(linea);
    
//recorremos toda la linea contando los espacios en blanco.
    for(i=0;i<TAM_LINEA;i++){
        // Si nos encontramos un espacio incrementamos el contador de espacios para dicha linea.
        if(linea[i]==' '){
            contador_espacios++;
        }
        // if 
        if(linea[i]==':'){
            bandera=1;  // Si encontramos ":" nos da igual el numero de espacios de esa linea ya que se trata de texto y no la procesamos.
            
        }
        if((linea[i]=='N') && (linea[i+1]=='O') && (linea[i+2]=='D')){
            contador_espacios=3;  // Para que no falle debemos leer esta linea.
        }

        //Si nos topamos con la letra D seguida de una I y una M
        if((linea[i]=='D') && (linea[i+1]=='I') && (linea[i+2]=='M')){
            k=30;
            //printf("\nSe ha detectado D I M\n"); //Detectamos la palabra DIMENSION
            //Recorremos toda la linea de nuevo pero esta vez buscamos solo numeros dentro de ella. Y la recorremos del final al principio.
            for(j=tamanio_linea_actual;j>0;j--){
                if(linea[j]=='0' || linea[j]=='1' || linea[j]=='2' || linea[j]=='3' || linea[j]=='4' || linea[j]=='5' || linea[j]=='6' || linea[j]=='7' || linea[j]=='8' || linea[j]=='9'){
                    numero_char[k]=linea[j];  
                    
                    //printf("\n\t Caracter detectado: %c", linea[j]);
                    //printf("\n\t Caracter en numero_char: %c", numero_char[k]);
                    numero1[z]=linea[j];   //Cuando encontremos un numero, lo guardamos
                    numero = atoi(numero1); //Pasamos ese numero de CHAR a INT
                    //printf("\nNUMERO %d", numero);
                    k--; 
                    numero=numero*multiplicador; //Usamos un multiplicador que va aumentando en cara interaccion para obtener decenas, centenas,...
                    multiplicador=multiplicador*10;
                    if(temporal!=0){    // en caso de ser la primera iteraccion no ejecutamos esto.
                        numero=temporal+numero; // para sumar por ejemplo 40 + 8
                    }
                    //printf("\nNUMERO %d", numero);
                    temporal=numero;    //Guardamos el numero para la siguiente iteraccion si es necesario

                }

            } 
            
            *max_num_datos =numero; // guardamos el numero obtenido en el puntero para usarlo en el main
        

        }
    }
    if(bandera==0){
        return contador_espacios;
    }
    else{
        return 0;
    }

}    //FIN DE LA FUNCION.







// -------------------------------------------------------------------------------------------
// CALCULO DE LA DISTANCIA EUCLIDEA ENTRE DOS CIUDADES. Se le pasa las coordeandas x e y de la ciudad 1 y de la ciudad 2

double calculo_dinstancia_euclidia(int x1, int y1, int x2, int y2){
    // Se calcula haciendo la raiz cuadrada de (x2-x1)^2 + (y2-y1)^2

    double resultado;  // la variable que retornaremos con el resultado.

    resultado = sqrt( pow((x2-x1),2) + pow((y2-y1),2) );

    return resultado;       // retornamos el resultado
}

// FIN DE LA FUNCION.










//---------------------------------------------------------------------------------------------
//Funcion para generar numeros aleatorios entre 0 y maximo_numero_datos

int genera_aleatorio(int maximo_numero_datos){
    int num_random;

	// QUITO num_random=rand () % (maximo_numero_datos-0+1) + 0;   // Este está entre M y N  // rand () % (N-M+1) + M;
	 num_random=rand () % maximo_numero_datos; 
	
    return num_random;
}









//-------------------------------------------------------------------------------------------

// En esta funcion calculamos la siguiente ciudad  a la que ir PARA TODAS LAS HORMIGAS 
int siguiente_iteraccion(double **matriz_distancia_euclidea,int **matriz_ciudades_visitadas,
                         double **matriz_feromonas, int iteraccion, int maximo_numero_datos, double *coste_acumulado){

    int i;
    //printf("\n\n\n\n ////////////////");
    // a partir de iteraccion sabemos a que posicion del vector de la matriz de ciudades visitadas tenemos que acceder
    for(i=0;i<M;i++){
    // Con este for recorremos todas las hormigas 

        //guardamos en el hueco de la siguiente ciudad el calculo de la siguiente ciudad teniendo en cuenta en cual estamos
        
        //printf("\n \nHORMIGA %d", i);
        matriz_ciudades_visitadas[i][iteraccion+1] = mejor_siguiente_ciudad(matriz_ciudades_visitadas[i][iteraccion],
                                                                            matriz_distancia_euclidea,matriz_feromonas ,
                                                                            maximo_numero_datos, matriz_ciudades_visitadas, 
                                                                            coste_acumulado, i, iteraccion); 
        //printf("\n \t ACTUAL  %d | SIGUIENTE %d",matriz_ciudades_visitadas[i][iteraccion] ,matriz_ciudades_visitadas[i][iteraccion+1]);
        //printf("\n ++++");
        
    } 

    

}








//--------------------------------------------------------------------------------------------


// PARA CADA HORMIGA EN INDIVIDUAL.
// En esta funcion calculamos la siguiente ciudad entre todas las posibles dada una ciudad inicial (ciudad_actual)

int mejor_siguiente_ciudad(int ciudad_actual, double **matriz_distancia_euclide, double **matriz_feromonas, 
                           int maximo_numero_datos, int **matriz_ciudades_visitadas, double *vector_coste_acumulado, 
                           int numero_hormiga, int iteraccion){
    // Vamos a comprobar una por una todas las posibles ciudades de destino (No incluiremos en la busqueda la ciudad actual, ni ninguna de las visitadas anteriormente.)
    // Como la ciudad de destino ya la conocemos (pasada por parametro) solo necesitamos recorrer un vector de maximo_numero_datos 

    int i, j, k, l, ciudad_destino, mejor_final, auxiliar;
    double vector_probabilidad_ciudades[maximo_numero_datos_CTE], // ojo, tamaño variable -> vector_probabilidad_ciudades[maximo_numero_datos], 
	       mejor_parcial;
    double numerador, denominador;

    //Declaramos un vector de longitud maximo_numero_datos para guardar todas las distancias, incluida la de la propia ciudad consigo misma, que sera 0 y luego deberemos tenerlo en cuenta
	
    for (i=0;i<maximo_numero_datos;i++){
        if(i == ciudad_actual){     // Si la i es igual a la ciudad actual quiere decir que vamos a calcular la distancia desde la ciudad actual a la ciudad actual.
            vector_probabilidad_ciudades[i]= NULO; // La distancia sera 0 o NULA
            
        }
        else{       // En el caso contrario procesamos la distancia

            numerador = pow(matriz_feromonas[ciudad_actual][i], A) * pow(1/matriz_distancia_euclide[ciudad_actual][i], B);

			  /*OJO: ¡¡el denominador no es este!!. Como hablamos en clase, no necesitamos realmente calcular una probabilidad
			   para que el programa funcione. Sería necesario para dar una descripción matemática correcta del algoritmo, eso si.
				Como el denominador es el mismo para todos los posibles destinos, resulta más sencillo calcular sólo el numerador
				y comparar dichos  numeradores */
        //    denominador = matriz_feromonas[ciudad_actual][i] * (1/matriz_distancia_euclide[ciudad_actual][i]);

            vector_probabilidad_ciudades[i] = numerador; // vector_probabilidad_ciudades[i] = (numerador/denominador);
            //vector_probabilidad_ciudades[i]= matriz_feromonas[ciudad_actual][i]*(1/matriz_distancia_euclide[ciudad_actual][i]); 
            // la distancia a cada ciudad sera la feromona entre el camino de esas ciudades (Guardado en la matriz de feromonas) y el inverso de la distancia 


        }
        // recorremos el vector de nuevo para poner a NULO las ciudades ya visitadas.
        // Desde 0 hasta el ultimo numero que hayamos llegado.

    }


    for (i=0;i<iteraccion;i++){
        for(k=0;k<iteraccion;k++){
            auxiliar=matriz_ciudades_visitadas[numero_hormiga][i];
            vector_probabilidad_ciudades[auxiliar]= -2;                 // -2 para diferenciar de -1 
        }
    }
    


    
/*
    printf("\n VECTOR DE PROBABILIDAD DE CIUDADES\n\t\t");
    for(i=0;i<maximo_numero_datos;i++){
        printf("  %f", vector_probabilidad_ciudades[i]);
    }
    printf("\n");
*/
    // Ahora vemos cual de ellas tiene un coste menor y hay una mayor probabilidad de que la hormiga vaya a dicha ciudad (nos fijaremos en el indice del vector para obtener dicha ciudad destino)
    // Recorremos el vector buscando la mejor 

    mejor_parcial=0; //Introducimos un numero muy alto para que todos los numeros del vector sean mejor que ese.

    for(i=0;i<maximo_numero_datos;i++){
        if( ( vector_probabilidad_ciudades[i] > mejor_parcial) && (vector_probabilidad_ciudades[i] > 0 ) ){    // Si el numero del vector es mejor que el ultimo mejor y tambien es mayor de 0 se convierte en el mejor nuevo resultado
            mejor_parcial=vector_probabilidad_ciudades[i];  // Guardamos el dato del coste
            mejor_final= i;     // Guardamos que ciudad es la mejor a la que ir.
        }
    }
    // Una vez acabado ya tenemos el mejor_parcial con el coste y mejor_final con la ciudad a la que ir

    // Podemos guardar el coste para tenerlo presente y luego elegir la hormiga con menor coste. Pasamos a esta funcion la variable coste_acumulado.

    //vector_coste_acumulado[numero_hormiga]=1/ ( matriz_distancia_euclide[ciudad_actual][mejor_final] );
    // Si ahora dividimos el vector de coste entre la feromona del camino, obtenemos el coste, ya que en el hay almacenada la probabilidad de que una hormiga vaya por ahi.
    //vector_coste_acumulado[numero_hormiga] = vector_coste_acumulado[numero_hormiga] / matriz_feromonas[ciudad_actual][mejor_final]; // Sacamos la feromona del camino a partir de la ciudad actual y la ciudad a la que hemos decidido ir.

    return mejor_final;
}
// FIN FUNCION






//--------------------------------------------------------------------------------------------

// Funcion para calcular la nueva matriz de feromonas en cada iteraccion.

void actualizacion_feromona(int **matriz_ciudades_visitadas, double **matriz_feromonas, int iteraccion, 
                            int maximo_numero_datos, double *vector_costes_acumulados){

    int i, j ,k, ciudad_inicio, ciudad_destino;


    // Actualizamos la evaporacion de la feromona.

    // Recorremos la matriz de feromonas
    for(i=0;i<maximo_numero_datos;i++){
        for(j=0;j<maximo_numero_datos;j++){
            matriz_feromonas[i][j]= (1-P)*matriz_feromonas[i][j]; // Actualizamos la evaporacion de la feromona.

        } 
    }

    // Ahora aplicamos un incremento por cada camino que ha elegido cada hormiga

    //recorremos la matriz de hormigas
    for(i=0;i<M;i++){
        for(j=0;j<(maximo_numero_datos);j++){ 
            if(j==(maximo_numero_datos-1)){
                // La ultima ciudad con la primera.
                ciudad_inicio=matriz_ciudades_visitadas[i][(maximo_numero_datos-1)];
                ciudad_destino=matriz_ciudades_visitadas[i][0];

                // Como es una matriz simetrica hay que actualizar los dos "lados" de la diagonal principal
                matriz_feromonas[ciudad_inicio][ciudad_destino]= matriz_feromonas[ciudad_inicio][ciudad_destino] + ( 1/vector_costes_acumulados[i]) ;
                matriz_feromonas[ciudad_destino][ciudad_inicio]= matriz_feromonas[ciudad_destino][ciudad_inicio] + (1/vector_costes_acumulados[i]);

            }
            else{
                ciudad_inicio=matriz_ciudades_visitadas[i][j];
                ciudad_destino=matriz_ciudades_visitadas[i][j+1];

                matriz_feromonas[ciudad_inicio][ciudad_destino]= matriz_feromonas[ciudad_inicio][ciudad_destino] +( 1/vector_costes_acumulados[i]) ;
                matriz_feromonas[ciudad_destino][ciudad_inicio]= matriz_feromonas[ciudad_destino][ciudad_inicio] + (1/vector_costes_acumulados[i]);

            }
        }
    }

    //Imprimimos la matriz de feromonas "nueva"/actualizada
   // printf("\n MATRIZ DE FEROMONAS ACTUALIZADA.\n\n");

/*
    for(i=0;i<maximo_numero_datos;i++){
        printf("\n");
        for(j=0;j<(maximo_numero_datos);j++){ 
            printf("  %f", matriz_feromonas[i][j]);
        }
    }
*/



}

// FIN FUNCION
//--------------------------------------------------------------------------------------------

//      actualizacion_costes_acumulados(vector_costes_acumulados, matriz_distancia_euclidea, matriz_ciudades_visitadas, maximo_numero_datos, f);
void actualizacion_costes_acumulados(double *vector_costes_acumulados, double **matriz_distancia_euclidea, 
                                     int **matriz_ciudades_visitadas, int maximo_numero_datos, int iteraccion){

    int i, j, k;
    //recorremos todas las hormigas
    for(i=0;i<M;i++){
        if(iteraccion==0){
            // en la primera iteraccion el coste acumulado es 0
            vector_costes_acumulados[i]=0;
        }
        else{       // Si no es la primera iteraccion debemos ver si el la ultima. para hacer la distancia con la primera ciudad.
            if(iteraccion==maximo_numero_datos){

                vector_costes_acumulados[i]= vector_costes_acumulados[i] + (  ( matriz_distancia_euclidea[matriz_ciudades_visitadas[i][0]][matriz_ciudades_visitadas[i][iteraccion]] ) );
            }
            else{
	
                vector_costes_acumulados[i]= vector_costes_acumulados[i] + ( ( matriz_distancia_euclidea[matriz_ciudades_visitadas[i][iteraccion-1]][matriz_ciudades_visitadas[i][iteraccion]] ) );
            }
        }
    
    
    }// FIN DEL RECORRIDO DE TODAS LAS HORMIGAS.


}






// FIN PROGRAMA
