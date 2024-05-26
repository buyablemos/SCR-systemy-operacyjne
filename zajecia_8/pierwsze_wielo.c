#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct data{
int N;          /* liczba liczb pierwszych do znalezienia (arg.wywolania) */
int sprawdz; /* nastepna liczba do sprawdzania */
int znalezione; /* liczba znalezionych*/
long suma; /* to bedzie obliczona suma liczb pierwszych */

pthread_mutex_t muteks_pobierania;
pthread_mutex_t muteks_sumowania;

};

int stop=1;


int check(int number) {
    if (number <= 1) {
        return 0;
    }

    for (int i = 2; i * i <= number; i++) {
        if (number % i == 0) {
            return 0; 
        }
    }

    return 1;
}

void *calculate(void *arg) {
    struct data *dane= (struct data *) arg;
  
    while (stop) {
        int current_number;
    
        if((dane->znalezione < dane->N)){
        pthread_mutex_lock(&dane->muteks_pobierania);
        current_number = dane->sprawdz++;
        pthread_mutex_unlock(&dane->muteks_pobierania);
        }

        if (check(current_number)) {
            pthread_mutex_lock(&dane->muteks_sumowania);

            dane->suma += current_number;
            dane->znalezione++;
            
            pthread_mutex_unlock(&dane->muteks_sumowania);
        }

        if(dane->znalezione >= dane->N){break;}
    }


    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {


    

    struct data *dane=malloc(sizeof(struct data));
    dane->sprawdz = 2; /* nastepna liczba do sprawdzania */
    dane->znalezione = 0; /* liczba znalezionych*/
    dane->suma = 0;
    pthread_mutex_init(&dane->muteks_pobierania,NULL);
    pthread_mutex_init(&dane->muteks_sumowania,NULL);

    dane->N = atoi(argv[1]);
    int K = atoi(argv[2]);

    pthread_t threads[K];

    for (int i = 0; i < K; i++) {
        int error = pthread_create(&threads[i], NULL, calculate, dane);
        if (error) {
            printf("Blad w pthread_create%d\n", error);
            return 1;
        }
    }

    for (int i = 0; i < K; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Suma liczb pierwszych to: %ld\n", dane->suma);
    
    printf("Suma znalezionych liczb pierwszych to: %d\n",dane->znalezione);
    free(dane);
    return 0;
}
