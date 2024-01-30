#include "aotp.h"
#include "avalam.h"
#include "mysyscall.h"
#include "data.h"

socket_t *se;
list_client_t *clients = NULL; // Liste des clients connectes
list_party_t *parties;         // Liste des parties en cours


void *handleClient(void *arg) {
    socket_t *sd = (socket_t *) arg;
    aotp_request_t *request = malloc(sizeof(aotp_request_t));
    char *buffer = malloc(sizeof(aotp_request_t));
    
    // Reception de la requete
    recv_data(sd, request, request2Struct);
    requestHandler(sd, request, &clients, &parties);


    // Si le client s'est deconnecte
    if(request->action == AOTP_DISCONNECT) {
        // On ferme la socket
        close(sd->fd);
        freeSocket(sd);
    }

}

int main() {
    se = createListeningSocket(DEFAULT_AOTP_IP, DEFAULT_AOTP_PORT, DEFAULT_AOTP_MAX_CLIENTS);
    socket_t *sd;

    while(1) {
        sd = acceptClient(se);
        // Create a new thread for each client
        pthread_t thread;

        // Create a new thread for each client
        pthread_create(&thread, NULL, handleClient, (void *) sd);
        pthread_detach(thread);
        

    }
}