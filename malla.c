/*a) Realiza un programa llamado malla.c que produzca el siguiente árbol de procesos. El programa
recibirá dos argumentos ‘x’ e ‘y’ que representan el número de filas y columnas. Para
comprobar la estructura de procesos que realmente estamos creando debemos emplear la
llamada al sistema “pstree –c” (2 puntos)*/


#include<unistd.h>
#include<stdlib.h>


int obtenerColumnas( char * argumento){
    return atoi(argumento);
}


int obtenerFilas( char * argumento){
    return atoi(argumento);
}

void crearFilas(int x){

    int i;
    pid_t pid;

    for(i=1; i<x; i++){//para que el hijo cree las filas
    pid=fork();

        if(pid!=0){//si no es 0, es el padre, tiene que salir para que no cree mas hijos
            break;
        }
    }
}

void crearColumnas(int x, int y){

    int j;
    pid_t pid;
    
        for (j=0; j<y; j++){//para que el padre cree las columnas de hijos
           pid=fork();//crea un hijo

           if(pid==0){ //si es igual a 0 es un hijo
               crearFilas(x);
               
               sleep(60); //para ver pstree -c
               exit(0); //se pone el exit() para que el hijo no construya sus propias columnas, ya que es una copia del padre
           }
   }
}


int main( int argc, char *argv[]){

    int x, y; //para los argumentos, filas/columnas

    x=obtenerFilas(argv[1]);
    y=obtenerColumnas(argv[2]);

    crearColumnas(x, y);

    sleep(60);
    exit(0);


}
