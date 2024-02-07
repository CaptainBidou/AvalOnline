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

/**
* \fn void *handleClient(void *arg)
* \brief Fonction qui gere un client
* \param arg Socket du client
*/
void *handleClient(void *arg) {
    printf("Client connecté\n");
    socket_t *sd = (socket_t *) arg;
    aotp_request_t request;

    recv_data(sd, &request, (serialize_t) request2Struct);
    requestHandler(sd, &request, &clients, &parties, NULL);
    // Fermeture de la socket
    freeSocket(sd);

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

/**
* \fn void exitFunction()
* \brief Fonction de fermeture
*/
void exitFunction() {
    printf("Fermeture du serveur\n");

    // Fermeture de la socket
    freeSocket(se);
    // Liberation de la memoire
    freePartyList(&parties);
}