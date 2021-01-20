//Autor: Kassuelo Moreira Okaszseski
//Github: https://github.com/kassuelo/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "tempo.h"

#define TAM 30688

int NUMTHREAD;

void *exec_func(void *args);    //função q a thread executa
int compara_genoma();
int exibe_resultado();
int compara_sequencia();
void inicializa_matriz();
void mostra_iguais();
void mostra_seq();
void mostra_indices();
void modo_sequencial();

int id[45]; //armazena os id das threads
int qtd_igual[10][10];
int maior_seq[10][10];
int indices[45][2]; //armazena o indice referente a cada comparação
int inicio[45]; //armazena os indices em que cada thread irá iniciar
int qtd; //qtd de elementos q cada thread irá percorrer

void main(int argc, char *argv[])
{
    printf("\n* digite 0(zero) para o modo sequencial:");
    printf("\nDigite a quantidade de threads (Max=45):\n");
    scanf("%d", &NUMTHREAD);
    tempo1();
    if (NUMTHREAD > 45) //45 é o máximo de comparações entre os 10 arquivos
    {
        NUMTHREAD = 45;
    }
    if (NUMTHREAD == 0) //caso digite 0 será executado o modo sequencial, sem threads
    {
        inicializa_matriz();
        mostra_indices();
        modo_sequencial();
        mostra_seq();
        mostra_iguais();
        tempo2();
        tempoFinal("milissegundos", argv[0], MSGLOG);
    }
    else
    {
        printf("%d threads serão criadas.\n", NUMTHREAD);
        pthread_t pidt[NUMTHREAD];
        qtd = ceil((double)(45) / NUMTHREAD); //determina quantas comparações cada thread executará
        inicio[0] = 0;
        inicializa_matriz();
        mostra_indices();
        for (int i = 0; i < NUMTHREAD; i++)
        {
            id[i] = i;
            //envia o indice da tabela de comparações que a thread iniciara
            pthread_create(&pidt[i], NULL, exec_func, (void *)&id[i]); 
            printf("\n************ Thread %d - PID %lu iniciada ************\n", i, pidt[i]);
            if (i < (NUMTHREAD - 1))
            {
                inicio[i + 1] = inicio[i] + qtd;
            }
        }
        for (int j = 0; j < NUMTHREAD; j++)
        {
            pthread_join(pidt[j], NULL);
        }
        mostra_seq();
        mostra_iguais();
        tempo2();
        tempoFinal("milissegundos", argv[0], MSGLOG);
    }
}

void inicializa_matriz()
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            qtd_igual[i][j] = 0;
            maior_seq[i][j] = 0;
        }
    }

    int i = 0;
    for (int k = 0; k < 9; k++)
    {

        for (int z = k + 1; z <= 9; z++)
        {
            if (i < 45)
            {
                indices[i][0] = k;
                indices[i][1] = z;
                i++;
            }
        }
    }
}

void mostra_iguais()
{
    int maior_qtd_iguais=0;
    int arq1;
    int arq2;
    printf("\n MATRIZ IGUAIS\n");
    printf("\n arq0 | arq1 | arq2 | arq3 | arq4 | arq5 | arq6 | arq7 | arq8 | arq9 |\n\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if(maior_qtd_iguais<qtd_igual[i][j]){
                maior_qtd_iguais= qtd_igual[i][j];
                arq1=i;
                arq2=j;
            }
            printf(" %5d|", qtd_igual[i][j]);
        }
        printf("\n");
    }
    printf("\nMaior quantidade de iguais = %d, entre os genomas %d e %d\n\n",maior_qtd_iguais,arq1,arq2);
}

void mostra_indices()
{
    printf("\n MATRIZ COMPARAÇÕES\n");
    printf("\n| Nr | arq1 | arq2 |\n\n");
    for (int i = 0; i < 45; i++)
    {
        if (i < 10)
        {
            printf("| %d  |  %d   |  %d   |\n", i, indices[i][0], indices[i][1]);
        }
        else
        {
            printf("| %d |  %d   |  %d   |\n", i, indices[i][0], indices[i][1]);
        }
    }
    printf("\n\n");
}

void mostra_seq()
{
    int maior_sequencia_de_todas=0;
    int arq1;
    int arq2;
    printf("\n MATRIZ SEQUÊNCIAS\n");
    printf("\n arq0 | arq1 | arq2 | arq3 | arq4 | arq5 | arq6 | arq7 | arq8 | arq9 |\n\n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if(maior_sequencia_de_todas<maior_seq[i][j]){
                maior_sequencia_de_todas= maior_seq[i][j];
                arq1=i;
                arq2=j;
            }
            printf(" %5d|", maior_seq[i][j]);
        }
        printf("\n");
    }
    printf("\nMaior sequência de todas = %d, entre os genomas %d e %d\n\n",maior_sequencia_de_todas,arq1,arq2);
}

void *exec_func(void *args)
{
    int id = *(int *)args;
    int ini = inicio[id];
    int final = (ini + qtd);
    if (final > 45)
    {
        final = 45; //45 comparações
    }

    printf("\nthread %d - inicia da comparação %d até a comparação %d\n\n\n", id, ini, final - 1);
    for (int i = ini; i < final; i++)
    {
        //envia dinamicamente o nome do arquivo de deve ser aberto
        char g1[14] = "genomas/";
        g1[8] = indices[i][0] + '0';
        strcat(g1, ".txt");
        char g2[14] = "genomas/";
        g2[8] = indices[i][1] + '0';
        strcat(g2, ".txt");
        //envia o nome dos 2 arquivos e os indices de inicio e fim das comparações
        compara_genoma(g1, g2, indices[i][0], indices[i][1]);
    }
    return (NULL);
}

//funcao carrega genomas
//===============================================================
int compara_genoma(char p[], char s[], int nr_arq1, int nr_arq2)
{
    char genoma1[TAM];
    char genoma2[TAM];
    int i;
    char c;

    //abre o primeiro
    FILE *file1;
    file1 = fopen(p, "r");
    i = 0;
    while ((c = getc(file1)) != EOF)
    {
        genoma1[i] = c;
        i++;
    }
    fclose(file1);

    //abre o segundo arquivo
    FILE *file2;
    file2 = fopen(s, "r");
    i = 0;
    while ((c = getc(file2)) != EOF)
    {
        genoma2[i] = c;
        i++;
    }
    fclose(file2);

    int igual = 0;
    int dif = 0;
    int seq_acumulada = 0;
    int total = 0;
    int inicio;

    for (int i = 0; i < TAM; i++)
    {
        if (genoma1[i] != NULL)
        {
            if ((genoma1[i] == genoma2[i]))
            {
                seq_acumulada++;
                igual++;
            }
            else
            {
                if (seq_acumulada > total)
                {

                    total = seq_acumulada;
                    inicio = i - total;
                    seq_acumulada = 0;
                }
                dif++;
            }
        }
    }
    if (seq_acumulada > total)
    {
        total = seq_acumulada;
        inicio = i - total;
        seq_acumulada = 0;
    }
    qtd_igual[nr_arq1][nr_arq2] = igual; //escreve na matriz o valor encontrado
    maior_seq[nr_arq1][nr_arq2] = total;
}

void modo_sequencial()
{
    printf("\nMODO SEQUENCIAL\ninicia da comparação 0 até a comparação 44\n\n\n");
    for (int i = 0; i < 45; i++)
    {
        char g1[14] = "genomas/";
        g1[8] = indices[i][0] + '0';
        strcat(g1, ".txt");
        char g2[14] = "genomas/";
        g2[8] = indices[i][1] + '0';
        strcat(g2, ".txt");
        compara_genoma(g1, g2, indices[i][0], indices[i][1]);
    }
}