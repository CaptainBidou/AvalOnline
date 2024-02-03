#include "aotp.h"
#include "avalam.h"
#include "mysyscall.h"
#include "data.h"
#include <semaphore.h>

socket_t *se;
list_client_t *clients = NULL; // Liste des clients connectes
list_party_t *parties = NULL;         // Liste des parties en cours

// mutex pour la liste des clients
sem_t *sem_clients;
sem_t *sem_parties;


void *handleClient(void *arg) {
    printf("Client connecté\n");
    socket_t *sd = (socket_t *) arg;
    aotp_request_t *request = malloc(sizeof(aotp_request_t));
    char *buffer = malloc(sizeof(buffer_t));

    // Reception de la requete
    recv_data(sd, request, (serialize_t) request2Struct);
    requestHandler(sd, request, &clients, &parties);
    
    // Fermeture de la socket
    freeSocket(sd);
}

int main() {
    se = createListeningSocket(DEFAULT_AOTP_IP, DEFAULT_AOTP_PORT, DEFAULT_AOTP_MAX_CLIENTS);
    sem_clients = create_sem(0);
    sem_parties = create_sem(0);
    socket_t *sd;

    while(1) {
        sd = acceptClient(se);
        pthread_t thread; 
        // Create a new thread for each client
        pthread_create(&thread, NULL, handleClient, (void *) sd);
        pthread_detach(thread);
    }
}