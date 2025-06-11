#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define PERFORMANCE
#define tam_max_pal 64
#define tam_tabela 1024


struct fre
{
    char palavra[tam_max_pal];
    int frequencia; //verificar se int basta mesmo para os textos mais longos
};

struct item{
    char palavra[tam_max_pal];
    int frequencia; 

    struct item *proximo;
};

struct THash {
    struct item *balde[tam_tabela];
};

#ifdef PERFORMANCE
double tempoAtual() {
    struct timespec tempo;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tempo);
    return tempo.tv_sec + tempo.tv_nsec/1000000000.0;
}
#endif

struct THash tabela;

int e_palavra(char c){
    switch (c)
    {
    case ',':
        return 0;
        break;
    
    case '.':
        return 0;
        break;
        
    case ';':
        return 0;
        break;

    case '/':
        return 0;
        break;

    case '\\':
        return 0;
        break;

    case '|':
        return 0;
        break;

    case '?':
        return 0;
        break;

    case '!':
        return 0;
        break;

    case '"': 
        return 0;
        break;

    case '\'':
        return 0;
        break;

    case '(':
        return 0;
        break;

    case ')':
        return 0;
        break;

    case '[':
        return 0;
        break;

    case ']': 
        return 0;
        break;

    case '{': 
        return 0;
        break;

    case '}': 
        return 0;
        break;

    case '<': 
        return 0;
        break;

    case '>':
        return 0;
        break;

    case '%': 
        return 0;
        break;

    case '*':
        return 0;
        break;

    case '-':
        return 0;
        break;

    case '_':
        return 0;
        break;

    case '+':
        return 0;
        break;

    case '=':
        return 0;
        break;

    case '1':
        return 0;
        break;

    case '2':
        return 0;
        break;

    case '3':
        return 0;
        break;

    case '4':
        return 0;
        break;

    case '5':
        return 0;
        break;

    case '6':
        return 0;
        break;

    case '7':
        return 0;
        break;

    case '8':
        return 0;
        break;

    case '9':
        return 0;
        break;

    case '0':
        return 0;
        break;

    case '™':
        return 0;
        break;

    case '©':
        return 0;
        break;

    case '\n':
        return 0;
        break;

    case ' ':
        return 0;
        break;

    case '\r':
        return 0;
        break;
    
    default:
        return 1;
        break;
    }
}

int Hash(unsigned char *palavra){
    unsigned long hash = 5381;
    int c;

    while (c = *palavra++){
        hash = ((hash << 5) + hash) + c;
    }

    return (int) (hash % tam_tabela);
}

void inserir_palavra (char *palavra){
    char palavra2[tam_max_pal];
    strncpy(palavra2, palavra, tam_max_pal);

    int posicao = Hash(palavra2);

    struct item *atual = tabela.balde[posicao];

    while(atual){
        if(strcasecmp(atual->palavra, palavra2) == 0){
            atual->frequencia += 1;
            return;
        }
        atual = atual->proximo;
    }

    struct item *novo = (struct item*)malloc(sizeof(struct item));
    strncpy(novo->palavra, palavra2, tam_max_pal);
    novo->frequencia = 1;
    novo->proximo = tabela.balde[posicao]; 
    tabela.balde[posicao] = novo; 
}

struct fre remover_palavra(){
    for (int i = 0; i < tam_tabela; i++){

        struct item *par = tabela.balde[i];
        
        if (par != NULL){
            tabela.balde[i] = par->proximo;

            struct fre retorno;
            strncpy(retorno.palavra, par->palavra, tam_max_pal);
            retorno.frequencia = par->frequencia;
            free(par);

            return retorno;
        }

    }

    struct fre fim;
    fim.frequencia = 0;
    return fim;
}

int main(int argc, char const *argv[])  
{
    double inicio, fim, delta;
    if (argc < 2) {
        printf("O formato correto e \"%s <nome do arquivo a ser analizado>.\"\n", argv[0]);
        return 1;
    }

    char *caminho = malloc(strlen(argv[1]) + 1);
    strcpy(caminho, argv[1]);

    FILE *arquivo;
    arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        printf("Erro em abrir o arquivo a ser analizado");
        return 2;
    }

    for (int i = 0; i < tam_tabela; i++) {
        tabela.balde[i] = NULL;
    }
    
    #ifdef PERFORMANCE
    inicio = tempoAtual();
    #endif

    char c = fgetc(arquivo);
    char palavra[tam_max_pal];
    int p = 0;

    while (c != EOF)
    {
        if (e_palavra(c) == 1)
        {
            palavra[p] = c;
            p++;
        }else{
            if(p != 0){
                palavra[p] = '\0';
                inserir_palavra(palavra);
                p = 0;
            }
            
        }
        c = fgetc(arquivo);                
    }

    #ifdef PERFORMANCE
    fim = tempoAtual();
    #endif

    fclose(arquivo);

    FILE *retorno;
    retorno = fopen("frequência.txt", "w");
    struct fre a = remover_palavra();
    while (a.frequencia != 0){
        fprintf(retorno, "%s: %d\n", a.palavra, a.frequencia);
        a = remover_palavra();
    } 

    #ifdef PERFORMANCE
    delta = fim - inicio;
    FILE *performance;
    performance = fopen("performance.txt", "a");
    fprintf(performance, "O arquivo %s, foi analizado de forma sequencial em %f segundos\n", caminho, delta);
    #endif

    return 0;
}
