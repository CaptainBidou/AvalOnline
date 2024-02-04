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

// Variable globale

///  Variable en tant que host
party_t *myParty = NULL; // Informations de la partie de l'utilisateur
list_client_t *players = NULL; // Liste des joueurs connectés
position_t hostPosition;

/// Variable en tant que client
list_party_t *parties = NULL; // Liste des parties en cours
client_t *client = NULL; // Informations du client
position_t position; // Position actuelle du client


// Fonctions propre à l'interface
void clearBuffer();
void menu();
void loadingBar();
void getPseudo(char *pseudo);
void afficherParties();
void promptHostIpPort(char *hostIp, short *hostPort);
void playGame(client_t *client, party_state_t state);
void exitFunction(); // Fonction de sortie
int handleResponse(aotp_response_t response_data);
void gameLoop(client_t *client, client_state_t state);
evolution_t promptEvolution(int numCoup);
aotp_response_t jouerEvolutionReq(client_t *client, position_t p, evolution_t evolution);

// Thread du serveur d'hébergement
void host(char *hostIp, short hostPort);

// Requete vers le serveur d'enregistrement
aotp_response_t createPartyReq(socket_t *socket, char *hostIp, short hostPort);
aotp_response_t requestJoinParty(client_t *client, party_id_t partyId);
aotp_response_t listPartyReq(socket_t *socket);
aotp_response_t connReq(socket_t *socket, char *pseudo);

// Requete vers le serveur de jeu
aotp_response_t jouerCoupReq(client_t *client, position_t p, char origine, char destination);
aotp_response_t jouerEvolutionReq(client_t *client, position_t p, evolution_t evolution);

//serveur de jeu 
void jouerPartyHost(socket_t * jaune,socket_t* rouge );
aotp_response_t requestReady(client_t *client);

socket_t *host_se, *host_sd; // Socket d'écoute et socket de dialogue

int main(int argc, char *argv[]) {
    atexit(exitFunction);
    char *serverAddress;
    short serverPort;
    client = initClient(-1, "", CLIENT_UNKOWN, NULL);
    hostPosition = getPositionInitiale();
    position = getPositionInitiale();
    aotp_response_t response;

    // Récupération des arguments
    getServerAddress(argc, argv, &serverAddress, &serverPort);
    getPseudo(client->pseudo);
    system("clear");
    socket_t *clientSocket = connectToServer(serverAddress, serverPort);
    response = connReq(clientSocket, client->pseudo);
    handleResponse(response);

    response = listPartyReq(clientSocket);
    handleResponse(response);

    // Barre de chargement
    loadingBar();
    
    while(1){
        system("clear");
        COULEUR(RED);
        printf("\n\nBienvenue \e[%dm%s, \e[%dmvoici la liste des parties en cours !\n", GREEN, client->pseudo, RED);
        // Affichage des parties en cours
        afficherParties();
        menu();
        COULEUR(GREEN);
        char choix = fgetc(stdin);
        clearBuffer();
        COLOR_RESET;
        switch (choix){
            case '1':
                char hostIp[16];
                short hostPort;
                COULEUR(RED);
                promptHostIpPort(hostIp, &hostPort);
                response = createPartyReq(clientSocket, hostIp, hostPort);
                handleResponse(response);
                // TODO : CREER PARTIE
                break;
            case '2':
                COULEUR(RED);
                printf("------ Rejoindre une partie ------\n\n Veuillez entrer le numéro de la partie à rejoindre : ");
                COLOR_RESET;
                COULEUR(GREEN);
                party_id_t numPartie;
                scanf("%d", &numPartie);
                clearBuffer();
                handleResponse(requestJoinParty(client, numPartie));

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

void menu() {
    COULEUR(RED);
    printf("\n\nQue voulez-vous faire ?\n");
    printf("1. Créer une partie\n");
    printf("2. Rejoindre une partie\n");
    printf("3. Quitter\n");
    COLOR_RESET;
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

aotp_response_t createPartyReq(socket_t *socket, char *hostIp, short hostPort) {
    aotp_request_t *request = createRequest(AOTP_CREATE_PARTY);
    request->client_id = client->id;
    strcpy(request->pseudo, client->pseudo);
    request->action = AOTP_CREATE_PARTY;
    request->party_id = -1;
    strcpy(request->host_ip, hostIp);
    request->host_port = hostPort;
    send_data(socket, request, (serialize_t) struct2Request);
    aotp_response_t response;
    recv_data(socket, &response, (serialize_t) response2Struct);

    return response;
}

aotp_response_t connReq(socket_t *socket, char *pseudo) {
    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_CONNECT);
    strcpy(request->pseudo, pseudo);
    request->client_id = -1;
    send_data(socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(socket, &response, (serialize_t) response2Struct);

    // Vérification de la réponse
    // TODO : Remplacer par un handler de réponse
    if(response.code == AOTP_OK) {
        printf("Connexion réussie !\n");
        printf("Votre ID : %d\n", response.client_id);
        client->id = response.client_id;
        
    }
    // Libération de la mémoire
    free(request);

    return response;
}


aotp_response_t listPartyReq(socket_t *socket) {
    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_LIST_PARTIES);
    request->client_id = client->id;
    send_data(socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(socket, &response, (serialize_t) response2Struct);

    // Vérification de la réponse
    // TODO : Remplacer par un handler de réponse
    return response;
}

void *handleClient(void *arg) {
    printf("Client connecté\n");
    socket_t *sd = (socket_t *) arg;
    aotp_request_t *request = malloc(sizeof(aotp_request_t));
    char *buffer = malloc(sizeof(buffer_t));
    int stillConnected = 1;
    list_party_t *hostParty = NULL;
    addParty(&hostParty, myParty); // Ajout de la partie de l'hôte à la liste pour l'handler
    // Reception de la requete
    while(stillConnected) {
        recv_data(sd, request, (serialize_t) request2Struct);
        stillConnected = requestHandler(sd, request, &players, &hostParty, &hostPosition);
    }
    removeParty(&hostParty, myParty); // Suppression et libération de la mémoire de la liste créée pour l'handler

    // Lorsque c'est le client qui héberge on garde la socket de dialogue pour les échanges tant que le client ne quitte pas
    // La socket est fermée dans le handler de requête
    // Todo : Vérifier si le client a quitté et fermer la socket
}

void host(char *hostIp, short hostPort) {
    host_se = createListeningSocket(hostIp, hostPort, DEFAULT_AOTP_MAX_CLIENTS);
    hostPosition = getPositionInitiale();
    
    while(1) {
        host_sd = acceptClient(host_se);
        pthread_t thread; 
        // Create a new thread for each client
        pthread_create(&thread, NULL, handleClient, (void *) host_sd);
        pthread_detach(thread);
    }
    // Fermeture de la socket
    freeSocket(host_se);
}

aotp_response_t jouerCoupReq(client_t *client, position_t p, char origine, char destination){
    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_SEND_MOVE);
    request->client_id = client->id;
    request->coup->origine = origine;
    request->coup->destination = destination;
    send_data(client->socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(client->socket, &response, (serialize_t) response2Struct);

    // Libération de la mémoire
    free(request);

    return response;
}

aotp_response_t jouerEvolutionReq(client_t *client, position_t p, evolution_t evolution) {
    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_SEND_EVOLUTION);
    request->client_id = client->id;
    request->evolution = &evolution;
    send_data(client->socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(client->socket, &response, (serialize_t) response2Struct);

    // Libération de la mémoire
    free(request);

    return response;
}

aotp_response_t requestJoinParty(client_t *client, party_id_t partyId) {
    // Récupération de la partie
    party_t *party = getPartyById(parties, partyId);
    if(party == NULL) {
        COULEUR(RED);
        printf("ERREUR : La partie n'existe pas !\n");
        COLOR_RESET;
        return;
    }
    // Création de la socket
    client->socket = connectToServer(party->host_ip, party->host_port);

    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_JOIN_PARTY);
    request->client_id = client->id;
    send_data(client->socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(client->socket, &response, (serialize_t) response2Struct);
    return response;
}

/**
 * \fn void playGame(client_t *client)
 * \brief Fonction de déroulement du jeu pour un client
 * \param client Informations du client
 * 
 * Cette fonction fait une requête pour connaitre l'état de la partie
 * Si l'état est WAITING, on demande au client s'il est prêt
 * Si l'état est PLAYING, le client est spectateur et se mets dans une boucle d'attente de la position
*/
void joinGame(client_t *client, party_state_t state) {
    system("clear");
    switch (state) {
        case PARTY_PLAYING: // Si la partie est en cours forcement le client est spectateur
            // Si la partie est en cours, le client est spectateur et donc recevra la position
            gameLoop(client, CLIENT_SPECTATOR);
            break;

        case PARTY_WAITING: // Si la partie est en attente de joueurs 
            COULEUR(RED);
            printf("\tLa partie est en attente de joueurs, êtes-vous prêt ? (o/n)\n");
            COLOR_RESET;
            COULEUR(GREEN);
            char choix;
            scanf("%c", &choix);
            clearBuffer();
            COLOR_RESET;
            if(choix == 'o') {
                handleResponse(requestReady(client));
            }
            // On attend la réponse PARTY_STARTED
            aotp_response_t response;
            recv_data(client->socket, &response, (serialize_t) response2Struct);
            handleResponse(response);
            break;
        
        case PARTY_FINISHED:
            COULEUR(RED);
            printf("La partie est terminée !\n");
            COLOR_RESET;
            break;
    }
}

void promptHostIpPort(char *hostIp, short *hostPort) {
    COULEUR(RED);
    printf("Veuillez entrer l'adresse IP de votre machine : ");
    COLOR_RESET;
    COULEUR(GREEN);
    fgets(hostIp, 16, stdin);
    COLOR_RESET;
    COULEUR(RED);
    printf("Veuillez entrer le port sur lequel vous souhaitez héberger la partie : ");
    COLOR_RESET;
    COULEUR(GREEN);
    scanf("%hd", hostPort);
    COLOR_RESET;
}

void exitFunction() {
    // Envoi de la requête de déconnexion au serveur d'enregistrement
    
    // Libération de la mémoire
}


int handleResponse(aotp_response_t response_data) {
    printf("Code de la réponse : %d\n", response_data.code);
    switch (response_data.code) {
        case AOTP_OK:
            // Traitement pour les réponses OK
            return 1;

        case AOTP_CONN_OK:
            client->id = response_data.client_id;
            return 1;

        case AOTP_PARTY_CREATED:
            myParty = response_data.parties->party;
            client->state = response_data.client_state;
            // TODO : remplacer par un thread pour que l'host puisse continuer à jouer
            host(myParty->host_ip, myParty->host_port);
            return 1;
        case AOTP_PARTY_JOINED:
            // On change l'état du client
            client->state = response_data.client_state;
            joinGame(client, response_data.parties->party->state);
            return 1;
        
        case AOTP_PARTY_LIST_RETREIVED:
            if(parties != NULL) {
                // TODO : Libérer la mémoire de la liste des parties
            }
            parties = response_data.parties;
            return 1;

        case AOTP_PARTY_STATE_UPDATED:
            return 1;

        case AOTP_PLAYER_STATE_UPDATED:
            // On change l'état du client
            client->state = response_data.client_state;
            COULEUR(RED);
            printf("Vous êtes prêt !\n");
            COLOR_RESET;
            return 1;

        case AOTP_PARTY_STARTED:
            //client->state = response_data.client_state;
            // On récupère l'état du joueur
            client_state_t state;
            // On récupère la position
            position = *(response_data.position);
            afficherPosition(position);
            // Ecriture de la position dans le fichier json
            writePosition(position);
            gameLoop(client, state);
            return 1;

            case AOTP_POSITION_UPDATED:
                // TODO : Mettre à jour la position
                writePosition(*(response_data.position));
                position = *(response_data.position);
                free(response_data.position);
                return 1;
        default:
            // Traitement pour les autres types de réponses non gérées
            printf("Réponse non gérée : %d\n", response_data.code);
            break;
    }

    return 0;

}
/**
 * \fn jouerPartyHost(socket_t * jaune,socket_t* rouge )
 * \brief joue une partie en tant qu'hôte
 * \param jaune Socket du joueur jaune
 * \param rouge Socket du joueur rouge
*/
void jouerPartyHost(socket_t * jaune,socket_t* rouge ) {

}

aotp_response_t requestReady(client_t *client) {
    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_SET_READY);
    request->client_id = client->id;
    send_data(client->socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(client->socket, &response, (serialize_t) response2Struct);
    
    free(request);
    return response;
}

void gameLoop(client_t *client, client_state_t state) {
    // TODO : Boucle de jeu
    COULEUR(RED);
    switch (state) {
        case CLIENT_TRAIT_RED:
            printf("Vous êtes ROUGE\n");
            break;
        case CLIENT_TRAIT_YELLOW:
            printf("Vous êtes JAUNE\n");
            break;
        case CLIENT_SPECTATOR:
            printf("Vous êtes SPECTATEUR\n");
            break;
        default:
            break;
    }
    COLOR_RESET;
    COULEUR(BLUE);
    printf("Pour suivre la partie, veuillez ouvrir la page suivante : \n");
    printf("web/avalam.html\n");
    aotp_response_t response;
    int successResponse = 0;
    while(1) {
        successResponse = handleResponse(response);
        if(client->state == position.trait) {
            char origine, destination;
            // TODO : Demander le coup au joueur
            if(position.numCoup <= 3) {
                evolution_t evolution = promptEvolution(position.numCoup);
                response = jouerEvolutionReq(client, position, evolution);
            }
            else {
                COULEUR(RED);
                printf("Veuillez entrer le numéro de la case d'origine : ");
                COLOR_RESET;
                COULEUR(GREEN);
                scanf("%c", &origine);
                clearBuffer();
                COULEUR(RED);
                printf("Veuillez entrer le numéro de la case de destination : ");
                COLOR_RESET;
                COULEUR(GREEN);
                scanf("%c", &destination);
                clearBuffer();
                COLOR_RESET;
                aotp_response_t response = jouerCoupReq(client, position, origine, destination);
            }
        }else {
            recv_data(client->socket, &response, (serialize_t) response2Struct);
        }
    }
}



evolution_t promptEvolution(int numCoup) {
    evolution_t evolution = {0, 0, 0, 0};
    COULEUR(RED);
    switch (numCoup) {
        case 0:
            printf("Veuillez choisir le bonus jaune : ");
            scanf("%c", &position.evolution.bonusJ);
        break;

        case 1:
            printf("Veuillez choisir le bonus rouge : ");
            scanf("%c", &position.evolution.malusJ);
        break;

        case 2:
            printf("Veuillez choisir le malus rouge : ");
            scanf("%c", &position.evolution.bonusR);
        break;

        case 3:
            printf("Veuillez choisir le malus jaune : ");
            scanf("%c", &position.evolution.malusR);
        break;
    }
    COLOR_RESET;
    return evolution;
}