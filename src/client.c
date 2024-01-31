#include "aotp.h"
#include "mysyscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define COULEUR(i) printf("\e[%dm", i)
#define COLOR_RESET printf("\e[0m")
#define RED 31
#define GREEN 32
#define BLUE 34

#define CHECK_SERVER 1
#define HOST_SERVER 2

list_party_t *parties = NULL; // Liste des parties en cours
list_client_t *players = NULL; // Liste des joueurs connectés

client_t *client = NULL; // Informations du client

void clearBuffer();
void loadingBar();
void getPseudo(char *pseudo);
void afficherParties();
void connReq(char *pseudo, char *serverAddress, short serverPort);

void createPartyReq(char *serverAddress, short serverPort, char *hostIp, short hostPort) {
    socket_t *socket = connectToServer(serverAddress, serverPort);

    aotp_request_t *request = createRequest(AOTP_CREATE_PARTY);
    request->client_id = client->id;
    strcpy(request->pseudo, client->pseudo);
    request->action = AOTP_CREATE_PARTY;
    request->party_id = -1;
    strcpy(request->host_ip, hostIp);
    request->host_port = hostPort;
    send_data(socket, request, (serialize_t) struct2Request);

    aotp_response_t *response = malloc(sizeof(aotp_response_t));
    recv_data(socket, response, (serialize_t) response2Struct);

    if(response->code == AOTP_OK) {
        printf("Partie créée !\n");
        // Creation du thread d'hote

    }
}

socket_t *host_se, *host_sd; // Socket d'écoute et socket de dialogue

int main(int argc, char *argv[]) {
    char pseudo[20];
    char *serverAddress;
    short serverPort;

    // Récupération des arguments
    getServerAddress(argc, argv, &serverAddress, &serverPort);
    getPseudo(pseudo);
    system("clear");
    connReq(pseudo, serverAddress, serverPort);

    // Barre de chargement
    loadingBar();
    
    while(1){
        system("clear");
        COULEUR(RED);
        printf("\n\nBienvenue \e[%dm%s, \e[%dmvoici la liste des parties en cours !\n", GREEN, pseudo, RED);
        system("clear");
        // Affichage des parties en cours
        afficherParties();

        COULEUR(RED);
        printf("Que voulez-vous faire ?\n\n");
        printf("1. Créer une partie\n");
        printf("2. Rejoindre une partie\n");
        printf("3. Quitter\n\n");
        printf("Votre choix : ");

        COULEUR(GREEN);
        fflush(stdin);
        int choix = fgetc(stdin);
        COLOR_RESET;
        switch (choix) {
            case '1':
                COULEUR(RED);
                system("clear");
                printf("------ Création de la partie ------\n\n Veuillez entrer le nom de la partie : ");
                // TODO : CREER PARTIE
                break;
            case '2':
                COULEUR(RED);
                printf("------ Rejoindre une partie ------\n\n Veuillez entrer le numéro de la partie à rejoindre : ");
                COLOR_RESET;
                COULEUR(GREEN);
                char numPartie = fgetc(stdin);
                clearBuffer();
                printf("Num partie : %c\n", numPartie);
                // TODO : REJOINDRE PARTIE
            break;

            case '3':
                COULEUR(RED);
                printf("Ah ché genant\n");
                COLOR_RESET;
                exit(0);
            break;
            
            default:
                COULEUR(RED);
                printf("\nCe choix n'est pas valide\n\n");
                COLOR_RESET;
            break;
        }
    }
}

void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void loadingBar()
{
    // Barre de chargement
    COULEUR(BLUE);
    printf("\nRécupération des données en cours...\n");
    for (int i = 0; i < 100; i++) {
        printf("\r");
        printf("[");
        for (int j = 0; j < i; j++) printf("=");

        for (int j = 0; j < 100 - i; j++) printf(" ");
        printf("] %d%%", i);
        fflush(stdout);
        usleep(1000);
    }
    printf("\n");
    COLOR_RESET;
}

void getPseudo(char *pseudo)
{
    system("clear");
    COULEUR(RED);
    printf("------ Bienvenue sur AvalOnline ! ------ \n\n");
    COLOR_RESET;
    COULEUR(RED);
    printf("Veuillez entrer votre pseudo : ");
    COLOR_RESET;
    COULEUR(GREEN);
    fgets(pseudo, 20, stdin);
    COLOR_RESET;
    // Suppression du \n à la fin du pseudo
    int i = 0;
    while (pseudo[i] != '\n')
    {
        i++;
    }
    pseudo[i] = '\0';
}

void afficherParties() {
    printf("\n\nParties en cours :\n");
    // afficher les parties de la liste
    if(parties == NULL) {
        COULEUR(BLUE);
        printf("\e[0mAucune partie en cours\n");
        return;
    }

    list_party_t *current = parties;
    while(current != NULL) {
        COULEUR(BLUE);
        char *status = partyState2String(current->party->state);
        printf("%3d. \e[0m Partie de %-15s %s\n", current->party->id, current->party->host_pseudo, status);
        current = current->next;
    }

}


void connReq(char *pseudo, char *serverAddress, short serverPort) {
    // Création de la socket
    socket_t *socket = connectToServer(serverAddress, serverPort);

    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_CONNECT);
    strcpy(request->pseudo, pseudo);
    request->client_id = -1;
    send_data(socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t *response = malloc(sizeof(aotp_response_t));
    recv_data(socket, response, (serialize_t) response2Struct);

    // Vérification de la réponse
    // TODO : Remplacer par un handler de réponse
    if(response->code == AOTP_OK) {
        printf("Connexion réussie !\n");
        printf("Votre ID : %d\n", response->client_id);
    }
    // Libération de la mémoire
    free(response);
    free(request);
    freeSocket(socket);
}

void *handleClient(void *arg) {
    printf("Client connecté\n");
    socket_t *sd = (socket_t *) arg;
    aotp_request_t *request = malloc(sizeof(aotp_request_t));
    char *buffer = malloc(sizeof(buffer_t));

    // Reception de la requete
    recv_data(sd, request, (serialize_t) request2Struct);
    requestHandler(sd, request, &players, &parties);
    
    // Fermeture de la socket
    freeSocket(sd);
}

void host(char *hostIp, short hostPort) {
    host_se = createListeningSocket(hostIp, hostPort, DEFAULT_AOTP_MAX_CLIENTS);

    while(1) {
        host_sd = acceptClient(host_se);
        pthread_t thread; 
        // Create a new thread for each client
        pthread_create(&thread, NULL, handleClient, (void *) host_sd);
        pthread_detach(thread);
    }
}