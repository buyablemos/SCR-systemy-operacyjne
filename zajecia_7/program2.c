#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#define USER_LENGTH 8

#define BABBLE_NAME "kongo"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80

struct babblespace {
    pthread_mutex_t babble_mutex;
    pthread_cond_t babble_cond;
    int babble_first, babble_total;
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

void clear_stdin() {
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}

void display(struct babblespace *babble_ptr) {
    pthread_mutex_lock(&(babble_ptr->babble_mutex));

    printf("W pamieci ");
    printf(BABBLE_NAME);
    printf(" zapisano nastepujace komunikaty:\n");

    int count = 0;
    int cur = babble_ptr->babble_first;

    while (count < babble_ptr->babble_total) {
        int index = cur % BABBLE_LIMIT;
        printf("%d:%s\n", index , babble_ptr->babbles[index]);

        cur++;
        count++;
    }

    pthread_mutex_unlock(&(babble_ptr->babble_mutex));
}

void write_babble(struct babblespace *babble_ptr) {
    char user[USER_LENGTH];
    char message[BABBLE_LENGTH-USER_LENGTH];
    char result[BABBLE_LENGTH];

    clear_stdin();

    pthread_mutex_lock(&(babble_ptr->babble_mutex));

    printf("Podaj swoje inicjały: \n");
    fgets(user, USER_LENGTH, stdin);

    user[strlen(user) - 1] = '\0';  // Usunięcie znaku nowej linii, który jest na końcu

    clear_stdin();

    printf("Podaj komunikat (max %d znakow): \n", BABBLE_LENGTH);
    fgets(message, BABBLE_LENGTH-USER_LENGTH-1 , stdin);
    message[strlen(message) - 1] = '\0';  // Usunięcie znaku nowej linii

    clear_stdin();

    int index = (babble_ptr->babble_first + babble_ptr->babble_total) % BABBLE_LIMIT;

    snprintf(result, BABBLE_LENGTH, "%s:%s", user, message);
    strcpy(babble_ptr->babbles[index], result);

    if (babble_ptr->babble_total < BABBLE_LIMIT) {
        babble_ptr->babble_total++;
    } else {
        babble_ptr->babble_first = (babble_ptr->babble_first + 1) % BABBLE_LIMIT;
    }

    pthread_cond_broadcast(&(babble_ptr->babble_cond));
    pthread_mutex_unlock((&babble_ptr->babble_mutex));
}

void monitor_babbles(struct babblespace *babble_ptr) {
    
     while(1){

    pthread_mutex_lock(&(babble_ptr->babble_mutex));

    pthread_cond_wait(&(babble_ptr->babble_cond), &(babble_ptr->babble_mutex));
        
    printf("W pamieci ");
    printf(BABBLE_NAME);
    printf(" dodano nastepujacy komunikat:\n");

    int cur;
    
      if (babble_ptr->babble_total < BABBLE_LIMIT) {
        cur=babble_ptr->babble_total-1;
    } else {
        cur=babble_ptr->babble_first;
    }



    printf("%d:%s\n", cur , babble_ptr->babbles[cur]);


    pthread_mutex_unlock(&(babble_ptr->babble_mutex));
    
    }
    
}

void sigusr1_handler() {
    exit(0);
}


int main() {

    char exit;

    int shm_fd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
    if (shm_fd == -1) {
        perror("Blad w otwarciu obszaru pamieci wspolnej");
        return 1;
    }

    // mapowanie otrzymanego obszaru pamięci do przestrzeni adresowej procesu z rzutowaniem na wskaźnik do podanej struktury
    struct babblespace *babble_ptr = (struct babblespace *)mmap(NULL, sizeof(struct babblespace), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (babble_ptr == MAP_FAILED) {
        perror("Nie udalo sie stworzyc wskaznika za pomoca nmapa");
        return 1;
    }

    close(shm_fd);

    pid_t pid = fork();

    if (pid == -1) {
        perror("Blad przy funkcji fork");
        return 1;
    }

    if (pid > 0) {
        
        // Proces rodzica
        while (exit != 'x') {
            printf("Wybierz literke i to co chcesz zrobic\nd-display\nw-write\nx-exit\nu-wyczyszcznie mutex\n(Ja jestem rodzicem)\n");
            scanf(" %c", &exit);

            switch (exit) {
                case 'd':
                    display(babble_ptr);
                    break;
                case 'w':
                    write_babble(babble_ptr);
                    break;
                case 'x':
                    break;
                case 'u' :
                pthread_mutex_unlock(&(babble_ptr->babble_mutex));
                break;

                default:
                    printf("Wybierz dobrze!\n");
                    break;
            }
        }
        kill(pid, SIGUSR1);
        wait(0);

    } else {

        signal(SIGUSR1, sigusr1_handler);
     
        // Proces potomka (wątek monitorujący)
        monitor_babbles(babble_ptr);
    }


    return 0;
}