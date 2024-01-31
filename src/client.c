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


void clearBuffer();
void loadingBar();
void getPseudo(char *pseudo);
void afficherParties();
void connReq(char *pseudo, char *serverAddress, short serverPort);

socket_t se, sd; // Socket d'écoute et socket de dialogue


int main(int argc, char *argv[]) {
    char pseudo[20];
    char *serverAddress;
    short serverPort;

    // Creation de parties de test
    // TODO : A SUPPRIMER
    party_t *p1 = malloc(sizeof(party_t));
    p1->id = 1;
    p1->host_port = 0;
    p1->state = PARTY_WAITING;
    strcpy(p1->host_pseudo, "Lukas");
    party_t *p2 = malloc(sizeof(party_t));
    p2->id = 2;
    p2->host_port = 0;
    p2->state = PARTY_PLAYING;
    strcpy(p2->host_pseudo, "Tomas");

    party_t *p3 = malloc(sizeof(party_t));
    p3->id = 3;
    p3->host_port = 0;
    p3->state = PARTY_WAITING;
    strcpy(p3->host_pseudo, "Arthur");

    addParty(&parties, p1);
    addParty(&parties, p2);
    addParty(&parties, p3);


    // Récupération des arguments
    getServerAddress(argc, argv, &serverAddress, &serverPort);
    getPseudo(pseudo);
    system("clear");
    // TODO : Requete de connexion
    connReq(pseudo, serverAddress, serverPort);
    // TODO : Récupération de la réponse avec la liste des parties en cours

    // Barre de chargement
    loadingBar();
    

    while(1){
        system("clear");

    COULEUR(RED);
    printf("\n\nBienvenue \e[%dm%s, \e[%dmvoici la liste des parties en cours !\n", GREEN, pseudo, RED);
    system("clear");

    // Affichage des parties en cours
    afficherParties();

<<<<<<< HEAD
        switch (choix){
            case '1':
                COULEUR(RED);
                system("clear");
                printf("------ Création de la partie ------\n\n");
            
=======
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

>>>>>>> 00bdefe9d8152fd6f61f7afeaa05cecca51be5ef
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
    }
    // Libération de la mémoire
    free(response);
    free(request);
    freeSocket(socket);
}