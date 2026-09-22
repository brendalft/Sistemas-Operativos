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
#include<fcntl.h> //open() y O_RDONLY
#include<sys/stat.h>//stat
#include<sys/wait.h>

#define TAM_BUFFER 1024

int main(int argc, char *argv[]){



    char *archivo; //nombre del archivo original que vamos a dividir
    int tamaño; //tamaño en bytes de cada fragmento
    int origen, destino; //descriptores del archivo original y del fragmento

    int fd[2]; // fd[0] para lectura de la tubería  ----- fd[1] para escritura

    int numero_fragmentos; //numero de fragmentos que vamos a necesitar
    int fragmento;
    char nombre[500]; //para el nombre de los frgamentos

    struct stat info; //guarda información sobre el archivo

    int restante, leido;

    char buffer[TAM_BUFFER]; //guarda temporalmente los datos leídos

    pid_t pid;




    archivo=argv[1]; //guardamos el nombre del archivo que es un parametro
    tamaño=atoi(argv[2]); //guardamos el tamaño en bytes de cada fragmento que es otro parametro

    stat(archivo, &info); //para obtener info del archivo, como su tamaño original

    numero_fragmentos=(info.st_size + tamaño - 1)/ tamaño; //calcula cuantos fragmentos vamos a necesitar

    origen=open(archivo, O_RDONLY); //abre al archivo para leerlo


    for(fragmento=0; fragmento<numero_fragmentos; fragmento++){//se repetirá el mismo proceso para cada fragmento

        pipe(fd); //crea una tubería para que se comuniquen padre e hijo

        pid=fork(); //crea un hijo

        if(pid==0){//es un hijo, recibe los datos del padre y crea el fragmento


            close(fd[1]); //cerramos fd[1] porque el hijo no necesita escribir en la tubería


            sprintf(nombre, "%s.h%02d", archivo, fragmento); //crea el nombre del frgamentos

            destino=creat(nombre, 0666); //crea el archivo destino donde se guadará los datos recibidos --- 0666 da permisos de leer y escribir

            while((leido=read(fd[0], buffer, TAM_BUFFER))>0){

                write(destino, buffer, leido);//escribe en el archivo destino los datos necesarios

            }

            close(destino); //cerramos el archivo
            close(fd[0]); //cerramos la lectura de la tubería
            exit(0);//el hijo termina



        }

        //sino es un hijo, es el padre, lee el fragmento del archivo original y se lo envía al hijo

        close(fd[0]); //el padre no necesita leer de la tubería, solo escribir para enviarselo al hijo

        restante=tamaño; //tamaño de bytes que debe recibir el hijo

        while(restante>0){ //mientras queden bytes por enviar

            if(restante<TAM_BUFFER){//si quedan menos bytes que el tamaño del buffer

                leido=read(origen, buffer, restante); //lee los bytes que quedan

            }
            else{ //si quedan suficientes bytes para llenar el buffer

                leido=read(origen, buffer, TAM_BUFFER); //lee como maximo tantos bytes como el tamaño del buffer
            }

            if(leido>0){ //si se leyeron correctamente los datos

                write(fd[1], buffer, leido); //envía los datos al hijo mediante la tubería

                restante= restante-leido; //resta los bytes que ya se han enviado
            }
        }

        close(fd[1]); //cerramos la escritura de la tubería

        wait(NULL); //espera a que termine el hijo




    }

    close(origen); //cierra el archivo origen
    exit(0); //termina el padre





}































