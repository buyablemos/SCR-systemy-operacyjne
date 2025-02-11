/******************************************************************************
* FILE: bug2.c
* DESCRIPTION:
*   A "hello world" Pthreads program that dumps core.  Figure out why and
*   then fix it - or else see the solution bug2fix.c.
* AUTHOR: 9/98 Blaise Barney
* LAST REVISED: 01/29/09
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NTHREADS	8
#define ARRAY_SIZE      2000000


void *Hello(void *threadid)
{
   double A[ARRAY_SIZE];
   int i; 
   long tid;
 

   tid = (long)threadid;
   sleep(3);
   for (i=0; i<ARRAY_SIZE; i++)
   {
      A[i] = i * 1.0;
   }
   printf("%ld: Hello World!   %f\n", tid, A[ARRAY_SIZE-1]);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
pthread_attr_t attr;
pthread_t threads[NTHREADS];
size_t stacksize;
int rc; 
long t;

pthread_attr_init(&attr);
size_t newsize=ARRAY_SIZE*sizeof(double)+1000000;
pthread_attr_setstacksize(&attr, newsize);
pthread_attr_getstacksize (&attr, &newsize);
printf("Thread stack size = %li bytes (hint, hint)\n",newsize);
for(t=0;t<NTHREADS;t++){
  rc = pthread_create(&threads[t], &attr, Hello, (void *)t);
  if (rc){
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
    }
  }
printf("Created %ld threads.\n", t);
pthread_exit(NULL);
}

/*
Problem polegał na tym, że domyślny rozmiar stosu wątku 
był zbyt duży jak na probe zaalokowania tej wielkiej tablicy ARRAY_SIZE
gdy zmienimy wartosc domyslna dla kazdego watku na wieksza niz rozmiar
tej tablicy doubli to program 
normalnie dziala i sie wykonuje to coma dziac
*/