#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define CARROS_IDA 5
#define CARROS_VOLTA 3
pthread_mutex_t mutex_ida;
pthread_mutex_t mutex_volta;
int idas = 0, voltas = 0;

void *ponte_ida(void *args)
{
    int id = (int)args;
        pthread_mutex_lock(&mutex_ida);
        printf("->E:Carro %i Entrando na ponte de A para B\n", id);
        printf("Fila de carros nesse momento: ");
        for (int i = 0; i < idas-1; i++){
            printf("->");
        }
        printf("\n");


        sleep(rand() % 10);
        printf("->S:Carro %i Saindo da ponte de A para B\n", id);
        idas--;
        pthread_mutex_unlock(&mutex_ida);
        sleep(rand() % 10);
}

void *ponte_volta(void *args)
{
    int id = (int)args;
        pthread_mutex_lock(&mutex_volta);
        printf("<-E:Carro %i Entrando na ponte de B para A\n", id+CARROS_IDA);
        printf("Fila de carros nesse momento: ");
        for (int i = 0; i < voltas-1; i++){
            printf("<-");
        }
        printf("\n");
        sleep(rand() % 10);
        printf("<-S:Carro %i Saindo da ponte de B para A\n", id+CARROS_IDA);
        voltas--;
        pthread_mutex_unlock(&mutex_volta);
        sleep(rand() % 10);
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
    int i;
    pthread_t transito_ida[CARROS_IDA], transito_volta[CARROS_VOLTA];

    srand(time(NULL));
    pthread_mutex_init(&mutex_ida, NULL);
    pthread_mutex_init(&mutex_volta, NULL);

    for (i = 0; i < CARROS_IDA; i++){
        pthread_create(&transito_volta[i], NULL, ponte_ida, (void *)i);
        idas++;
    }
    for (i = 0; i < CARROS_VOLTA; i++){
        pthread_create(&transito_ida[i], NULL, ponte_volta, (void *)i);
        voltas++;
    }

    for (i = 0; i < CARROS_IDA; i++){
        pthread_join(transito_volta[i], NULL);
    }
    for (i = 0; i < CARROS_VOLTA; i++){
        pthread_join(transito_ida[i], NULL);
    }
    printf("foram todos");
}