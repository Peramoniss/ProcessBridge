#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CARROS_A 10
#define CARROS_B 12
#define CARROS_C 4
#define CARROS_D 7

#define CARROS_VOLTA 3

enum controlador {
    LISTA_A,
    LISTA_B,
    LISTA_C,
    LISTA_D
};

pthread_mutex_t mutex_ida;
pthread_mutex_t mutex_volta;
int idas = 0, voltas = 0;

struct lista_item {
	int valor;
    char lista;
	struct lista_item *next;
} typedef LISTA;

void *ponte_ida(void *args)
{
    LISTA * carro = (LISTA *)args;
    pthread_mutex_lock(&mutex_ida);
    printf("->E:Carro %i Entrando na ponte, vindo da fila %c\n", carro->valor, carro->lista);
    idas--;
    printf("Fila de carros nesse momento: %d", idas);
    printf("\n");


    sleep(rand() % 2);
    printf("->S:Carro %i Saindo da ponte\n", carro->valor);
    pthread_mutex_unlock(&mutex_ida);
    sleep(1);
}

void adiciona_lista(LISTA **inicio, int num, char list){
    LISTA* carro = malloc(sizeof(LISTA));
    LISTA* temp = *inicio;
    carro->valor = num;
    carro->lista = list;
    carro->next = NULL;

    if(temp != NULL){
        while (temp->next != NULL){
            temp = temp->next;
        }
        temp->next = carro;
    }else{
        *inicio = carro;
    }
}

void adiciona_meio_lista(LISTA **inicio, int num, char list, int pos){
    LISTA* carro = malloc(sizeof(LISTA));
    LISTA* temp = *inicio;
    carro->valor = num;
    carro->lista = list;
    carro->next = NULL;
    int j = 0;

    if(temp != NULL){
        while (temp->next != NULL && j < pos-1){
            temp = temp->next;
            j++;
        }
        carro->next = temp->next;
        temp->next = carro;

    }else{
        *inicio = carro;
    }
}

void percorre_lista(LISTA *inicio){
    LISTA* temp = inicio;
    printf("Escrevendo lista:");
    while (temp != NULL){
        printf("%i ",temp->valor);
        temp = temp->next;
    }
    printf("\n");
}

int lista_vazia(LISTA *inicio){
    return inicio == NULL ?  1 : 0; 
}

LISTA *carro_avanca(LISTA **inicio){
    if(*inicio == NULL)
        return NULL;

    LISTA *temp = *inicio;
    *inicio = (*inicio)->next;
    return temp;
}

int main()
{
    /*
        Exemplo de uso de prioridades:

        pthread_attr_t attr;
        struct sched_param param;
        param.sched_priority = 2;
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&transito_volta[i], &attr, ponte_ida, (void *)i);
    */
    LISTA *lista_A = NULL, *lista_B = NULL, *lista_C = NULL, *lista_D = NULL;
    int i;
    pthread_t transito_ida[CARROS_A], transito_volta[CARROS_B], transito;

    srand(time(NULL));
    pthread_mutex_init(&mutex_ida, NULL);
    pthread_mutex_init(&mutex_volta, NULL);

    //adiciona_meio_lista(&lista_A, 269, 'A', 3);

    for (i = 0; i < CARROS_A; i++){
        adiciona_lista(&lista_A, i, 'A');
        idas++;
    }

    for (i = CARROS_A; i < CARROS_A+CARROS_B; i++){
        adiciona_lista(&lista_B, i, 'B');
        idas++;
    }

    for (i = CARROS_A + CARROS_B; i < CARROS_A+CARROS_B+CARROS_C; i++){
        adiciona_lista(&lista_C, i, 'C');
        idas++;
    }

    for (i = CARROS_A + CARROS_B + CARROS_C; i < CARROS_A+CARROS_B+CARROS_C+CARROS_D; i++){
        adiciona_lista(&lista_D, i, 'D');
        idas++;
    }

    //adiciona_meio_lista(&lista_A, 735, 'A', 3);

    percorre_lista(lista_A);
    percorre_lista(lista_B);
    percorre_lista(lista_C);
    percorre_lista(lista_D);

    enum controlador controller = LISTA_A, meia_fila = LISTA_C;
    int controla_fila_meio = 0;
    while (lista_vazia(lista_A) == 0 || lista_vazia(lista_B) == 0 || lista_vazia(lista_C) == 0 || lista_vazia(lista_D) == 0){ //enquanto uma das filas ainda possuir carros

        //criar quatro filas diferentes, e o join vai estabelecer a ordem
        //join espera topo da fila A ou B, dependo do controlador. 
        //outro controlador para intercalar C e D entrando no meio da B

        if(controller == LISTA_A){
            LISTA *carro_atual = carro_avanca(&lista_A);
            if(carro_atual!=NULL){
                pthread_create(&transito, NULL, ponte_ida, (void *)carro_atual);
                pthread_join(transito, NULL);
            }
        }else{
            LISTA *carro_atual = carro_avanca(&lista_B);
            if(carro_atual!=NULL){
                pthread_create(&transito, NULL, ponte_ida, (void *)carro_atual);
                pthread_join(transito, NULL);
            }
            controla_fila_meio++;
            ///printf("\nFILA MEIO: %d\n", controla_fila_meio);
        }

        if(controller == LISTA_A) controller = LISTA_B;
        else controller = LISTA_A;

        if(controla_fila_meio >= 2 || lista_vazia(lista_B) == 1){ //foi o ciclo ou a fila B ja esvaziou
            controla_fila_meio = 0;

            if(meia_fila == LISTA_C){ //e a vez de C e C ainda nao esvaziou
                //printf("\nTROCAS\n");
                LISTA *carro_entrando = carro_avanca(&lista_C);
                if(carro_entrando!=NULL){
                    adiciona_meio_lista(&lista_B, carro_entrando->valor, carro_entrando->lista, 3);
                    //percorre_lista(lista_B);
                    meia_fila = LISTA_D;
                }else goto fim;
            } 
            else if(meia_fila == LISTA_D){
                LISTA *carro_entrando = carro_avanca(&lista_D);
                if(carro_entrando!=NULL){
                    adiciona_meio_lista(&lista_B, carro_entrando->valor, carro_entrando->lista, 3);
                    //percorre_lista(lista_B);
                    meia_fila = LISTA_C;
                }else goto fim;
            }else{
                fim: 
                if(meia_fila == LISTA_C) meia_fila = LISTA_D;
                else meia_fila = LISTA_C;

                // printf("Controlador: %d", meia_fila);

                // percorre_lista(lista_A);
                // percorre_lista(lista_B);
                // percorre_lista(lista_C);
                // percorre_lista(lista_D);
            }
        }
    }

    // for (i = 0; i < CARROS_B; i++){
    //     pthread_join(transito_volta[i], NULL);
    // }

    // for (i = 0; i < CARROS_VOLTA; i++){
    //     pthread_join(transito_voltatransito_ida[i], NULL);
    // }
    printf("foram todos");
}