/*Realizar un programa llamado hacha.c que divida un archivo en varios trozos con el mismo nombre
y extensión h00, h01, … . Se debe tener en cuenta el formato y consideraciones siguientes:
$ hacha <archivo> <tamaño>
<archivo> nombre del archivo a dividir
<tamaño> tamaño en bytes de los archivos divididos
 Para dividir, el proceso hacha (proceso padre) generará tantos procesos hijos como archivos se
tengan que crear. Mediante tuberías, el proceso padre enviará la información a los hijos y serán
estos últimos los que creen los archivos de destino y escriban en ellos.
$ hacha at_madrid.mp3 50000
$ ls
at_madrid.mp3.h00
at_madrid.mp3.h01
at_madrid.mp3.h02
Consideraciones
 Las entradas y salidas a los archivos se realizarán con llamadas al sistema estudiadas en la asignatura
(no se puede utilizar printf, scanf, etc.)
 El padre creará tantos hijos como fragmentos del archivo a realizar, siendo decisión del alumno si los
hijos se lanzan secuencial o concurrentemente.*/





#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h> 
#include<sys/stat.h>//stat
#include<sys/wait.h>

#define TAM_BUFFER 1024




int calcularFragmentos( char *archivo, int tamaño){

    struct stat info; //guarda información sobre el archivo
    stat(archivo, &info); //para obtener info del archivo, como su tamaño original
    return (info.st_size + tamaño - 1)/ tamaño; //calcula cuantos fragmentos vamos a necesitar
}


void crearFragmento(int fd[2], char *archivo, int fragmento){

    int destino;
    int leido;
    char nombre[500];
    char buffer[TAM_BUFFER];

    close(fd[1]);
    sprintf(nombre, "%s.h%02d", archivo, fragmento);
    destino=creat(nombre, 0666);

    while((leido=read(fd[0], buffer, TAM_BUFFER))>0){
        write(destino, buffer, leido);
    }
    close(destino);
    close(fd[0]);
    exit(0);
}




void enviarFragmento(int fd[2], int origen, int tamaño){

    int restante;
    int leido;
    char buffer[TAM_BUFFER];

    close(fd[0]);
    restante=tamaño;

    while(restante>0){
        if(restante<TAM_BUFFER){
            leido=read(origen, buffer, restante);
        }
        else{
            leido=read(origen, buffer, TAM_BUFFER);
        }

        if(leido>0){
            write(fd[1], buffer, leido);
           restante=restante-leido;
        }
        else{
            break;
        }
    }
    close(fd[1]);
    wait(NULL);
}




void crearFragmentos(char *archivo, int tamaño, int numero_fragmentos, int origen){

    int fd[2];
    int fragmento;
    pid_t pid;

    for(fragmento=0; fragmento<numero_fragmentos; fragmento++){

        pipe(fd);
        pid=fork();

        if(pid==0){
            crearFragmento(fd, archivo, fragmento);
        }
        enviarFragmento(fd, origen, tamaño);
    }
}










int main(int argc, char *argv[]){

    char *archivo;
    int tamaño;
    int origen;
    int numero_fragmentos;


    archivo=argv[1];

    tamaño=atoi(argv[2]);

    numero_fragmentos=calcularFragmentos(archivo, tamaño);

    origen=open(archivo, O_RDONLY);

    crearFragmentos(archivo, tamaño, numero_fragmentos, origen);

    close(origen);

    exit(0);


}































