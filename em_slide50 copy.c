#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NESCRITORES 1
#define NLEITORES 3
pthread_mutex_t mutex;
pthread_mutex_t database;
int nleitores = 0;

void *esc(void *args)
{
    int id = (int)args;
    while (1)
    {
        pthread_mutex_lock(&database);
        printf("Escritor %i Acessando a base\n", id);
        sleep(rand() % 10);
        printf("Escritor %i Saindo da base\n", id);
        pthread_mutex_unlock(&database);
        sleep(rand() % 10);
    }
}
void *leit(void *args)
{
    int id = (int)args;
    while (1)
    {
        pthread_mutex_lock(&mutex); //bloqueia para nao ter outros leitores acessando a mesma variavel de contagem de leitores
        nleitores++; //incrementa o numero de leitores
        if (nleitores == 1) //se for o primeiro leitor, bloqueio o banco para os escritores
            pthread_mutex_lock(&database);
        pthread_mutex_unlock(&mutex); //desbloqueia para que outros leitores possam usar o processador
        printf("Leitor %i Acessando a base\n", id);
        sleep(rand() % 10);
        printf("Leitor %i Saindo da base\n", id);
        pthread_mutex_lock(&mutex); //bloqueia para outros leitores nao alterarem a variavel nleitores
        nleitores--; //retira esse leitor da lista
        if (nleitores == 0) //se nao houverem mais leitores, libera o banco para os escritores
            pthread_mutex_unlock(&database);
        pthread_mutex_unlock(&mutex); //libera para outros leitores usarem a CPU
        sleep(rand() % 10);
    }
}
int main()
{
    int i;
    pthread_t leitores[NLEITORES], escritores[NLEITORES];

    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&database, NULL);

    for (i = 0; i < NESCRITORES; i++)
        pthread_create(&escritores[i], NULL, esc, (void *)i);
    for (i = 0; i < NLEITORES; i++)
        pthread_create(&leitores[i], NULL, leit, (void *)i);

    for (i = 0; i < NESCRITORES; i++)
        pthread_join(escritores[i], NULL);
    for (i = 0; i < NLEITORES; i++)
        pthread_join(leitores[i], NULL);
}