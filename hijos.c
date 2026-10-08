#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>


int crearMemoriaCompartida(int y){

    int shmid;
    shmid=shmget(IPC_PRIVATE, y *sizeof(pid_t), IPC_CREAT|0666);
    return shmid;
}


pid_t *unirMemoriaCompartida(int shmid){

    pid_t *finales;
    finales=(pid_t*) shmat(shmid, 0, 0);
    return finales;
}


void crearHijosFinales(int x, int y, pid_t *finales, pid_t array[]){

    int j, k;
    pid_t pid;

    for(j=0; j<y; j++){
        pid=fork();

        if(pid==0){
            finales[j]=getpid();
            printf("Soy el subhijo %d, mis padres son: ", getpid());

            for(k=0; k<x; k++){
                printf("%d ", array[k]);
            }
            printf("\n");
            exit(0);
        }
    }

    for(j=0; j<y; j++){
        wait(NULL);
    }
}


void mostrarSuperpadre(int y, pid_t *finales){

    int j;
    printf("Soy el superpadre (%d): mis hijos finales son: ", getpid());

    for(j=0; j<y; j++){
        printf("%d ", finales[j]);
    }
    printf("\n");
}


void eliminarMemoriaCompartida(int shmid, pid_t *finales){

    shmdt((char*)finales);
    shmctl(shmid, IPC_RMID,0);
}


void crearCadena(int x, int y, pid_t *finales, int shmid){

    int i;
    pid_t pid;
    pid_t array[x];

    array[0]=getpid();
    
    for(i=1; i<x; i++){
        pid=fork();
        
        if(pid!=0){
            wait(NULL);
            if(getpid()!=array[0]){
                exit(0);
            }
            break;
        }
        array[i]=getpid();
    }
    if(i==x){
        crearHijosFinales(x, y, finales, array);
    }
    if(getpid()==array[0]){
        mostrarSuperpadre(y, finales);
        eliminarMemoriaCompartida(shmid, finales);
    }
}


int main(int argc, char *argv[]){

    int x, y;
    int shmid;
    pid_t *finales;

    x=atoi(argv[1]);
    y=atoi(argv[2]);

    shmid=crearMemoriaCompartida(y);

    finales=unirMemoriaCompartida(shmid);

    crearCadena(x, y, finales, shmid);

    exit(0);
}

















