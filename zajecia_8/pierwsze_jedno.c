#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char *argv[]) {
   
    int N = atoi(argv[1]);

    int znalezione = 0;
    long suma = 0;
    int number = 2; // Zaczynau od dwojki

while (znalezione < N) {
    if (check(number)) {
    suma += number;
    znalezione++;
    }
    number++;
}

    printf("Suma %d liczb pierwszych to: %ld\n", N, suma);

    return 0;
}
