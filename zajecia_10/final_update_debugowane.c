#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <openssl/sha.h>

#define MAX_DL_SLOWA 32
#define MAX_DL_HASH 65
#define MAX_UZYTKOWNIKOW 1000
#define LICZBA_WATKOW 4
char salt[7] = "scr2324";

typedef struct lista
{
    char *haslo;
    struct lista *next;
} lista;

typedef struct
{
    char id[MAX_DL_SLOWA/4];
    char hash[MAX_DL_HASH];
    bool zlamane;
} Uzytkownik;

typedef struct
{
    char **slownik;
    int rozmiar_slownika;
    Uzytkownik *uzytkownicy;
    int liczba_uzytkownikow;
    pthread_mutex_t mutex;
    pthread_mutex_t mutex_zew;
    pthread_mutex_t mutex1;
    pthread_mutex_t mutex2;
    pthread_mutex_t mutex3;
    pthread_mutex_t mutex4;
    pthread_cond_t cond;
    int haslo_zlamane;         // id zlamanego hasla ostatnio
    char *haslo_zlamane_value; // ostatnie zlamane haslo
    int wyswietlanie;
    struct lista *glowa;
    int koniec;
    bool blokada;
    int i4;
    int i3;
    int i2;
    int i1;
} ParametryWatkow;

ParametryWatkow parametry;

struct lista *dodaj(struct lista *head, const char *haslo_przekazane)
{
    struct lista *nowywezel = (struct lista *)malloc(sizeof(struct lista));

    nowywezel->haslo = strdup(haslo_przekazane);
    nowywezel->next = NULL;

    if (head == NULL)
    {
        // Lista jest pusta
        head = nowywezel;
    }
    else
    {
        // Lista nie jest pusta
        struct lista *current = head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = nowywezel;
    }

    return head;
}

void wypisz(struct lista *head)
{
    struct lista *current = head;
    printf("\n");
    while (current != NULL)
    {
        printf("Hasło złamane: %s\n", current->haslo);
        current = current->next;
    }
    printf("\n");
}

void freelista(struct lista *head)
{
    struct lista *current = head;
    while (current != NULL)
    {
        struct lista *next = current->next;
        free(current->haslo);
        free(current);
        current = next;
    }
}

void oblicz_md5(const char *data, int len, char *md5buf)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data, len);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);
    for (i = 0; i < md_len; i++)
    {
        snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
    }
}



void calculate_sha256(const char *password,unsigned char *hash)
{   
    char password_with_salt[MAX_DL_HASH+7];
    unsigned char hash_help[MAX_DL_HASH];
    sprintf(password_with_salt, "%s%s", password, salt);
    SHA256((unsigned char *)password_with_salt, strlen(password_with_salt), hash_help);
    
    for (int i = 0; i < 32; i++)
    {
        snprintf(&(hash[i * 2]), 16*2, "%02x", hash_help[i]);
    }
}

void wczytaj_dane_uzytkownikow(const char *sciezka_pliku, Uzytkownik *uzytkownicy, int *liczba_uzytkownikow)
{
    FILE *plik = fopen(sciezka_pliku, "r");
    if (plik == NULL)
    {
        perror("Błąd otwarcia pliku");
        exit(1);
    }

    *liczba_uzytkownikow = 0;
    while (fscanf(plik, "%s %[^\n]", uzytkownicy[*liczba_uzytkownikow].id, uzytkownicy[*liczba_uzytkownikow].hash) == 2 && *liczba_uzytkownikow < MAX_UZYTKOWNIKOW)
    {   
        uzytkownicy[*liczba_uzytkownikow].zlamane = false;
        (*liczba_uzytkownikow)++;
    }

    fclose(plik);
}

void sprawdz_haslo(const char *slowo, Uzytkownik *uzytkownicy, int liczba_uzytkownikow, ParametryWatkow *parametry, int id)
{
    char hash[MAX_DL_HASH];
    char hash2[MAX_DL_HASH];
    char hash3[MAX_DL_HASH];

    char slowo_duzelitery[MAX_DL_SLOWA]; // hasz3
    char slowo_duzapierw[MAX_DL_SLOWA];  // hasz2

    // Z duzymi literami
    for (int i = 0; i < strlen(slowo); i++)
    {
        slowo_duzelitery[i] = toupper(slowo[i]);
    }
    slowo_duzelitery[strlen(slowo)] = '\0';

    // Pierwsza duza
    for (int i = 0; i < strlen(slowo); i++)
    {
        slowo_duzapierw[i] = tolower(slowo[i]);
    }
    slowo_duzapierw[0] = toupper(slowo_duzapierw[0]);
    slowo_duzapierw[strlen(slowo)] = '\0';

    if (id % 4 == 1)
    {           parametry->i1++;
                pthread_mutex_unlock(&parametry->mutex1);
        calculate_sha256(slowo, hash);
        calculate_sha256(slowo_duzapierw,  hash2);
        calculate_sha256(slowo_duzelitery,  hash3);

        for (int i = 0; i < liczba_uzytkownikow; i++)
        {   
            
            if (uzytkownicy[i].zlamane == false){

                if (strcmp(hash, uzytkownicy[i].hash) == 0)

                {   pthread_mutex_lock(&parametry->mutex_zew);
                    pthread_mutex_lock(&parametry->mutex);
                    printf("Watek:%d Znaleziono hasło: ID=%s, Hasło=%s\n", id,
                           uzytkownicy[i].id, slowo);
                    parametry->haslo_zlamane = i;
                    parametry->haslo_zlamane_value = strdup(slowo);
                    parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                    pthread_cond_signal(&parametry->cond);
                    parametry->blokada = true;
                    pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }continue;
                }
                if (strcmp(hash2, uzytkownicy[i].hash) == 0)
                {   pthread_mutex_lock(&parametry->mutex_zew);
                    pthread_mutex_lock(&parametry->mutex);
                    printf("Watek:%d Znaleziono hasło: ID=%s,Hasło=%s\n", id,
                           uzytkownicy[i].id, slowo_duzapierw);
                    parametry->haslo_zlamane = i;
                    parametry->haslo_zlamane_value = strdup(slowo_duzapierw);
                    parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                    pthread_cond_signal(&parametry->cond);
                    parametry->blokada = true;
                
                pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }continue;
                }
                if (strcmp(hash3, uzytkownicy[i].hash) == 0)
                {   pthread_mutex_lock(&parametry->mutex_zew);
                    pthread_mutex_lock(&parametry->mutex);

                    printf("Watek:%d Znaleziono hasło: ID=%s, Hasło=%s\n", id,
                           uzytkownicy[i].id, slowo_duzelitery);
                    parametry->haslo_zlamane = i;
                    parametry->haslo_zlamane_value = strdup(slowo_duzelitery);
                    parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                    pthread_cond_signal(&parametry->cond);
                    parametry->blokada = true;
                    pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }
                }
            }
        }
           
    }
    

    if (id % 4 == 2)
    {           parametry->i2++;
                pthread_mutex_unlock(&parametry->mutex2);
        // Sprawdź hasło z prefiksem lub postfiksem liczbowym
        for (int j = 1; j < 100; j++)
        {
            char slowo_z_prefiksem[MAX_DL_SLOWA + 11];
            char slowo2_z_prefiksem[MAX_DL_SLOWA + 11];
            char slowo3_z_prefiksem[MAX_DL_SLOWA + 11];

            sprintf(slowo_z_prefiksem, "%d%s", j, slowo);
            sprintf(slowo2_z_prefiksem, "%d%s", j, slowo_duzapierw);
            sprintf(slowo3_z_prefiksem, "%d%s", j, slowo_duzelitery);

            calculate_sha256(slowo_z_prefiksem,  hash);
            calculate_sha256(slowo2_z_prefiksem,  hash2);
            calculate_sha256(slowo3_z_prefiksem,  hash3);

            for (int i = 0; i < liczba_uzytkownikow; i++)
            {
                if (uzytkownicy[i].zlamane == false)
                {
                
              
                    if (strcmp(hash, uzytkownicy[i].hash) == 0)
                    {   
                        pthread_mutex_lock(&parametry->mutex_zew);
                        pthread_mutex_lock(&parametry->mutex);

                        printf("Watek:%d Znaleziono hasło z prefiksem: ID=%s,Hasło=%s\n", id,
                               uzytkownicy[i].id,slowo_z_prefiksem);
                        parametry->haslo_zlamane = i;
                        parametry->haslo_zlamane_value = strdup(slowo_z_prefiksem);
                        parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                        pthread_cond_signal(&parametry->cond);
                        parametry->blokada = true;
                        pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }continue;
                    }
                    if (strcmp(hash2, uzytkownicy[i].hash) == 0)
                    {   pthread_mutex_lock(&parametry->mutex_zew);
                        pthread_mutex_lock(&parametry->mutex);
                        printf("Watek:%d Znaleziono hasło z prefiksem: ID=%s, Hasło=%s\n", id,
                               uzytkownicy[i].id, slowo2_z_prefiksem);
                        parametry->haslo_zlamane = i;
                        parametry->haslo_zlamane_value = strdup(slowo2_z_prefiksem);
                        parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                        pthread_cond_signal(&parametry->cond);
                        parametry->blokada = true;
                        pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }continue;
                    }
                    if (strcmp(hash3, uzytkownicy[i].hash) == 0)
                    {   pthread_mutex_lock(&parametry->mutex_zew);
                        pthread_mutex_lock(&parametry->mutex);
                        printf("Watek:%d Znaleziono hasło z prefiksem: ID=%s,Hasło=%s\n", id,
                               uzytkownicy[i].id, slowo3_z_prefiksem);
                        parametry->haslo_zlamane = i;
                        parametry->haslo_zlamane_value = strdup(slowo3_z_prefiksem);
                        parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                        pthread_cond_signal(&parametry->cond);
                        parametry->blokada = true;
                
                pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }
                    }
                }
                
            }
        }
    }
    if (id % 4 == 3)
    {           parametry->i3++;
                pthread_mutex_unlock(&parametry->mutex3);
        for (int j = 1; j < 100; j++)
        {
            char slowo_z_postfiksem[MAX_DL_SLOWA + 11];
            char slowo2_z_postfiksem[MAX_DL_SLOWA + 11];
            char slowo3_z_postfiksem[MAX_DL_SLOWA + 11];

            sprintf(slowo_z_postfiksem, "%s%d", slowo, j);
            sprintf(slowo2_z_postfiksem, "%s%d", slowo_duzapierw, j);
            sprintf(slowo3_z_postfiksem, "%s%d", slowo_duzelitery, j);

            calculate_sha256(slowo_z_postfiksem,  hash);
            calculate_sha256(slowo2_z_postfiksem,  hash2);
            calculate_sha256(slowo3_z_postfiksem,  hash3);

            for (int i = 0; i < liczba_uzytkownikow; i++)
            {
                if (uzytkownicy[i].zlamane == false)
                {
               
                    if (strcmp(hash, uzytkownicy[i].hash) == 0)
                    {   pthread_mutex_lock(&parametry->mutex_zew);
                        pthread_mutex_lock(&parametry->mutex);
                        
                        printf("Watek:%d Znaleziono hasło z postfiksem: ID=%s, Hasło=%s\n", id,
                               uzytkownicy[i].id, slowo_z_postfiksem);
                        parametry->haslo_zlamane = i;
                        parametry->haslo_zlamane_value = strdup(slowo_z_postfiksem);
                        parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                        pthread_cond_signal(&parametry->cond);
                        parametry->blokada = true;
                
                pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }continue;
                    }
                    if (strcmp(hash2, uzytkownicy[i].hash) == 0)
                    {   pthread_mutex_lock(&parametry->mutex_zew);
                         pthread_mutex_lock(&parametry->mutex);
                         

                        printf("Watek:%d Znaleziono hasło z postfiksem: ID=%s, Hasło=%s\n", id,
                               uzytkownicy[i].id, slowo2_z_postfiksem);
                        parametry->haslo_zlamane = i;
                        parametry->haslo_zlamane_value = strdup(slowo2_z_postfiksem);
                        parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                        pthread_cond_signal(&parametry->cond);
                        parametry->blokada = true;
                
                pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }continue;
                    }
                    if (strcmp(hash3, uzytkownicy[i].hash) == 0)
                    {   
                        pthread_mutex_lock(&parametry->mutex_zew);
                        pthread_mutex_lock(&parametry->mutex);
                       
                        printf("Watek:%d Znaleziono hasło z postfiksem: ID=%s, Hasło=%s\n", id,
                               uzytkownicy[i].id, slowo3_z_postfiksem);
                        parametry->haslo_zlamane = i;
                        parametry->haslo_zlamane_value = strdup(slowo3_z_postfiksem);
                        parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                        pthread_cond_signal(&parametry->cond);
                        parametry->blokada = true;
                
                pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }
                    }
                }
            }
        }
        

    }

    if (id % 4 == 0)
    {   
        parametry->i4++;
        pthread_mutex_unlock(&parametry->mutex4);
        // Sprawdź hasło z prefiksem i postfiksem liczbowym
        for (int j = 0; j < 100; j++)
        {
            for (int y = 0; y < 100; y++)
            {
                char slowo_z_pre_post[MAX_DL_SLOWA + 22];
                char slowo2_z_pre_post[MAX_DL_SLOWA + 22];
                char slowo3_z_pre_post[MAX_DL_SLOWA + 22];

                sprintf(slowo_z_pre_post, "%d%s%d", j, slowo, y);
                sprintf(slowo2_z_pre_post, "%d%s%d", j, slowo_duzapierw, y);
                sprintf(slowo3_z_pre_post, "%d%s%d", j, slowo_duzelitery, y);
                calculate_sha256(slowo_z_pre_post,  hash);
                calculate_sha256(slowo2_z_pre_post,  hash2);
                calculate_sha256(slowo3_z_pre_post,  hash3);

                for (int i = 0; i < liczba_uzytkownikow; i++)
                {
                        if (uzytkownicy[i].zlamane == false){
                        if (strcmp(hash, uzytkownicy[i].hash) == 0)
                        {   pthread_mutex_lock(&parametry->mutex_zew);
                            pthread_mutex_lock(&parametry->mutex);
                            

                            
                            
                    
                            printf("Watek:%d Znaleziono hasło z prefiksem i postfixem: ID=%s, Hasło=%s\n", id,
                                   uzytkownicy[i].id, slowo_z_pre_post);
                            parametry->haslo_zlamane = i;
                            parametry->haslo_zlamane_value = strdup(slowo_z_pre_post);
                            parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                            pthread_cond_signal(&parametry->cond);
                            parametry->blokada = true;
                            
                            pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }       continue;
                        }
                        if (strcmp(hash2, uzytkownicy[i].hash) == 0)
                        {   pthread_mutex_lock(&parametry->mutex_zew);
                            pthread_mutex_lock(&parametry->mutex);
                            
                            printf("Watek:%d Znaleziono hasło z prefiksem i postfixem: ID=%s, Hasło=%s\n", id,
                                   uzytkownicy[i].id, slowo2_z_pre_post);
                            parametry->haslo_zlamane = i;
                            parametry->haslo_zlamane_value = strdup(slowo2_z_pre_post);
                            parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                            pthread_cond_signal(&parametry->cond);
                            parametry->blokada = true;
                             
                            pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }       continue;
                        }
                        if (strcmp(hash3, uzytkownicy[i].hash) == 0)

                        {   pthread_mutex_lock(&parametry->mutex_zew);
                            pthread_mutex_lock(&parametry->mutex);
                               
                            printf("Watek:%d Znaleziono hasło z prefiksem i postfixem: ID=%s, Hasło=%s\n", id,
                                   uzytkownicy[i].id, slowo3_z_pre_post);
                            parametry->haslo_zlamane = i;
                            parametry->haslo_zlamane_value = strdup(slowo3_z_pre_post);
                            parametry->glowa = dodaj(parametry->glowa, parametry->haslo_zlamane_value);
                            pthread_cond_signal(&parametry->cond);
                            parametry->blokada = true;
                            
                            pthread_mutex_unlock(&parametry->mutex);
                if (!parametry->blokada)
                {
                    pthread_mutex_unlock(&parametry->mutex_zew);
                }continue;;
                        }

                    
                }
                }
            }
        }
    }
}

int wczytaj_slownik(const char *sciezka_slowa, char ***slownik, int *rozmiar_slownika)
{

    char slowo[MAX_DL_SLOWA];

    FILE *plik = fopen(sciezka_slowa, "r");
    if (plik == NULL)
    {
        perror("Błąd otwarcia pliku");
        exit(1);
    }

    while (fscanf(plik, "%s", slowo) == 1)
    {

        char *noweslowo = (char *)malloc(strlen(slowo) + 1);
        strcpy(noweslowo, slowo);
        *slownik = (char **)realloc(*slownik, ((*rozmiar_slownika) + 1) * sizeof(char *));
        (*slownik)[(*rozmiar_slownika)] = noweslowo;
        (*rozmiar_slownika)++;
    }

    fclose(plik);
    return 0;
}

void *producent_hasel(void *arg)
{
    int id_watku = *(int *)arg;

    if (id_watku % 4 == 0)
    {
        while (1)
        {
            if (parametry.koniec == 1)
            {
                printf("Zakończenie pracy watku ID: %d\n", id_watku);
                pthread_exit(NULL);
            }
            else
            {
                pthread_mutex_lock(&parametry.mutex4);
                if (parametry.i4 >= parametry.rozmiar_slownika)
                {
                    pthread_mutex_unlock(&parametry.mutex4);
                    break;
                }
                
                sprawdz_haslo(parametry.slownik[parametry.i4], parametry.uzytkownicy, parametry.liczba_uzytkownikow, &parametry, id_watku);
                
            }
        }
    }
    if (id_watku % 4 == 3)
    {
        while (1)
        {
            if (parametry.koniec == 1)
            {
                printf("Zakończenie pracy watku ID: %d\n", id_watku);
                pthread_exit(NULL);
            }
            else
            {
                pthread_mutex_lock(&parametry.mutex3);
                if (parametry.i3 >= parametry.rozmiar_slownika)
                {
                    pthread_mutex_unlock(&parametry.mutex3);
                    break;
                }
                sprawdz_haslo(parametry.slownik[parametry.i3], parametry.uzytkownicy, parametry.liczba_uzytkownikow, &parametry, id_watku);
                
            }
        }
    }
    if (id_watku % 4 == 2)
    {
        while (1)
        {
            if (parametry.koniec == 1)
            {
                printf("Zakończenie pracy watku ID: %d\n", id_watku);
                pthread_exit(NULL);
            }
            else
            {
                pthread_mutex_lock(&parametry.mutex2);
                if (parametry.i2 >= parametry.rozmiar_slownika)
                {
                    pthread_mutex_unlock(&parametry.mutex2);
                    break;
                }
                sprawdz_haslo(parametry.slownik[parametry.i2], parametry.uzytkownicy, parametry.liczba_uzytkownikow, &parametry, id_watku);
                
            }
        }
    }
    if (id_watku % 4 == 1)
    {
        while (1)
        {
            if (parametry.koniec == 1)
            {
                printf("Zakończenie pracy watku ID: %d\n", id_watku);
                pthread_exit(NULL);
            }
            else
            {
                pthread_mutex_lock(&parametry.mutex1);
                if (parametry.i1 >= parametry.rozmiar_slownika)
                {
                    pthread_mutex_unlock(&parametry.mutex1);
                    break;
                }
                sprawdz_haslo(parametry.slownik[parametry.i1], parametry.uzytkownicy, parametry.liczba_uzytkownikow, &parametry, id_watku);
            }
        }
    }

    printf("Zakończenie pracy watku ID: %d\n", id_watku);

    pthread_exit(NULL);
}

void obsluga_sighup()
{

    pthread_mutex_lock(&parametry.mutex);
    parametry.wyswietlanie = 1;
    printf("\nOtrzymano sygnał SIGHUP\n");
    pthread_cond_signal(&parametry.cond);
    pthread_mutex_unlock(&parametry.mutex);
}

void *konsument_hasel(void *arg)
{

    int id = *(int *)arg;
    int myid = id + 1;

    while (1)
    {
        if (parametry.koniec == 1)
        {   
            printf("Wątek konsumenta ID:%d : Zakończono działanie", myid);
            break;
        }
        else
        {
            pthread_mutex_lock(&parametry.mutex);
            pthread_cond_wait(&parametry.cond, &parametry.mutex);

            if (parametry.wyswietlanie == 1)
            {
                wypisz(parametry.glowa);

                parametry.wyswietlanie = 0;
            }
            else
            {
                printf("Wątek konsumenta ID:%d : Znaleziono hasło: %s\n", myid, parametry.haslo_zlamane_value);
                printf("Watek konsumenta ID:%d  wyrzucam z bazy hasło: ID=%s, Hasło=%s\n", myid,
                       parametry.uzytkownicy[parametry.haslo_zlamane].id, parametry.uzytkownicy[parametry.haslo_zlamane].hash);
                parametry.uzytkownicy[parametry.haslo_zlamane].zlamane = true;
            }
            parametry.blokada = false;

            pthread_mutex_unlock(&parametry.mutex);
            pthread_mutex_unlock(&parametry.mutex_zew);
        }
    }
    return NULL;
}

int main()
{
    const char *sciezka_slowa = "slownik_upgrade.txt"; // sciezka slownika
    char sciezka_hasel[MAX_DL_SLOWA];              // sciezka z zaszyfrowanymi haslami

    int liczba_uzytkownikow;
    Uzytkownik uzytkownicy[MAX_UZYTKOWNIKOW];
    char **slownik = NULL;
    int rozmiar_slownika = 0;
    pthread_t watki_producentow[LICZBA_WATKOW], konsument;
    int ids[LICZBA_WATKOW]; // Identyfikatory wątków

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); i++)
    {
        ids[i] = i + 1;
    }

    signal(SIGHUP, obsluga_sighup);

    printf("Wprowadz nazwe pliku z haslami do zlamania: \n");
    fscanf(stdin, "%s", sciezka_hasel);

    wczytaj_dane_uzytkownikow(sciezka_hasel, uzytkownicy, &liczba_uzytkownikow);
    wczytaj_slownik(sciezka_slowa, &slownik, &rozmiar_slownika);

    pthread_mutex_init(&parametry.mutex, NULL);
    pthread_mutex_init(&parametry.mutex4, NULL);
    pthread_mutex_init(&parametry.mutex3, NULL);
    pthread_mutex_init(&parametry.mutex2, NULL);
    pthread_mutex_init(&parametry.mutex1, NULL);
    pthread_mutex_init(&parametry.mutex_zew, NULL);
    pthread_cond_init(&parametry.cond, NULL);
    parametry.slownik = slownik;
    parametry.rozmiar_slownika = rozmiar_slownika;
    parametry.uzytkownicy = uzytkownicy;
    parametry.liczba_uzytkownikow = liczba_uzytkownikow;
    parametry.haslo_zlamane = 0;
    parametry.wyswietlanie = 0;
    parametry.glowa = NULL;
    parametry.koniec = 0;
    parametry.i4 = 0;
    parametry.i3 = 0;
    parametry.i2 = 0;
    parametry.i1 = 0;

    while (pthread_create(&konsument, NULL, konsument_hasel, (void *)&ids[(sizeof(ids) / sizeof(ids[0]) - 1)]) != 0)
        ;
    sleep(1);

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); i++)
    {
        while (pthread_create(&watki_producentow[i], NULL, producent_hasel, (void *)&ids[i]) != 0)
        {
        }
    }

    while (1)
    {

        
        printf("Wprowadz nazwe pliku z haslami do zlamania lub wpisz x aby zakonczyc po obliczeniach: \n");
        if (scanf("%s", sciezka_hasel) == EOF)
        {
            break; 
        }

        //jesli x to zakoncz dzialanie
        if (sciezka_hasel[0] == 'x')
        {   //parametry.koniec = 1;
            break;
        }

        parametry.koniec = 1;

        for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); i++)
        {
            pthread_join(watki_producentow[i], NULL);
        }
        
        pthread_cancel(konsument);
        pthread_join(konsument, NULL);

        for (int i = 0; i < MAX_UZYTKOWNIKOW; ++i)
        {
            uzytkownicy[i].zlamane = false;
            memset(uzytkownicy[i].hash, 0, sizeof(uzytkownicy[i].hash));
            memset(uzytkownicy[i].id, 0, sizeof(uzytkownicy[i].id));
        }

        wczytaj_dane_uzytkownikow(sciezka_hasel, uzytkownicy, &liczba_uzytkownikow);

        pthread_mutex_init(&parametry.mutex, NULL);
        pthread_mutex_init(&parametry.mutex4, NULL);
        pthread_mutex_init(&parametry.mutex3, NULL);
        pthread_mutex_init(&parametry.mutex2, NULL);
        pthread_mutex_init(&parametry.mutex1, NULL);
        pthread_mutex_init(&parametry.mutex_zew, NULL);
        pthread_cond_init(&parametry.cond, NULL);
        parametry.slownik = slownik;
        parametry.rozmiar_slownika = rozmiar_slownika;
        parametry.uzytkownicy = uzytkownicy;
        parametry.liczba_uzytkownikow = liczba_uzytkownikow;
        parametry.haslo_zlamane = 0;
        parametry.wyswietlanie = 0;
        freelista(parametry.glowa);
        parametry.glowa = NULL;
        parametry.koniec = 0;
        parametry.i4 = 0;
        parametry.i3 = 0;
        parametry.i2 = 0;
        parametry.i1 = 0;

        pthread_create(&konsument, NULL, konsument_hasel, (void *)&ids[(sizeof(ids) / sizeof(ids[0]) - 1)]);

        for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); i++)
        {
            pthread_create(&watki_producentow[i], NULL, producent_hasel, (void *)&ids[i]);
        }
    }

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); i++)
    {
        pthread_join(watki_producentow[i], NULL);
    }
    
    pthread_cancel(konsument);
    pthread_join(konsument, NULL);

    printf("To wszystkie złamane hasła podczas działania programu:\n");
    printf("------------------------------------------------------\n");
    wypisz(parametry.glowa);

    for (int i = 0; i < rozmiar_slownika; i++)
    {
        free(slownik[i]);
    }
    free(slownik);

    freelista(parametry.glowa);

    pthread_mutex_destroy(&parametry.mutex);
    pthread_mutex_destroy(&parametry.mutex_zew);
    pthread_mutex_destroy(&parametry.mutex4);
    pthread_mutex_destroy(&parametry.mutex3);
    pthread_mutex_destroy(&parametry.mutex2);
    pthread_mutex_destroy(&parametry.mutex1);
    pthread_cond_destroy(&parametry.cond);

    return 0;
}