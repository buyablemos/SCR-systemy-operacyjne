#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <ctype.h>

#define MAX_DL_SLOWA 32
#define MAX_DL_HASH (EVP_MAX_MD_SIZE * 2)
#define MAX_UZYTKOWNIKOW 1000

typedef struct {
    int id;
    char hash[MAX_DL_HASH];
    char email[MAX_DL_SLOWA];
    char nazwa[MAX_DL_SLOWA];
} Uzytkownik;

void oblicz_md5(const char *data,int len, char *md5buf) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	const EVP_MD *md = EVP_md5();
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len, i;
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, data, len);
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_free(mdctx);
	for (i = 0; i < md_len; i++) {
		snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
	}
}

void wczytaj_dane_uzytkownikow(const char *sciezka_pliku, Uzytkownik *uzytkownicy, int *liczba_uzytkownikow) {
    FILE *plik = fopen(sciezka_pliku, "r");
    if (plik == NULL) {
        perror("Błąd otwarcia pliku");
        exit(1);
    }

    *liczba_uzytkownikow = 0;
    while (fscanf(plik, "%d %s %s %[^\n]", &uzytkownicy[*liczba_uzytkownikow].id, uzytkownicy[*liczba_uzytkownikow].hash,
                  uzytkownicy[*liczba_uzytkownikow].email, uzytkownicy[*liczba_uzytkownikow].nazwa) == 4 && *liczba_uzytkownikow < MAX_UZYTKOWNIKOW) {
        (*liczba_uzytkownikow)++;
    }

    fclose(plik);
}

void sprawdz_haslo(const char *slowo, Uzytkownik *uzytkownicy, int liczba_uzytkownikow) {
    char hash[MAX_DL_HASH];
    char hash2[MAX_DL_HASH];
    char hash3[MAX_DL_HASH];

    char slowo_duzelitery[MAX_DL_SLOWA]; //hasz3
    char slowo_duzapierw[MAX_DL_SLOWA]; //hasz2

    
    
    
    // Z duzymi literami
    
    for (int i = 0; i < strlen(slowo); i++) {
        slowo_duzelitery[i] = toupper(slowo[i]);
    }
    slowo_duzelitery[strlen(slowo)] = '\0';
    
    //Pierwsza duza
    
    for (int i = 0; i < strlen(slowo); i++) {
        slowo_duzapierw[i] = tolower(slowo[i]);
    }
    slowo_duzapierw[0] = toupper(slowo_duzapierw[0]);
    slowo_duzapierw[strlen(slowo)] = '\0';

    oblicz_md5(slowo,strlen(slowo), hash);
    oblicz_md5(slowo_duzapierw,strlen(slowo_duzapierw), hash2);
    oblicz_md5(slowo_duzelitery,strlen(slowo_duzelitery), hash3);


    for (int i = 0; i < liczba_uzytkownikow; i++) {
        if (strcmp(hash, uzytkownicy[i].hash) == 0) {
            printf("Znaleziono hasło: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo);
        }
        if (strcmp(hash2, uzytkownicy[i].hash) == 0) {
            printf("Znaleziono hasło: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo_duzapierw);
        }
        if (strcmp(hash3, uzytkownicy[i].hash) == 0) {
            printf("Znaleziono hasło: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo_duzelitery);
        }
    }

    // Sprawdź hasło z prefiksem lub postfiksem liczbowym
    for (int j = 1; j <= 100; j++) {
        char slowo_z_prefiksem[MAX_DL_SLOWA];
        char slowo_z_postfiksem[MAX_DL_SLOWA];

        char slowo2_z_prefiksem[MAX_DL_SLOWA];
        char slowo2_z_postfiksem[MAX_DL_SLOWA];
        
        char slowo3_z_prefiksem[MAX_DL_SLOWA];
        char slowo3_z_postfiksem[MAX_DL_SLOWA];
        
        sprintf(slowo_z_prefiksem, "%d%s", j, slowo);
        sprintf(slowo_z_postfiksem, "%s%d", slowo, j);

        sprintf(slowo2_z_prefiksem, "%d%s", j, slowo_duzapierw);
        sprintf(slowo2_z_postfiksem, "%s%d", slowo_duzapierw, j);

        sprintf(slowo3_z_prefiksem, "%d%s", j, slowo_duzelitery);
        sprintf(slowo3_z_postfiksem, "%s%d", slowo_duzelitery, j);

        oblicz_md5(slowo_z_prefiksem,strlen(slowo_z_prefiksem), hash);
        oblicz_md5(slowo2_z_prefiksem,strlen(slowo2_z_prefiksem), hash2);
        oblicz_md5(slowo3_z_prefiksem,strlen(slowo3_z_prefiksem), hash3);

        for (int i = 0; i < liczba_uzytkownikow; i++) {
            if (strcmp(hash, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z prefiksem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                       uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo_z_prefiksem);
            }
             if (strcmp(hash2, uzytkownicy[i].hash) == 0) {
            printf("Znaleziono hasło z prefiksem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo2_z_prefiksem);
            }
            if (strcmp(hash3, uzytkownicy[i].hash) == 0) {
            printf("Znaleziono hasło z prefiksem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo3_z_prefiksem);
            }
        }

        oblicz_md5(slowo_z_postfiksem,strlen(slowo_z_postfiksem), hash);
        oblicz_md5(slowo2_z_postfiksem,strlen(slowo2_z_postfiksem), hash2);
        oblicz_md5(slowo3_z_postfiksem,strlen(slowo3_z_postfiksem), hash3);

        for (int i = 0; i < liczba_uzytkownikow; i++) {
            if (strcmp(hash, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z postfiksem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                       uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo_z_postfiksem);
            }
             if (strcmp(hash2, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z postfiksem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo2_z_postfiksem);
            }
            if (strcmp(hash3, uzytkownicy[i].hash) == 0) {
            printf("Znaleziono hasło z postfiksem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo3_z_postfiksem);
           }
        }
    }

    // Sprawdź hasło z prefiksem i postfiksem liczbowym
    for (int j = 0; j <= 100; j++) {
        for (int y = 0; y <= 100; y++){
        char slowo_z_pre_post[MAX_DL_SLOWA];
        char slowo2_z_pre_post[MAX_DL_SLOWA];
        char slowo3_z_pre_post[MAX_DL_SLOWA];

        sprintf(slowo_z_pre_post, "%d%s%d", j, slowo, y);
        sprintf(slowo2_z_pre_post, "%d%s%d", j, slowo_duzapierw, y);
        sprintf(slowo3_z_pre_post, "%d%s%d", j, slowo_duzelitery,y);
        oblicz_md5(slowo_z_pre_post,strlen(slowo_z_pre_post), hash);
        oblicz_md5(slowo2_z_pre_post,strlen(slowo2_z_pre_post), hash2);
        oblicz_md5(slowo3_z_pre_post,strlen(slowo3_z_pre_post), hash3);

        for (int i = 0; i < liczba_uzytkownikow; i++) {
            if (strcmp(hash, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z prefiksem i postfixem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                       uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo_z_pre_post);
            }
            if (strcmp(hash2, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z prefiksem i postfixem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                       uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo2_z_pre_post);
            }
            if (strcmp(hash3, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z prefiksem i postfixem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                       uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa, slowo3_z_pre_post);
            }
        }

    }
    }
}


int wczytaj_slownik(const char* sciezka_slowa,char ***slownik,int *rozmiar_slownika){

char slowo[MAX_DL_SLOWA];

FILE *plik = fopen(sciezka_slowa, "r");
    if (plik == NULL) {
        perror("Błąd otwarcia pliku");
        exit(1);
    }

while (fscanf(plik, "%s", slowo) == 1) {
        
         char *noweslowo = (char *)malloc(strlen(slowo) + 1);
        strcpy(noweslowo, slowo);
        *slownik = (char **)realloc(*slownik, ((*rozmiar_slownika) + 1) * sizeof(char *));
        (*slownik)[(*rozmiar_slownika)] = noweslowo;
        (*rozmiar_slownika)++;
    }

}

int main() {
    const char *sciezka_slowa = "duzyslownik.txt"; // sciezka slownika
    const char *sciezka_hasel = "hasla3.txt";   // sciezka z zaszyfrowanymi haslami

    FILE *plik = fopen(sciezka_slowa, "r");
    if (plik == NULL) {
        perror("Błąd otwarcia pliku");
        exit(1);
    }

    
    int liczba_uzytkownikow;
    Uzytkownik uzytkownicy[MAX_UZYTKOWNIKOW];
    char **slownik=NULL;
    int rozmiar_slownika=0;


    wczytaj_dane_uzytkownikow(sciezka_hasel, uzytkownicy, &liczba_uzytkownikow);
    wczytaj_slownik(sciezka_slowa,&slownik,&rozmiar_slownika);
    
    
    for(int i=0;i<rozmiar_slownika;i++){
        sprawdz_haslo(slownik[i], uzytkownicy, liczba_uzytkownikow);
        printf("%d\n",i);
    }
    
    for (int i = 0; i < rozmiar_slownika; i++) {
    free(slownik[i]);
    }
    free(slownik);
    fclose(plik);

    return 0;
}