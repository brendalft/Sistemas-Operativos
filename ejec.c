/*b) Realiza un programa llamado ejec.c que reciba un argumento. El programa tendrá que generar
el árbol de procesos que se indica y llevar a cabo la funcionalidad que se describe a
continuación. El proceso Z, transcurridos los segundos indicados por el argumento, enviará una
señal al proceso A. El proceso A, al recibir la señal, ejecutará el comando “pstree”. El proceso
Z no puede utilizar el comando “sleep”, por lo que el proceso Z debe planificarse una alarma
con los segundos indicados por el argumento. Se deberá controlar la correcta destrucción del
árbol (los padres no pueden morir antes que los hijos).  (2 puntos) */



#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h> 




pid_t pid_abueloA; //pid de A, lo necesita Z


//se ejecuta cuando A recibe SIGUSR1
void ejecutar_pstree(int numero){

    pid_t pid_hijoA;

    pid_hijoA=fork(); //A crea un hijo para que ejecute pstree

    if(pid_hijoA==0){// es un hijo

        execlp("pstree","pstree", NULL); //el hijp ejecuta el comando pstree

        perror("Error al ejecutar pstree");//si execlp falla, muestra el error

        exit(1); // el hijo de A termina , se pone un 1 porque significa que termina con un error



    }


}


//se ejecuta cuando Zcibe SIGALRM
void ejecutar_alarma(int numero){

    kill(pid_abueloA, SIGUSR1); //Z envía SIGUSR1 a A
}


int main(int argc, char *argv[]){

    pid_t pid, pid_ejec, pid_A, pid_B, pid_X, pid_Y, pid_Z;
    int estado;
    int resultado;
    int segundos;

    segundos= atoi(argv[1]); //convertimos el argumento a un entero



    pid_ejec = getpid(); //pid del proceso inicial (lo primero del árbol)

    printf("Soy el proceso ejec: mi pid es %d\n", pid_ejec);


    pid=fork(); //creamos el proceso A

    if(pid==0){ // si el pid es cero, entonces es un hijo, es el hijo A

        pid_A=getpid(); //pid de A

        pid_abueloA = pid_A; //guardamos el pid de A en pid_abueloA para que después Z pueda usarlo

        printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", pid_A, pid_ejec);

        signal(SIGUSR1, ejecutar_pstree); //A ejecutará pstree cuanto reciba la señal SIGUSR1



        pid=fork(); //A crea a B

        if(pid==0){//es un hijo, B

            pid_B=getpid(); //guardamos el pid de B

            printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", pid_B, pid_A, pid_ejec);



            pid=fork(); //B crea a X

            if(pid==0){ //es un hijo, X

                pid_X=getpid();//guardamos el pid de X

                printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", pid_X, pid_B, pid_A, pid_ejec);


                sleep(segundos); // X espera esos segundos

                printf("Soy X (%d) y muero\n", pid_X);

                exit(0); //X termina

            }

            pid=fork(); //B crea a Y

            if(pid==0){  // es un hijo, Y

                pid_Y = getpid();

                printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuedlo es %d. Mi bisabuelo es %d\n", pid_Y, pid_B, pid_A, pid_ejec);

                sleep(segundos);  //Y espera esos segundos
                printf("Soy Y (%d) y muero\n", pid_Y);
                exit(0); //Y termina

            }

            pid=fork(); //B crea a Z

            if(pid==0){// es un hijo, Z

                pid_Z=getpid();

                printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", pid_Z, pid_B, pid_A, pid_ejec);


                signal(SIGALRM, ejecutar_alarma); //Z ejecutará ejecutar_alarma cuando reciba la señal SIGALRM

                alarm(segundos);//Z programa una alarma, no puede usar sleep

                pause(); //Z espera hasat que reciba una señal

                printf("Soy Z (%d) y muero\n", pid_Z);

                exit(0); //cuando recibe la señal muere
            }


            wait(NULL); // B espera a que termine X
            wait(NULL); // B espera a que termine Y
            wait(NULL); // B espera a que termine Z

            printf("Soy B (%d) y muero\n", pid_B);

            exit(0); //cuando terminen sus hijos, B muere

        }

        //A espera a que B termine
        do{
            resultado=wait(&estado);
        }while(resultado==-1);  //cuando el resulatdo sea 0, B murió

        printf("Soy A (%d) y muero\n", pid_A);

        exit(0);



    }

    wait(NULL); //ejec espera a que termine A

    printf("Soy ejec (%d) y muero\n", pid_ejec);
    exit(0);








}
























