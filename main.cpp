#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_PRODUSE 100
#define MAX_REZERVARI 100
#define MAX_NAME 50

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

typedef struct {
    int id;
    char name[MAX_NAME];
    float price;
    int stock;
    char expiry_date[11];
} Product;

typedef struct {
    int id;
    int product_id;
    char client_name[MAX_NAME];
    int quantity;
    char reservation_date[11];
} Reservation;

Product products[MAX_PRODUSE];
Reservation reservations[MAX_REZERVARI];
int product_count = 0;
int reservation_count = 0;

void clearScreen() {
    sleep(1);
    system("cls||clear");
}

void printHeader() {
    printf(CYAN "\n=== SISTEM DE REZERVARE FARMACIE ===\n" RESET);
    printf(CYAN "1" RESET " - Afiseaza produse disponibile\n");
    printf(CYAN "2" RESET " - Rezerva produs\n");
    printf(CYAN "3" RESET " - Anuleaza rezervare\n");
    printf(CYAN "4" RESET " - Cauta produs\n");
    printf(CYAN "5" RESET " - Afiseaza rezervari\n");
    printf(CYAN "6" RESET " - Adauga produs nou\n");
    printf(CYAN "7" RESET " - Sterge produs\n");
    printf(CYAN "0" RESET " - Iesire\n");
    printf(CYAN "\n================================================================\n" RESET);
}

int validateDate(const char* date) {
    if (strlen(date) != 10) return 0;
    if (date[2] != '/' || date[5] != '/') return 0;

    int day = atoi(date);
    int month = atoi(date + 3);
    int year = atoi(date + 6);

    if (day < 1 || day > 31) return 0;
    if (month < 1 || month > 12) return 0;
    if (year < 2023 || year > 2100) return 0;

    return 1;
}

void loadProducts() {
    FILE *f = fopen("produse.txt", "r");
    if (f == NULL) {
        printf(RED "Atentie: Fisierul produse.txt nu a fost gasit. Va fi creat unul nou.\n" RESET);
        return;
    }

    product_count = 0;
    while (fscanf(f, "%d %49s %f %d %10s",
                  &products[product_count].id,
                  products[product_count].name,
                  &products[product_count].price,
                  &products[product_count].stock,
                  products[product_count].expiry_date) == 5) {
        product_count++;
        if (product_count >= MAX_PRODUSE) break;
    }
    fclose(f);
}

void loadReservations() {
    FILE *f = fopen("rezervari.txt", "r");
    if (f == NULL) {
        printf(RED "Atentie: Fisierul rezervari.txt nu a fost gasit. Va fi creat unul nou.\n" RESET);
        return;
    }

    reservation_count = 0;
    while (fscanf(f, "%d %d %49s %d %10s",
                  &reservations[reservation_count].id,
                  &reservations[reservation_count].product_id,
                  reservations[reservation_count].client_name,
                  &reservations[reservation_count].quantity,
                  reservations[reservation_count].reservation_date) == 5) {
        reservation_count++;
        if (reservation_count >= MAX_REZERVARI) break;
    }
    fclose(f);
}

void saveProducts() {
    FILE *f = fopen("produse.txt", "w");
    if (f == NULL) {
        printf(RED "Eroare la salvarea in produse.txt!\n" RESET);
        return;
    }

    for (int i = 0; i < product_count; i++) {
        fprintf(f, "%d %s %.2f %d %s\n",
                products[i].id,
                products[i].name,
                products[i].price,
                products[i].stock,
                products[i].expiry_date);
    }
    fclose(f);
    printf(GREEN "Datele au fost salvate in produse.txt\n" RESET);
}

void saveReservations() {
    FILE *f = fopen("rezervari.txt", "w");
    if (f == NULL) {
        printf(RED "Eroare la salvarea in rezervari.txt!\n" RESET);
        return;
    }

    for (int i = 0; i < reservation_count; i++) {
        fprintf(f, "%d %d %s %d %s\n",
                reservations[i].id,
                reservations[i].product_id,
                reservations[i].client_name,
                reservations[i].quantity,
                reservations[i].reservation_date);
    }
    fclose(f);
    printf(GREEN "Datele au fost salvate in rezervari.txt\n" RESET);
}

void showProducts() {
    clearScreen();
    printf(CYAN "\n=== PRODUSE DISPONIBILE ===\n" RESET);
    printf(YELLOW "ID   Nume                 Pret     Stoc   Expira\n" RESET);
    printf("------------------------------------------------\n");

    for (int i = 0; i < product_count; i++) {
        printf("%-4d " MAGENTA "%-20s" RESET " " GREEN "%-6.2f RON" RESET " %-6d " RED "%s" RESET "\n",
               products[i].id,
               products[i].name,
               products[i].price,
               products[i].stock,
               products[i].expiry_date);
    }
}

void addProduct() {
    clearScreen();
    printf(CYAN "\n=== ADAUGA PRODUS NOU ===\n" RESET);

    if (product_count >= MAX_PRODUSE) {
        printf(RED "Nu mai pot fi adaugate produse! Limita maxima (%d) a fost atinsa.\n" RESET, MAX_PRODUSE);
        return;
    }

    Product p;
    int id_exists;

    do {
        id_exists = 0;
        printf("ID produs (numar intreg unic): ");
        while (scanf("%d", &p.id) != 1) {
            printf(RED "Input invalid! Introdu un numar intreg pentru ID: " RESET);
            while (getchar() != '\n');
        }

        for (int i = 0; i < product_count; i++) {
            if (products[i].id == p.id) {
                printf(RED "ID-ul %d exista deja! Alegeti alt ID.\n" RESET, p.id);
                id_exists = 1;
                break;
            }
        }
    } while (id_exists);

    printf("Nume produs (max %d caractere fara spatii): ", MAX_NAME-1);
    scanf("%49s", p.name);

    printf("Pret (format 00.00, valoare pozitiva): ");
    while (scanf("%f", &p.price) != 1 || p.price <= 0) {
        printf(RED "Pret invalid! Introdu un numar pozitiv (ex: 12.99): " RESET);
        while (getchar() != '\n');
    }

    printf("Stoc initial (numar intreg pozitiv): ");
    while (scanf("%d", &p.stock) != 1 || p.stock < 0) {
        printf(RED "Stoc invalid! Introdu un numar intreg pozitiv: " RESET);
        while (getchar() != '\n');
    }

    do {
        printf("Data expirarii (format ZZ/LL/AAAA, ex: 15/06/2024): ");
        scanf("%10s", p.expiry_date);
        if (!validateDate(p.expiry_date)) {
            printf(RED "Format invalid! Foloseste exact formatul ZZ/LL/AAAA\n" RESET);
        }
    } while (!validateDate(p.expiry_date));

    products[product_count++] = p;
    saveProducts();

    printf(GREEN "\nProdus adaugat cu succes!\n" RESET);
}

void deleteProduct() {
    clearScreen();
    printf(CYAN "\n=== STERGERE PRODUS ===\n" RESET);
    showProducts();

    if (product_count == 0) {
        printf(YELLOW "Nu exista produse inregistrate.\n" RESET);
        return;
    }

    int product_id;
    printf("\nIntroduceti ID-ul produsului de sters: ");
    while (scanf("%d", &product_id) != 1) {
        printf(RED "Input invalid! Introdu un numar: " RESET);
        while (getchar() != '\n');
    }

    int found = 0;
    for (int i = 0; i < product_count; i++) {
        if (products[i].id == product_id) {
            found = 1;

            int reservation_count_for_product = 0;
            for (int j = 0; j < reservation_count; j++) {
                if (reservations[j].product_id == product_id) {
                    reservation_count_for_product++;
                }
            }

            if (reservation_count_for_product > 0) {
                printf(YELLOW "\nAtentie: Produsul are %d rezervari active!\n" RESET, reservation_count_for_product);
            }

            printf(MAGENTA "\nSigur doriti sa stergeti produsul %s (ID: %d)? (d/n): " RESET,
                  products[i].name, products[i].id);
            char confirm;
            scanf(" %c", &confirm);

            if (tolower(confirm) == 'd') {
                for (int j = i; j < product_count - 1; j++) {
                    products[j] = products[j + 1];
                }
                product_count--;

                saveProducts();
                printf(GREEN "\nProdusul a fost sters cu succes!\n" RESET);
            } else {
                printf(YELLOW "\nStergere anulata.\n" RESET);
            }
            break;
        }
    }

    if (!found) {
        printf(RED "\nProdusul cu ID-ul %d nu a fost gasit!\n" RESET, product_id);
    }
}

void reserveProduct() {
    clearScreen();
    printf(CYAN "\n=== REZERVARE PRODUS ===\n" RESET);
    showProducts();

    Reservation r;
    int product_id, found = 0;

    printf("\nIntroduceti ID-ul produsului pe care doriti sa il rezervati: ");
    while (scanf("%d", &product_id) != 1) {
        printf(RED "Input invalid! Introdu un numar intreg pentru ID: " RESET);
        while (getchar() != '\n');
    }

    for (int i = 0; i < product_count; i++) {
        if (products[i].id == product_id) {
            found = 1;
            printf("Produs selectat: " MAGENTA "%s" RESET " (Stoc disponibil: " GREEN "%d" RESET ")\n",
                   products[i].name, products[i].stock);

            printf("Introduceti numele dvs. (max %d caractere): ", MAX_NAME-1);
            scanf("%49s", r.client_name);

            do {
                printf("Cantitatea dorita (numar intreg pozitiv): ");
                while (scanf("%d", &r.quantity) != 1) {
                    printf(RED "Input invalid! Introdu un numar intreg: " RESET);
                    while (getchar() != '\n');
                }

                if (r.quantity <= 0) {
                    printf(RED "Cantitatea trebuie sa fie pozitiva!\n" RESET);
                } else if (r.quantity > products[i].stock) {
                    printf(RED "Stoc insuficient! Disponibil: " GREEN "%d" RESET "\n", products[i].stock);
                }
            } while (r.quantity <= 0 || r.quantity > products[i].stock);

            products[i].stock -= r.quantity;

            r.id = reservation_count + 1;
            r.product_id = product_id;

            do {
                printf("Data rezervarii (format ZZ/LL/AAAA): ");
                scanf("%10s", r.reservation_date);
                if (!validateDate(r.reservation_date)) {
                    printf(RED "Format invalid! Foloseste exact formatul ZZ/LL/AAAA\n" RESET);
                }
            } while (!validateDate(r.reservation_date));

            reservations[reservation_count++] = r;
            saveProducts();
            saveReservations();

            printf(GREEN "\nRezervare efectuata cu succes!\n" RESET);
            printf("Numar rezervare: " BLUE "%d" RESET "\n", r.id);
            printf("Data rezervarii: " YELLOW "%s" RESET "\n", r.reservation_date);
            break;
        }
    }

    if (!found) {
        printf(RED "\nProdusul cu ID-ul %d nu a fost gasit!\n" RESET, product_id);
    }
}

void showReservations() {
    clearScreen();
    printf(CYAN "\n=== LISTA DE REZERVARI ===\n" RESET);

    if (reservation_count == 0) {
        printf(YELLOW "Nu exista rezervari inregistrate.\n" RESET);
        return;
    }

    printf(YELLOW "ID   ID Produs  Client          Cantitate  Data\n" RESET);
    printf("------------------------------------------------\n");

    for (int i = 0; i < reservation_count; i++) {
        printf("%-4d %-10d %-15s %-8d " BLUE "%s" RESET "\n",
               reservations[i].id,
               reservations[i].product_id,
               reservations[i].client_name,
               reservations[i].quantity,
               reservations[i].reservation_date);
    }
}

void cancelReservation() {
    clearScreen();
    printf(CYAN "\n=== ANULARE REZERVARE ===\n" RESET);
    showReservations();

    if (reservation_count == 0) return;

    int reservation_id;
    printf("\nIntroduceti ID-ul rezervarii de anulat: ");
    while (scanf("%d", &reservation_id) != 1) {
        printf(RED "Input invalid! Introdu un numar: " RESET);
        while (getchar() != '\n');
    }

    int found = 0;
    for (int i = 0; i < reservation_count; i++) {
        if (reservations[i].id == reservation_id) {
            found = 1;

            for (int j = 0; j < product_count; j++) {
                if (products[j].id == reservations[i].product_id) {
                    products[j].stock += reservations[i].quantity;
                    break;
                }
            }

            for (int j = i; j < reservation_count - 1; j++) {
                reservations[j] = reservations[j + 1];
            }
            reservation_count--;

            saveProducts();
            saveReservations();
            printf(GREEN "\nRezervarea a fost anulata cu succes!\n" RESET);
            break;
        }
    }

    if (!found) {
        printf(RED "\nRezervarea nu a fost gasita!\n" RESET);
    }
}

void searchProduct() {
    clearScreen();
    printf(CYAN "\n=== CAUTARE PRODUS ===\n" RESET);

    char search_term[MAX_NAME];
    printf("Introduceti numele produsului cautat: ");
    scanf("%49s", search_term);

    printf(YELLOW "\nRezultatele cautarii:\n" RESET);
    printf("ID   Nume                 Pret     Stoc   Expira\n");
    printf("------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < product_count; i++) {
        if (strstr(products[i].name, search_term) != NULL) {
            printf("%-4d " MAGENTA "%-20s" RESET " " GREEN "%-6.2f RON" RESET " %-6d " RED "%s" RESET "\n",
                   products[i].id,
                   products[i].name,
                   products[i].price,
                   products[i].stock,
                   products[i].expiry_date);
            found = 1;
        }
    }

    if (!found) {
        printf(RED "Nu au fost gasite produse care corespund cautarii.\n" RESET);
    }
}

int main() {
    loadProducts();
    loadReservations();

    int option;
    do {
        clearScreen();
        printHeader();
        printf("\nAlegeti o optiune: ");
        while (scanf("%d", &option) != 1) {
            printf(RED "Input invalid! Introdu un numar: " RESET);
            while (getchar() != '\n');
        }

        switch(option) {
            case 1: showProducts(); break;
            case 2: reserveProduct(); break;
            case 3: cancelReservation(); break;
            case 4: searchProduct(); break;
            case 5: showReservations(); break;
            case 6: addProduct(); break;
            case 7: deleteProduct(); break;
            case 0: printf("\nLa revedere!\n"); break;
            default: printf(RED "\nOptiune invalida!\n" RESET);
        }

        if (option != 0) {
            printf("\nApasa Enter pentru a continua...");
            while (getchar() != '\n');
            getchar();
        }
    } while (option != 0);

    return 0;
}
