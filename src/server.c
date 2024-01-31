#include "aotp.h"
#include "avalam.h"
#include "mysyscall.h"
#include "data.h"
#include <semaphore.h>

socket_t *se;
list_client_t *clients = NULL; // Liste des clients connectes
list_party_t *parties;         // Liste des parties en cours

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
    // Creation de parties de test
    // TODO : A SUPPRIMER
    party_t *p1 = malloc(sizeof(party_t));
    p1->id = 1;
    strncpy(p1->host_ip,"0",1);
    p1->host_port = 0;
    p1->state = PARTY_WAITING;
    strcpy(p1->host_pseudo, "Lukas");
    party_t *p2 = malloc(sizeof(party_t));
    p2->id = 2;
    strncpy(p2->host_ip,"0",1);
    p2->host_port = 0;
    p2->state = PARTY_PLAYING;
    strcpy(p2->host_pseudo, "Tomas");

    party_t *p3 = malloc(sizeof(party_t));
    p3->id = 3;
    strncpy(p3->host_ip,"0",1);
    p3->host_port = 0;
    p3->state = PARTY_WAITING;
    strcpy(p3->host_pseudo, "Arthur");

    addParty(&parties, p1);
    addParty(&parties, p2);
    addParty(&parties, p3);
    // fin de la section

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