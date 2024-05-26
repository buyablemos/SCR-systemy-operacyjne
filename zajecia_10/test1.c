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

    oblicz_md5(slowo,strlen(slowo), hash);

    for (int i = 0; i < liczba_uzytkownikow; i++) {
        if (strcmp(hash, uzytkownicy[i].hash) == 0) {
            printf("Znaleziono hasło: ID=%d, Email=%s, Nazwa użytkownika=%s, Hasło=%s\n",
                   uzytkownicy[i].id, uzytkownicy[i].email, uzytkownicy[i].nazwa_uzytkownika, slowo);
        }
    }
}

int main() {
    const char *sciezka_slowa = "slownik.txt"; // Zmień na właściwą nazwę pliku ze słownikiem
    const char *sciezka_hasel = "hasla.txt";   // Zmień na właściwą nazwę pliku z zahaszowanymi hasłami

    FILE *plik = fopen(sciezka_slowa, "r");
    if (plik == NULL) {
        perror("Błąd otwarcia pliku");
        exit(1);
    }

    char slowo[MAX_DL_SLOWA];
    char hash[MAX_DL_HASH];
    int liczba_uzytkownikow;
    Uzytkownik uzytkownicy[MAX_UZYTKOWNIKOW];

    wczytaj_dane_uzytkownikow(sciezka_hasel, uzytkownicy, &liczba_uzytkownikow);

    while (fscanf(plik, "%s", slowo) == 1) {
        sprawdz_haslo(slowo,uzytkownicy,liczba_uzytkownikow);
    }

    fclose(plik);

    return 0;
}
