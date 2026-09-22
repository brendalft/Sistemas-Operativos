#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>


int main(int argc, char *argv[]){


    int x, y; // para los argumentos
    int i, j , k;
    pid_t pid;
    pid_t *finales;
    int shmid;

    x=atoi(argv[1]); //numero de procesos de cada columna
    y=atoi(argv[2]); //numero de columnas


    shmid=shmget(IPC_PRIVATE, y *sizeof(pid_t), IPC_CREAT|0666); //crea la memoria compartida para guardar los pid de los hijos finales

    finales=(pid_t*) shmat(shmid, 0, 0); //une el proceso a la memoria compartida

    for(j=0; j<y; j++){ //repite la crecaión de las columnas 'y' veces

        pid=fork();

        if(pid==0){//es un hijo

            pid_t array[x]; //array donde se guardan los pid de los procesos de esa columna

            array[0]=getpid(); //guarda el pid del primer proceso de la columna

            for(i=1; i<x; i++){//crea el resto de procesos


                pid=fork();

                if(pid!=0){ //si es el padre, deja de crear procesos en esa columna
                    break;
                }

                array[i]=getpid(); //el nuevo hijo guarda su pid en la posicion siguiente


            }

            if(i==x){ //para comprobar si ese proceso es el último de la columna


                finales[j]=getpid(); //si es el último proceso de la columna, guarda su pid en la memoria compartida

                printf("Soy el subhijo %d, mis padres son: ", getpid()); //muestra el pid del subhijo final


                //para mostrar los pid de los padres
                for(k=0; k<x-1; k++){
                    printf("%d ", array[k]);
                }

                printf("\n"); //salto de línea

            }

            sleep(2); //espera 2 segundos antes de terminar
            exit(0);//termina ese proceso
        }

    }


    //el superpadre espera a que terminen todos sus hijos
    for(j=0; j<y; j++){
        wait(NULL);
    }

    printf("Soy el superpadre (%d): mis hijos finales son: ", getpid()); //muestra el pid del superpadre

    //para mostrar el pid de los hijos finales
    for(j=0; j<y; j++){
        printf("%d ", finales[j]);
    }

    printf("\n"); //salto de línea


    shmdt((char*)finales);  //separa al superpadre de la memoria compartida

    shmctl(shmid, IPC_RMID,0); //elimina la memoria compartida

    exit(0); //el superoadre termina


}















