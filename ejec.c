#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pid_abueloA; //pid de A, lo necesita Z
pid_t pid_hijoB;   //pid de B, lo necesita A
pid_t hijoX, hijoY, hijoZ; //pids de X, Y y Z, los necesita B



void ejecutar_pstree(int numero){//se ejecuta cuando A recibe SIGUSR1

    pid_t pid_hijoA;
    pid_hijoA=fork();//A crea un hijo para que ejecute pstree

    if(pid_hijoA==0){//es un hijo

        execlp("pstree","pstree", NULL); //el hijo ejecuta el comando pstree
        perror("Error al ejecutar pstree");//si execlp falla, muestra el error
        exit(1); //el hijo de A termina con error
    }

    wait(NULL);//A espera a que termine pstree
    kill(pid_hijoB, SIGUSR2); //A manda una señla a B porque ya pueden morir sus hijos
}


//se ejecuta cuando Z recibe SIGALRM
void ejecutar_alarma(int numero){

    kill(pid_abueloA, SIGUSR1); //Z envía SIGUSR1 a A
}


//Se ejecuta cuando B recibe SIGUSR2
void despertar_hijos(int numero){

    kill(hijoX, SIGUSR2);
    kill(hijoY, SIGUSR2);
    kill(hijoZ, SIGUSR2);
}


//Se ejecuta cuando X, Y o Z reciben SIGUSR2, para que pause termine
void despertar(int numero){
}


void crearX(pid_t pid_B, pid_t pid_A, pid_t pid_ejec, int segundos){

    pid_t pid_X;

    pid_X=getpid();//guardamos el pid de X

    printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n",
           pid_X, pid_B, pid_A, pid_ejec);

    signal(SIGUSR2, despertar); 

    pause(); //X espera hasta que B le avise 

    printf("Soy X (%d) y muero\n", pid_X);

    exit(0); //X termina
}



void crearY(pid_t pid_B, pid_t pid_A, pid_t pid_ejec, int segundos){

    pid_t pid_Y;

    pid_Y=getpid();

    printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuedlo es %d. Mi bisabuelo es %d\n",
           pid_Y, pid_B, pid_A, pid_ejec);

    signal(SIGUSR2, despertar); 

    pause(); //Y espera hasta que B le avise

    printf("Soy Y (%d) y muero\n", pid_Y);

    exit(0); //Y termina
}



void crearZ(pid_t pid_B, pid_t pid_A, pid_t pid_ejec, int segundos){

    pid_t pid_Z;

    pid_Z=getpid();

    printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n",
           pid_Z, pid_B, pid_A, pid_ejec);

    signal(SIGALRM, ejecutar_alarma); //Z ejecutará ejecutar_alarma cuando reciba la señal SIGALRM

    signal(SIGUSR2, despertar);

    alarm(segundos);//Z programa una alarma ya que no puede usar sleep

    pause(); //Z espera hasta que reciba una señal

    pause(); //Z espera hasta que B le avise, cuando pstree termine

    printf("Soy Z (%d) y muero\n", pid_Z);

    exit(0); //cuando recibe la señal muere
}



void crearB(pid_t pid_A, pid_t pid_ejec, int segundos){

    pid_t pid, pid_B;

    pid_B=getpid();

    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n",
           pid_B, pid_A, pid_ejec);

    signal(SIGUSR2, despertar_hijos); //B reenvia la señal a sus hijos


    pid=fork(); //B crea a X

    hijoX=pid; 

    if(pid==0){ //es un hijo, X

        crearX(pid_B, pid_A, pid_ejec, segundos);

    }


    pid=fork(); //B crea a Y

    hijoY=pid; 
    if(pid==0){  //es un hijo, Y

        crearY(pid_B, pid_A, pid_ejec, segundos);

    }


    pid=fork(); //B crea a Z

    hijoZ=pid; 

    if(pid==0){//es un hijo, Z

        crearZ(pid_B, pid_A, pid_ejec, segundos);

    }


    wait(NULL); // B espera a que termine X
    wait(NULL); // B espera a que termine Y
    wait(NULL); // B espera a que termine Z

    printf("Soy B (%d) y muero\n", pid_B);
    exit(0); //cuando terminen sus hijos muere
}



void crearA(pid_t pid_ejec, int segundos){

    pid_t pid, pid_A;

    pid_A=getpid();

    pid_abueloA = pid_A; //guardamos el pid de A para que después Z pueda usarlo

    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n",
           pid_A, pid_ejec);

    signal(SIGUSR1, ejecutar_pstree); //A ejeucta pstree cuando recibe la señal SIGUSR1


    pid=fork(); //A crea a B

    pid_hijoB=pid; 

    if(pid==0){//es un hijo, B

        crearB(pid_A, pid_ejec, segundos);

    }


    //A espera a que terminen sus dos hijos: B y el que ejecuta pstree
    wait(NULL);
    wait(NULL);

    printf("Soy A (%d) y muero\n", pid_A);

    exit(0);
}



void crearProcesoA(pid_t pid_ejec, int segundos){

    pid_t pid;

    pid=fork(); 

    if(pid==0){ // si el pid es cero, entonces es un hijo, es el hijo A

        crearA(pid_ejec, segundos);

    }
}


int main(int argc, char *argv[]){

    pid_t pid_ejec;
    int segundos;

    segundos=atoi(argv[1]); 
    pid_ejec=getpid(); //pid de ejec

    printf("Soy el proceso ejec: mi pid es %d\n", pid_ejec);

    crearProcesoA(pid_ejec, segundos);

    wait(NULL); //ejec espera a que termine A

    printf("Soy ejec (%d) y muero\n", pid_ejec);

    exit(0);
}
