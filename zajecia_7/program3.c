#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BABBLE_NAME "/pamiecABCD"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80

struct babblespace {
    pthread_mutex_t babble_mutex;
    pthread_cond_t babble_cond;
    int babble_first, babble_total;
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

void display_babbles(struct babblespace *ptr) {
    pthread_mutex_lock(&(ptr->babble_mutex));
    printf("Wypisywanie babbli:\n");
    for (int i = 0; i < ptr->babble_total; i++) {
        int index = (ptr->babble_first + i) % BABBLE_LIMIT;
        printf("[%d] %s\n", i + 1, ptr->babbles[index]);
    }
    pthread_mutex_unlock(&(ptr->babble_mutex));
}
                                                                     
void add_babble(struct babblespace *ptr, const char *input) {
    pthread_mutex_lock(&(ptr->babble_mutex));

    snprintf(ptr->babbles[(ptr->babble_first + ptr->babble_total) % BABBLE_LIMIT], BABBLE_LENGTH, "%s", input);

    if (ptr->babble_total < BABBLE_LIMIT) {
       ptr->babble_total++;
    }
    else {
      ptr->babble_first = (ptr->babble_first + 1) % BABBLE_LIMIT;
    }
    pthread_cond_broadcast(&(ptr->babble_cond));
    pthread_mutex_unlock(&(ptr->babble_mutex));
}

void write_babble(struct babblespace *ptr){
      char input[BABBLE_LENGTH];
      printf("Podaj komunikat (do %d znaków): ", BABBLE_LENGTH - 1);
      fgets(input, sizeof(input), stdin);
      input[strcspn(input, "\n")] = '\0';
      add_babble(ptr, input);
}


void print_menu(){
        printf("\n****MENU****\n");
        printf("d-display\n");
        printf("w-write\n");
        printf("q-quit\n");
}

void child_process(struct babblespace *ptr) {
    while (1) {
        pthread_mutex_lock(&(ptr->babble_mutex));
        pthread_cond_wait(&(ptr->babble_cond), &(ptr->babble_mutex));
        display_babbles(ptr);
        pthread_mutex_unlock(&(ptr->babble_mutex));
    }
}


int main() {
    int shm_fd;
    struct babblespace *ptr;
    shm_fd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
    if (shm_fd == -1) {
        perror("Blad otwierania-shm_open");
        exit(EXIT_FAILURE);
    }
    
    ptr = mmap(NULL, sizeof(struct babblespace), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Blad mapowania-mmap");
        exit(EXIT_FAILURE);
    }


    pid_t pid = fork();

    if (pid == -1) {
        perror("Blad fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        child_process(ptr);
        exit(EXIT_SUCCESS);
    }
    else{
    char option;
    while(1){
        print_menu();
        scanf("%c",&option);
        getchar();
        switch (option){
    case 'd':
      display_babbles(ptr);
      break;
    case 'w':
      write_babble(ptr);
      break;
    case 'q':
      kill(pid, SIGTERM);
      waitpid(pid, NULL, 0);

      if (munmap(ptr, sizeof(struct babblespace)) == -1) {
           perror("Blad rozmapowania-munmap");}
      if (close(shm_fd) == -1) {
        perror("Blad zamykania-close");}
      return 0;
    default:
      printf("Bledna opcja MENU\n");
      break;
    }
    }
}
}
