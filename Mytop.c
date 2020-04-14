// Alunos: RICARDO ABREU DE OLIVEIRA, IGOR CASSIO TOLEDO FRANCO, WELLINGTON GABRIEL DE MATTIA
// Version: Sem %CPU

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>



int main (int argc, char *argv){
   
    char informacoesStat[10] = "stat";
    char informacoesStatus[10] = "status";
    clock_t tempoCPU;
    long long unsigned user, nice, system, idle;

    // Ponteiros para arquivos
    FILE *arquivoStat;
    FILE *arquivoStatus;

    // Criando um tipo diretorio
    DIR *diretorio;  
    DIR *dirHome;

    // O retorno da função readdir é um ponteiro para uma struct dirent
    struct dirent *dirProcesso;           
    struct dirent *estruturaUser;
    
    // VARIAVEIS DO PROCESSO
    pid_t pid;
    char nomeUser[20];
    char Uid[20];
    char comando[20];
    char estado;
    long int prioridade;
    long unsigned tempo;
    long unsigned usertime, systemtime,somaAnterior;
    char caminhoDoStat[100] = "/proc/";
    char caminhoDoStatus[100] = "/proc/";
    //**********************

    // Passo de pegar o nome no Usuario da sessao
    dirHome = opendir("/home/");
    while (estruturaUser = readdir(dirHome)){
        if (  (strcmp( estruturaUser->d_name, "..") != 0) && (strcmp( estruturaUser->d_name, ".") != 0)  ) // Eliminando diretorios nulos
            strcpy(nomeUser, estruturaUser->d_name);
    }

    
    diretorio = opendir("/proc");                       // ABRINDO o diretorio /proc, retorna um ponteiro para uma estrutura DIR
    if (diretorio == NULL){
        perror("Aconteceu algum erro!!");
        return(1);
    }
    else{
        while (*argv != '\0'){
            diretorio = opendir("/proc");

            // Impressão com %CPU
            //printf(" PID                 COMMAND      STATE      PR        TIME             CPU      USER\n");
            // Imoressão sem CPU
            printf("     PID                               COMMAND    STATE      PR        TIME           USER\n");
            
            printf("\r");             

            if (diretorio != NULL){
                while ( (dirProcesso = readdir(diretorio))){    // Lendo cada diretório(processo) ate não ter mais   
                    if (isdigit(*(dirProcesso->d_name))){       // Verifica se o nome do diretorio eh um número(processo)
                        
                        // Caminho para o stat
                        strcat(caminhoDoStat, dirProcesso->d_name);
                        strcat(caminhoDoStat,"/");
                        strcat(caminhoDoStat, informacoesStat);
                        
                        // Caminho para o status
                        strcat(caminhoDoStatus, dirProcesso->d_name);
                        strcat(caminhoDoStatus,"/");
                        strcat(caminhoDoStatus, informacoesStatus);
                        
                        arquivoStat = fopen(caminhoDoStat, "r");
                        arquivoStatus = fopen(caminhoDoStatus, "r");    // Pegar o User identifier

                        //***Verificando stat
                        if (arquivoStat != NULL){
                            // Leitura do arquivo stat no /proc/PID/
                            fscanf(arquivoStat, "%d %s %c %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu %lu %*s %*s %*s %ld", 
                                                &pid, comando, &estado, &usertime, &systemtime, &prioridade);

                            //Realizando a soma do tempo de usuario com tempo de kernel
                            tempo = usertime + systemtime;

                            // Impressão sem %CPU
                            if (prioridade < 0)
                                printf("%7.d   \t%30s \t%5c        %d\t  %lu   ", pid, comando, estado, (int)prioridade, tempo);
                            
                            else{
                                printf("%7.d   \t%30s \t%5c        %d\t      %5lu   ", pid, comando, estado, (int)prioridade, tempo);
                            }

                            fclose(arquivoStat);
                        }

                        //***Verificando status
                        if (arquivoStatus != NULL){
                            fscanf(arquivoStatus, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s", Uid);

                            // Se o Uid for 0000 é root se for 1000 é do Usuário
                            if (atoi(Uid) > 0){   
                                printf("        %s\n", nomeUser);
                                printf("\r");   // Volta para o começo da linha do terminal
                                
                            }
                            else{ 
                                printf("        root\n");
                                printf(  "\r"); // Volta para o começo da linha do terminal

                            }

                            fclose(arquivoStatus);
                        }
                        // Com o passar do tempo, fazendo a sobrescrita
                        // nas strings, há a chance de sobrar algum resquicío
                        // da string anterior, para  evitar isso, "zeramos" o vetor
                        memset(caminhoDoStat,'\0',sizeof(caminhoDoStat));
                        memset(caminhoDoStatus,'\0',sizeof(caminhoDoStatus));
                        
                        // Copiamos o caminho padrão para o diretório proc
                        strcpy(caminhoDoStat,"/proc/");
                        strcpy(caminhoDoStatus, "/proc/");
                    }   
                }
                // Desalocando memória
                free(dirProcesso);
            }
            closedir(diretorio);
            
            // Como o programa lê arquivos que foram escritos pelo SO,
            // pode-se ocorrer o caso em que a nossa ferramenta
            // tenta ler o arquivo, e acontece um erro, acreditamos que
            // seja pelo motivo de interromperemos o SO no momento da
            // escrita do arquivo. Por isso forçamos um "pequeno" delay.
            sleep(1);
        } 
    }

    return EXIT_SUCCESS;
}