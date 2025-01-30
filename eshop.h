#ifndef ESHOP_H
#define ESHOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define SERVER_PORT 8080
#define TOTAL_PRODUCTS 20
#define MAX_CLIENTS 5
#define ORDERS_PER_CLIENT 10

// Δομή προϊόντος
typedef struct {
    char name[50];
    float cost;
    int stock;
    int order_count;
    int sold_count;
    char denied_clients[100][50];
    int denied_count;
} Item;

extern Item inventory[TOTAL_PRODUCTS];
extern int total_requests, successful_requests, failed_requests;
extern float total_income;

void setup_inventory();
void *process_client(void *client_sock);

#endif
