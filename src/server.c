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

// Fonction de fermeture
void exitFunction();


void *handleClient(void *arg) {
    printf("Client connecté\n");
    socket_t *sd = (socket_t *) arg;
    aotp_request_t *request = malloc(sizeof(aotp_request_t));
    buffer_t *buffer = malloc(sizeof(buffer_t));
    
    recv_data(sd, request, (serialize_t) request2Struct);
    requestHandler(sd, request, &clients, &parties, NULL);
    // Fermeture de la socket
    freeSocket(sd);
    // Liberation de la memoire
    free(request);

    // Fin du thread
    pthread_exit(NULL);
}

int main() {
    atexit(exitFunction);
    se = createListeningSocket(DEFAULT_AOTP_IP, DEFAULT_AOTP_PORT, DEFAULT_AOTP_MAX_CLIENTS);
    sem_clients = create_sem(1);
    sem_parties = create_sem(1);
    socket_t *sd;

    while(1) {
        sd = acceptClient(se);
        pthread_t thread; 
        // Create a new thread for each client
        pthread_create(&thread, NULL, handleClient, (void *) sd);
        pthread_detach(thread);
    }

    return EXIT_SUCCESS;
}

void exitFunction() {
    printf("Fermeture du serveur\n");

    // Fermeture de la socket
    freeSocket(se);
    
    // Liberation de la memoire
    freeListParty(parties);
}