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

list_party_t *parties = NULL;         // Liste des parties en cours
client_t *client = NULL;

void clearBuffer();
void loadingBar();
void getPseudo(char *pseudo);
void afficherParties();
void connReq(char *pseudo, char *serverAddress, short serverPort);
list_party_t* listPartyReq(char *pseudo,char *serverAddress, short serverPort);

socket_t se, sd; // Socket pour l'host d'écoute et socket de dialogue


int main(int argc, char *argv[]) {
    char pseudo[20];
    char *serverAddress;
    short serverPort;
    client = malloc(sizeof(client_t));

    // Récupération des arguments
    getServerAddress(argc, argv, &serverAddress, &serverPort);
    getPseudo(pseudo);
    system("clear");
    connReq(pseudo, serverAddress, serverPort);

    parties = listPartyReq(pseudo,serverAddress,serverPort);

    // Barre de chargement
    loadingBar();
    

    while(1){
        system("clear");

        COULEUR(RED);
        printf("\n\nBienvenue \e[%dm%s, \e[%dmvoici la liste des parties en cours !\n", GREEN, pseudo, RED);

        // Affichage des parties en cours
        afficherParties();
        menu();
        
        COULEUR(GREEN);
        char choix = fgetc(stdin);
        clearBuffer();
        COLOR_RESET;

        switch (choix){
            case '1':
                COULEUR(RED);
                system("clear");
                printf("------ Création de la partie ------\n\n");
            
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
    for (int i = 0; i < 100; i++)
    {
        printf("\r");
        printf("[");
        for (int j = 0; j < i; j++)
        {
            printf("=");
        }
        for (int j = 0; j < 100 - i; j++)
        {
            printf(" ");
        }
        printf("] %d%%", i);
        fflush(stdout);
        usleep(10000);
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
        client->id = response->client_id;
        
    }
    // Libération de la mémoire
    free(response);
    free(request);
    freeSocket(socket);
}


list_party_t* listPartyReq(char *pseudo,char *serverAddress, short serverPort){
    // Création de la socket
    socket_t *socket = connectToServer(serverAddress, serverPort);

    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_LIST_PARTIES);
    strcpy(request->pseudo, pseudo);
    request->client_id = client->id;
    send_data(socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t *response = malloc(sizeof(aotp_response_t));
    recv_data(socket, response, (serialize_t) response2Struct);

    // Vérification de la réponse
    // TODO : Remplacer par un handler de réponse
    
    //afficher les parties de la liste

    list_party_t* tmp = response->parties;

    // Libération de la mémoire
    
    free(request);
    freeSocket(socket);
    free(response);

    return tmp;
    
   

}