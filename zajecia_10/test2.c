#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#define MAX_DL_SLOWA 32
#define MAX_DL_HASH (EVP_MAX_MD_SIZE * 2)
#define MAX_UZYTKOWNIKOW 1000

typedef struct {
    int id;
    char hash[MAX_DL_HASH];
    char email[MAX_DL_SLOWA];
    char nazwa_uzytkownika[MAX_DL_SLOWA];
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
                  uzytkownicy[*liczba_uzytkownikow].email, uzytkownicy[*liczba_uzytkownikow].nazwa_uzytkownika) == 4 && *liczba_uzytkownikow < MAX_UZYTKOWNIKOW) {
        (*liczba_uzytkownikow)++;
    }

    fclose(plik);
}

void sprawdz_haslo(const char *slowo, Uzytkownik *uzytkownicy, int liczba_uzytkownikow) {
    char hash[MAX_DL_HASH];

    // Sprawdź hasło
    oblicz_md5(slowo,strlen(slowo), hash);

    for (int i = 0; i < liczba_uzytkownikow; i++) {
        if (strcmp(hash, uzytkownicy[i].hash) == 0) {
            printf("Znaleziono hasło: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa_uzytkownika, slowo);
        }
    }

    // Sprawdź hasło z prefiksem lub postfiksem liczbowym
    for (int j = 0; j <= 100; j++) {
        char slowo_z_prefiksem[MAX_DL_SLOWA];
        char slowo_z_postfiksem[MAX_DL_SLOWA];
        
        sprintf(slowo_z_prefiksem, "%d%s", j, slowo);
        sprintf(slowo_z_postfiksem, "%s%d", slowo, j);

        oblicz_md5(slowo_z_prefiksem,strlen(slowo_z_prefiksem), hash);

        for (int i = 0; i < liczba_uzytkownikow; i++) {
            if (strcmp(hash, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z prefiksem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                       uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa_uzytkownika, slowo_z_prefiksem);
            }
        }

        oblicz_md5(slowo_z_postfiksem,strlen(slowo_z_postfiksem), hash);

        for (int i = 0; i < liczba_uzytkownikow; i++) {
            if (strcmp(hash, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z postfiksem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                       uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa_uzytkownika, slowo_z_postfiksem);
            }
        }
    }

    // Sprawdź hasło z prefiksem i postfiksem liczbowym
    for (int j = 0; j <= 100; j++) {
        for (int y = 0; y <= 100; y++){
        char slowo_z_pre_post[MAX_DL_SLOWA];
        
        sprintf(slowo_z_pre_post, "%d%s%d", j, slowo, y);
    

        oblicz_md5(slowo_z_pre_post,strlen(slowo_z_pre_post), hash);

        for (int i = 0; i < liczba_uzytkownikow; i++) {
            if (strcmp(hash, uzytkownicy[i].hash) == 0) {
                printf("Znaleziono hasło z prefiksem i postfixem: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                       uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa_uzytkownika, slowo_z_pre_post);
            }
        }

    }
    }
}

int main() {
    const char *sciezka_slowa = "slownik.txt"; // sciezka slownika
    const char *sciezka_hasel = "hasla2.txt";   // sciezka z zaszyfrowanymi haslami

    FILE *plik = fopen(sciezka_slowa, "r");
    if (plik == NULL) {
        perror("Błąd otwarcia pliku");
        exit(1);
    }

    char slowo[MAX_DL_SLOWA];
    int liczba_uzytkownikow;
    Uzytkownik uzytkownicy[MAX_UZYTKOWNIKOW];

    wczytaj_dane_uzytkownikow(sciezka_hasel, uzytkownicy, &liczba_uzytkownikow);

    while (fscanf(plik, "%s", slowo) == 1) {
        sprawdz_haslo(slowo, uzytkownicy, liczba_uzytkownikow);
    }

    fclose(plik);

    return 0;
}
