#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <ctype.h>
#include <locale.h>

#define PERFORMANCE
#define tam_max_pal 64
#define tam_tabela 1024

struct fre{
    char palavra[tam_max_pal];
    int frequencia; 
};

struct item{
    char palavra[tam_max_pal];
    int frequencia; 

    struct item *proximo;
};

struct THash {
    struct item *balde[tam_tabela];
    pthread_mutex_t mutex[tam_tabela];
};

#ifdef PERFORMANCE
double tempoAtual() {
    struct timespec tempo;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tempo);
    return tempo.tv_sec + tempo.tv_nsec/1000000000.0;
}
#endif

int nThreads;
long long tamanhoTexto;
char *texto;
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

    while ((c = *palavra++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return abs((int) (hash % tam_tabela));
}

void inserir_palavra (char *palavra){
    char palavra2[tam_max_pal];
    strncpy(palavra2, palavra, tam_max_pal);

    int posicao = Hash((unsigned char*) palavra2);

    pthread_mutex_lock(&tabela.mutex[posicao]);

    struct item *atual = tabela.balde[posicao];

    while(atual){
        if(strcasecmp(atual->palavra, palavra2) == 0){
            atual->frequencia += 1;
            pthread_mutex_unlock(&tabela.mutex[posicao]);
            return;
        }
        atual = atual->proximo;
    }

    struct item *novo = (struct item*)malloc(sizeof(struct item));
    strncpy(novo->palavra, palavra2, tam_max_pal);
    novo->frequencia = 1;
    novo->proximo = tabela.balde[posicao]; 
    tabela.balde[posicao] = novo; 
    pthread_mutex_unlock(&tabela.mutex[posicao]); 
}

struct fre remover_palavra(){
    for (int i = 0; i < tam_tabela; i++){
        pthread_mutex_lock(&tabela.mutex[i]);

        struct item *par = tabela.balde[i];
        
        if (par != NULL){
            tabela.balde[i] = par->proximo;

            struct fre retorno;
            strncpy(retorno.palavra, par->palavra, tam_max_pal);
            retorno.frequencia = par->frequencia;
            free(par);

            pthread_mutex_unlock(&tabela.mutex[i]);
            return retorno;
        }

        pthread_mutex_unlock(&tabela.mutex[i]);

    }

    struct fre fim;
    fim.frequencia = 0;
    return fim;
}

void liberar_tabela() {
    for (int i = 0; i < tam_tabela; i++) {
        pthread_mutex_destroy(&tabela.mutex[i]);

        struct item *atual = tabela.balde[i];
        while (atual != NULL) {
            struct item *temp = atual;
            atual = atual->proximo;
            free(temp);
        }

        tabela.balde[i] = NULL;
    }
}

void *thread(void *tid) {
    long int id = (long int) tid;
    int ini, fim, p = 0;
    char palavra[tam_max_pal];

    ini = id * (tamanhoTexto/nThreads);
    if (id != 0){
        while (e_palavra(texto[ini]) != 0){
            ini++;
        }
    }

    if (id != nThreads - 1) {
        fim = (id + 1) * (tamanhoTexto/nThreads);
        fim++;
        while (fim < tamanhoTexto && e_palavra(texto[fim]) != 0) {
            fim++;
        }
    } else {
        fim = tamanhoTexto - 1;
    }

    for (int i = ini; i <= fim; i++)
    {
        if (e_palavra(texto[i]) == 1){
            palavra[p] = texto[i];
            p++;
        } else {
            if (p != 0) {
                palavra[p] = '\0';
                inserir_palavra(palavra);
                p = 0;
            }
        }
    }

    if (p != 0) {
    palavra[p] = '\0';
    inserir_palavra(palavra);
    p = 0;
}

    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "");
    double inicio, fim, delta;
    pthread_t *tid;

    if (argc < 3) {
        printf("O formato correto e \"%s <nome do arquivo a ser analisado>. <numero de threads>\"\n", argv[0]);
        return 1;
    }

    char *caminho = malloc(strlen(argv[1]) + 1);
    if(caminho==NULL) { printf("Erro em alocar memoria para o caminho\n"); return(2);}
    strcpy(caminho, argv[1]);

    FILE *arquivo;
    arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        printf("Erro em abrir o arquivo a ser analisado\n");
        return 3;
    }

    nThreads = atoi(argv[2]);
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nThreads);
    if(tid==NULL) { printf("Erro em alocar memoria para o tid\n"); return(4);}

    for (int i = 0; i < tam_tabela; i++) {
        tabela.balde[i] = NULL;
        pthread_mutex_init(&tabela.mutex[i], NULL);
    }

    struct stat file; 
    if (fstat(fileno(arquivo), &file) == 0) {
        tamanhoTexto = file.st_size;
    } else {
        printf("Erro em descobrir o tamanho do arquivo\n");
        return 5;
    }
    
    texto = malloc(tamanhoTexto + 1);
    if(texto==NULL) { printf("Erro em alocar memoria para o texto\n"); return(6);}
    
    #ifdef PERFORMANCE
    inicio = tempoAtual();
    #endif

    fread(texto, 1, tamanhoTexto, arquivo);

    texto[tamanhoTexto] = '\0';
    for (int i = 0; i < nThreads; i++) {
        if (pthread_create(&tid[i], NULL, thread, (void*) i)) {
            printf("Erro na criacao das threads\n"); 
            return 7;
        }
    }

    for (int i=0; i<nThreads; i++) {
        if (pthread_join(tid[i], (void*) NULL)) {
            fprintf(stderr, "Erro na finalizacao das threads\n"); 
            return 8;
        }
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
    fprintf(performance, "O arquivo %s, foi analisado de forma concorrete em %f segundos com %d threads\n", caminho, delta, nThreads);
    fclose(performance);
    #endif
    
    free(texto);
    free(caminho);
    free(tid);
    liberar_tabela();

    return 0;
}
