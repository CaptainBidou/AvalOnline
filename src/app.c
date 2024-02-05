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
char *serverAddress;
short serverPort;


// Fonctions propre à l'interface
void clearBuffer();
void menu();
void loadingBar();
void afficherEnAttente(char * message);
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
void host(party_t *myParty);

// Requete vers le serveur d'enregistrement
aotp_response_t createPartyReq(char *hostIp, short hostPort);
aotp_response_t listPartyReq();
aotp_response_t connReq(char *pseudo);

// Requete vers le serveur de jeu
aotp_response_t jouerCoupReq(client_t *client, position_t p, char origine, char destination);
aotp_response_t jouerEvolutionReq(client_t *client, position_t p, evolution_t evolution);
aotp_response_t requestJoinParty(client_t *client, party_id_t partyId);

//serveur de jeu 
void jouerPartyHost(socket_t * jaune,socket_t* rouge );
aotp_response_t requestReady(client_t *client);

socket_t *host_se=NULL , *host_sd =NULL; // Socket d'écoute et socket de dialogue

int main(int argc, char *argv[]) {
    atexit(exitFunction);
    client = initClient(-1, "", CLIENT_UNKOWN, NULL);
    hostPosition = getPositionInitiale();
    position = getPositionInitiale();
    aotp_response_t response;

    // Récupération des arguments
    getServerAddress(argc, argv, &serverAddress, &serverPort);
    getPseudo(client->pseudo);
    system("clear");
    socket_t *clientSocket = connectToServer(serverAddress, serverPort);
    response = connReq(client->pseudo);
    handleResponse(response);

    response = listPartyReq();
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
                response = createPartyReq(hostIp, hostPort);
                handleResponse(response);
                
                //TODO : jouer la partie 
                
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

/**
* \fn void clearBuffer()
* \brief Vide le buffer
*/
void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/**
* \fn void menu()
* \brief Affiche le menu client
*/
void menu() {
    COULEUR(RED);
    printf("\n\nQue voulez-vous faire ?\n");
    printf("1. Créer une partie\n");
    printf("2. Rejoindre une partie\n");
    printf("3. Quitter\n");
    COLOR_RESET;
}

/**
* \fn void loadingBar()
* \brief Affiche une barre de chargement totalement inutile
*/
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

/**
* \fn void getPseudo(char *pseudo)
* \brief Demande le pseudo du client
* \param pseudo Pseudo du client
*/
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

/**
* \fn void afficherParties()
* \brief Affiche les parties en cours
*/
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

/**
* \fn void createPartyReq(char *hostIp, short hostPort)
* \brief Crée une partie
* \param hostIp Adresse IP de l'hôte
* \param hostPort Port de l'hôte
* \return Réponse du serveur
*/
aotp_response_t createPartyReq(char *hostIp, short hostPort) {
    socket_t *socket = connectToServer(serverAddress, serverPort);
    
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
    free(request);
    free(socket);
    return response;
}

/**
* \fn aotp_response_t connReq(char *pseudo)
* \brief Requête de connexion
* \param pseudo Pseudo du client
* \return Réponse du serveur
*/

aotp_response_t connReq(char *pseudo) {
    socket_t *socket = connectToServer(serverAddress, serverPort);
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
    free(socket);

    return response;
}

/**
* \fn aotp_response_t listPartyReq()
* \brief Requête de liste des parties
* \return Réponse du serveur
*/
aotp_response_t listPartyReq() {
    // Envoi de la requête de connexion
    socket_t *socket = connectToServer(serverAddress, serverPort);
    aotp_request_t *request = createRequest(AOTP_LIST_PARTIES);
    request->client_id = client->id;
    send_data(socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(socket, &response, (serialize_t) response2Struct);

    // Libération de la mémoire
    free(request);
    free(socket);
    
    return response;
}

/**
* \fn void *handleClient(void *arg)
* \brief Gestion des clients 
* \param arg Argument
*/
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

/**
* \fn void host(char *hostIp, short hostPort)
* \brief Héberge une partie
* \param hostIp Adresse IP de l'hôte
* \param hostPort Port de l'hôte
*/
void host(party_t *myParty) {
    host_se = createListeningSocket(myParty->host_ip, myParty->host_port, DEFAULT_AOTP_MAX_CLIENTS);
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

/**
* \fn aotp_response_t jouerCoupReq(client_t *client, position_t p, char origine, char destination)
* \brief Requête pour jouer un coup classique
* \param client Informations du client
* \param p Position actuelle
* \param origine Case d'origine
* \param destination Case de destination
* \return Réponse du serveur
*/
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

/**
* \fn aotp_response_t jouerEvolutionReq(client_t *client, position_t p, evolution_t evolution)
* \brief Requête pour jouer un coup évolution
* \param client Informations du client
* \param p Position actuelle
* \param evolution Evolution
* \return Réponse du serveur
*/
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

/**
* \fn aotp_response_t requestJoinParty(client_t *client, party_id_t partyId)
* \brief Requête pour rejoindre une partie
* \param client Informations du client
* \param partyId Identifiant de la partie
* \return Réponse du serveur
*/
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
            gameLoop(client, client->state);
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
            /*
            // On attend la réponse PARTY_STARTED
            pthread_t * threadAffichage;
            pthread_create(threadAffichage, NULL, afficherEnAttente, "En attente de la partie");*/

            aotp_response_t response;
            recv_data(client->socket, &response, (serialize_t) response2Struct);

            // On arrête le thread d'affichage
            //pthread_cancel(threadAffichage);


            handleResponse(response);
            break;
        
        case PARTY_FINISHED:
            COULEUR(RED);
            printf("La partie est terminée !\n");
            COLOR_RESET;
            break;
    }
}

/**
* \fn void promptHostIpPort(char *hostIp, short *hostPort)
* \brief Demande à l'utilisateur l'adresse IP et le port pour héberger une partie
* \param hostIp Adresse IP de l'hôte
* \param hostPort Port de l'hôte
*/
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

/**
* \fn void exitFunction()
* \brief Fonction de sortie
*/
void exitFunction() {
    // Envoi de la requête de déconnexion au serveur d'enregistrement
    
    //je supprime les socket
    freeSocket(client->socket);
    freeSocket(host_se);

    // Libération de la mémoire
}

/**
* \fn int handleResponse(aotp_response_t response_data)
* \brief Gestion des réponses
* \param response_data Réponse du serveur
* \return 1 si la réponse a été gérée, 0 sinon
*/
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
            
            pthread_t * threadHost;
            pthread_create(threadHost, NULL, host, myParty);
            joinGame(client, response_data.parties->party->state);


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
            client_state_t state = response_data.client_state;
            // On récupère la position
            position = *(response_data.position);
            free(response_data.position);
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

/**
* \fn aotp_response_t requestReady(client_t *client)
* \brief Requête pour indiquer que le client est prêt
* \param client Informations du client
* \return Réponse du serveur
*/
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

/**
* \fn void gameLoop(client_t *client, client_state_t state)
* \brief Boucle de jeu
* \param client Informations du client
* \param state Etat du client
*/
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
    printf("[DEBUG] selectionner le fichier web/js/avalonline-%d.js\n", getpid());
    aotp_response_t response;
    int successResponse = 0;
    while(1) {
        printf("[DEBUG] Trait : %d State : %d\n", position.trait, state);
        if(state == position.trait) {
            char origine, destination;
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

        successResponse = handleResponse(response);
    }
}


/**
* \fn evolution_t promptEvolution(int numCoup)
* \brief Demande à l'utilisateur de choisir une évolution
* \param numCoup Numéro du coup
* \return Evolution
*/
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

/**
* \fn void afficherEnAttente(char * message)
* \brief Affiche un message avec une animation de chargement
* \param message Message à afficher
*/
void afficherEnAttente(char * message) {
    char animation[] = {'|', '/', '-', '\\'};
    int i = 0;
    system("clear");
    printf("\033[1;31m"); // Changement de couleur du texte en rouge
    
    while (1) {
        printf("\r%s %c ",message, animation[i]);
        fflush(stdout); // Rafraîchissement de la sortie standard
        usleep(200000); // Pause de 200 millisecondes (0.2 seconde)
        i = (i + 1) % 4; // Pour faire tourner l'animation
    }
}