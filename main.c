#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define SERVER_PORT 8080 // Θύρα διακομιστή
#define TOTAL_PRODUCTS 20 // Συνολικός αριθμός προϊόντων
#define MAX_CLIENTS 5 // Μέγιστος αριθμός πελατών
#define ORDERS_PER_CLIENT 10 // Αριθμός παραγγελιών ανά πελάτη

// Δομή που αναπαριστά ένα προϊόν στο κατάστημα
typedef struct {
    char name[50]; // Όνομα προϊόντος
    float cost; // Τιμή προϊόντος
    int stock; // Διαθέσιμο απόθεμα
    int order_count; // Αριθμός αιτήσεων αγοράς
    int sold_count; // Αριθμός τεμαχίων που πουλήθηκαν
    char denied_clients[100][50]; // Λίστα πελατών που δεν εξυπηρετήθηκαν
    int denied_count; // Συνολικός αριθμός αποτυχημένων παραγγελιών
} Item;

// Παγκόσμιες μεταβλητές
Item inventory[TOTAL_PRODUCTS];
int total_requests = 0, successful_requests = 0, failed_requests = 0;
float total_income = 0;

// Συνάρτηση για αρχικοποίηση του αποθέματος προϊόντων
void setup_inventory() {
    for (int i = 0; i < TOTAL_PRODUCTS; i++) {
        sprintf(inventory[i].name, "Product_%d", i + 1);
        inventory[i].cost = (rand() % 1000) / 10.0; // Τυχαία τιμή προϊόντος
        inventory[i].stock = 2; // Διαθέσιμα τεμάχια
        inventory[i].order_count = 0;
        inventory[i].sold_count = 0;
        inventory[i].denied_count = 0;
    }
}

// Συνάρτηση χειρισμού πελάτη
void *process_client(void *client_sock) {
    int sock = *(int *)client_sock;
    free(client_sock);
    char buffer[256];
    for (int i = 0; i < ORDERS_PER_CLIENT; i++) {
        int product_index = rand() % TOTAL_PRODUCTS;
        inventory[product_index].order_count++;
        sleep(1); // Χρόνος επεξεργασίας
        if (inventory[product_index].stock > 0) {
            inventory[product_index].stock--;
            inventory[product_index].sold_count++;
            successful_requests++;
            total_income += inventory[product_index].cost;
            sprintf(buffer, "Order successful for %s!\n", inventory[product_index].name);
        } else {
            failed_requests++;
            inventory[product_index].denied_count++;
            sprintf(buffer, "Order failed for %s!\n", inventory[product_index].name);
        }
        send(sock, buffer, strlen(buffer), 0);
    }
    close(sock);
    return NULL;
}

int main() {
    setup_inventory();
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, MAX_CLIENTS);
    
    printf("Server listening on port %d...\n", SERVER_PORT);
    
    while (1) {
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, NULL, NULL);
        pthread_t thread;
        pthread_create(&thread, NULL, process_client, client_sock);
        pthread_detach(thread);
    }
    return 0;
}
