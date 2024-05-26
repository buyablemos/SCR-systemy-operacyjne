#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include <stdbool.h>

int N;
int X;
int Y;
int S;
int ostatni;



pthread_mutex_t mutex_zew;
pthread_mutex_t mutex_wew;
pthread_cond_t dzialka_cond;
pthread_cond_t dzialka_cond2;

int **dzialkaglobal;


void *syn(void *arg){

int id=*(int*)arg;
int losX;
int losY;


srand(time(NULL)+id*21);

for(int i=0;i<S;i++){

pthread_mutex_lock(&mutex_wew);

losX=rand()%X;
losY=rand()%Y;

if(dzialkaglobal[losX][losY]==0){

dzialkaglobal[losX][losY]=id;

}
pthread_mutex_unlock(&mutex_wew);


}

pthread_mutex_lock(&mutex_zew);
pthread_mutex_lock(&mutex_wew);


ostatni=id;
printf("Syn: %d zakończył podboje\n",id);

pthread_cond_signal(&dzialka_cond);  //po zakonczeniu wyslij sygnal
pthread_cond_wait(&dzialka_cond2, &mutex_wew);

pthread_mutex_unlock(&mutex_wew);
pthread_mutex_unlock(&mutex_zew);

pthread_exit(NULL);
}

void *rejent(void *arg){

int suma_wolnych;    

int licznik_pol=0;

int synowie_probujacy = N;

while(synowie_probujacy>0){
pthread_mutex_lock(&mutex_wew);
pthread_cond_wait(&dzialka_cond, &mutex_wew);
  

   for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            if (dzialkaglobal[i][j]==ostatni)
            {
                printf("Syn: %d zajal ostatecznie miejsca X: %d,Y: %d\n",dzialkaglobal[i][j],i,j);
                licznik_pol++;
            }
            
        }
    }


printf("Syn: %d zajal ostatecznie %d pola\n",ostatni,licznik_pol);
synowie_probujacy--;
licznik_pol=0;
pthread_cond_signal(&dzialka_cond2);
pthread_mutex_unlock(&mutex_wew);

}

//po zakonczeniu walki synow

pthread_mutex_lock(&mutex_zew);



   for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            if (dzialkaglobal[i][j]==0)
            {
                printf("Wolne zostaly ostatecznie miejsca X: %d,Y: %d\n",i,j);
                suma_wolnych++;
            }
            
        }
    }
    printf("Wolnych zostalo ostatecznie %d\n",suma_wolnych);

pthread_mutex_unlock(&mutex_zew);

pthread_exit(NULL);

}

int main(int argc, char *argv[])
{
 //watek nadzorujacy walke
  

  int error; 

  N=atoi(argv[1]);
  X=atoi(argv[2]);
  Y=atoi(argv[3]);
  S=atoi(argv[4]);
  pthread_t threads[N+1];
  int ids[N];

  dzialkaglobal=(int **)malloc(X* sizeof(int *));

    for (int i = 0; i < X; i++) {
        dzialkaglobal[i] = (int *)malloc(Y * sizeof(int));
    }


    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            dzialkaglobal[i][j]=0;
        }
    }
     

  /* inicjalizacja zmiennej i mutexa */
  pthread_mutex_init(&mutex_zew, NULL);
  pthread_cond_init (&dzialka_cond, NULL);
  pthread_cond_init (&dzialka_cond2, NULL);
  pthread_mutex_init(&mutex_wew, NULL);
 

  error=pthread_create(&threads[N],NULL, rejent, NULL);
    if (error) {
            printf("Blad w pthread_create%d\n", error);
            exit(-1);
        }
 
  
  for (int i = 0; i < N; i++) {
   ids[i]=i+1;
   error=pthread_create(&threads[i],NULL, syn, (void *)&ids[i]);
    if (error) {
            printf("Blad w pthread_create%d\n", error);
            exit(-1);
        }
  }


  /* czekanie na zakonczenie */
  for (int i = 0; i < N+1; i++) {
    pthread_join(threads[i], NULL);
  }
  printf ("Main(): Dzialanie zakonczyły %d watki\n", N+1);

  /* posprzatanie */
  free(dzialkaglobal);
  pthread_mutex_destroy(&mutex_zew);
  pthread_mutex_destroy(&mutex_wew);
  pthread_cond_destroy(&dzialka_cond);
  pthread_exit (NULL);

}
