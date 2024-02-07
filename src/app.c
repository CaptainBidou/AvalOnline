/* ------------------------------------------------------------------------ */
/*                   E N T Ê T E S    S T A N D A R D S                     */
/* ------------------------------------------------------------------------ */
#include "aotp.h"
#include "mysyscall.h"
#include "design.h"
#include "req.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* ------------------------------------------------------------------------ */
/*                   v A R I A B L E S  G L O B A L E S                     */
/* ------------------------------------------------------------------------ */
///  Variables en tant que host
party_t *myParty = NULL; // Informations de la partie de l'utilisateur
list_client_t *players = NULL; // Liste des joueurs connectés
position_t hostPosition;
sem_t semHostJoin; // Sémaphore pour que l'host attende qu'il est lancé son serveur avant de faire sa requête de join

//client
char *serverAddress;
short serverPort;
client_t *client = NULL; // Informations du client
list_party_t *parties = NULL; // Liste des parties en cours
position_t position; // Position actuelle du client



socket_t *host_se= NULL , *host_sd =NULL; // Socket d'écoute et socket de dialogue

/* ------------------------------------------------------------------------ */
/*   P R O T O T Y P E S   D E   F O N C T I O N S  S T A N D A R D S       */
/* ------------------------------------------------------------------------ */
void exitFunction(); // Fonction de sortie


/* ------------------------------------------------------------------------ */
/*   P R O T O T Y P E S   D E   F O N C T I O N S  J E U X  C L I E N T    */
/* ------------------------------------------------------------------------ */

/**
 * \fn void playGame(client_t *client)
 * \brief Fonction de déroulement du jeu pour un client
 * \param client Informations du client
 * 
 * Cette fonction fait une requête pour connaitre l'état de la partie
 * Si l'état est WAITING, on demande au client s'il est prêt
 * Si l'état est PLAYING, le client est spectateur et se mets dans une boucle d'attente de la position
*/
void joinGame(client_t *client, party_state_t state);

/**
 * \fn void playGame(client_t *client)
 * \brief Fonction de déroulement du jeu pour un client
 * \param client Informations du client
 * 
 * Cette fonction fait une requête pour connaitre l'état de la partie
 * Si l'état est WAITING, on demande au client s'il est prêt
 * Si l'état est PLAYING, le client est spectateur et se mets dans une boucle d'attente de la position
*/
void joinGame(client_t *client, party_state_t state);

/**
* \fn void gameLoop(client_t *client, client_state_t state)
* \brief Boucle de jeu
* \param client Informations du client
* \param state Etat du client
*/
void gameLoop(client_t *client, client_state_t state);

/* ------------------------------------------------------------------------------ */
/*   P R O T O T Y P E S   D E   F O N C T I O N S  R E P  T O  C L I E N T       */
/* ------------------------------------------------------------------------------ */
/**
* \fn int handleResponse(aotp_response_t response_data)
* \brief Gestion des réponses
* \param response_data Réponse du serveur
* \return 1 si la réponse a été gérée, 0 sinon
*/
int handleResponse(aotp_response_t response_data);



/* --------------------------------------------------------------*/
/*   P R O T O T Y P E S   D E   F O N C T I O N S  H O S T      */
/* --------------------------------------------------------------*/

// Thread du serveur d'hébergement
/**
* \fn void host(char *hostIp, short hostPort)
* \brief Héberge une partie
* \param hostIp Adresse IP de l'hôte
* \param hostPort Port de l'hôte
*/
void host(party_t *myParty);




int main(int argc, char *argv[]) {
    install_signal_handler(SIGINT, exitFunction, 0);
    atexit(exitFunction);
    client = initClient(-1, "", CLIENT_UNKOWN, NULL);
    hostPosition = getPositionInitiale();
    position = getPositionInitiale();
    aotp_response_t response;

    // Récupération des arguments
    getServerAddress(argc, argv, &serverAddress, &serverPort);
    getPseudo(client->pseudo);
    //system("clear");
    socket_t *clientSocket = connectToServer(serverAddress, serverPort);
    response = connReq(client->pseudo, serverAddress, serverPort, client);
    handleResponse(response);

    response = listPartyReq(serverAddress, serverPort, client);
    handleResponse(response);

    // Barre de chargement
    //loadingBar();
    
    while(1){
        //system("clear");
        COULEUR(RED);
        printf("\n\nBienvenue \e[%dm%s, \e[%dmvoici la liste des parties en cours !\n", GREEN, client->pseudo, RED);
        // Affichage des parties en cours
        afficherParties(parties);
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
                response = createPartyReq(hostIp, hostPort,serverAddress, serverPort, client);
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
                handleResponse(requestJoinParty(client, numPartie, parties));

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
    CHECK(sem_init(&semHostJoin, 0, 0), "_SEM_INIT : semHostJoin");
    sem_post(&semHostJoin);
    printf("Serveur hébergé sur %s:%d\n", myParty->host_ip, myParty->host_port);
    while(1) {
        host_sd = acceptClient(host_se);
        pthread_t thread; 
        // Create a new thread for each client
        pthread_create(&thread, NULL, handleClient, (void *) host_sd);
        pthread_detach(thread);
    }
    // Fermeture de la socket
    freeSocket(host_se);
    sem_close(&semHostJoin);
}



/**
* \fn void exitFunction()
* \brief Fonction de sortie
*/
void exitFunction() {
    // Envoi de la requête de déconnexion au serveur d'enregistrement
    
    //je supprime les socket
    if(client != NULL) {
        freeSocket(client->socket);
        free(client);
    }
    freeSocket(host_se);
    printf("Fermeture du programme\n");
}

/**
* \fn int handleResponse(aotp_response_t response_data)
* \brief Gestion des réponses
* \param response_data Réponse du serveur
* \return 1 si la réponse a été gérée, 0 sinon
*/
int handleResponse(aotp_response_t response_data) {
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
            
            /*
            pthread_t threadHost;
            pthread_create(&threadHost, NULL, (void *)host, myParty);
            printf("Connexion à la partie en cours... %s %d\n", myParty->host_ip, myParty->host_port);

            sem_wait(&semHostJoin); // Attente que le serveur soit lancé
            printf("Serveur lancé\n");
            client->socket = connectToServer(myParty->host_ip, myParty->host_port);
            handleResponse(requestJoinParty(client,myParty->id, parties));
            */
            host(myParty);


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

            case AOTP_PARTY_FINISHED:
                // TODO : Afficher le gagnant
                return 1;

        default:
            // Traitement pour les autres types de réponses non gérées
            printf("Réponse non gérée : %d\n", response_data.code);
            break;
    }
    return 0;
}



/**
* \fn void gameLoop(client_t *client, client_state_t state)
* \brief Boucle de jeu
* \param client Informations du client
* \param state Etat du client
*/
void gameLoop(client_t *client, client_state_t state) {
    // TODO : Boucle de jeu
    showClientState(state);
    aotp_response_t response;
    int successResponse = 0;
    int origine, destination;
    while((getCoupsLegaux(position)).nb!=0) {
        printf("[DEBUG] Trait : %d State : %d\n", position.trait, state);
        if(state == position.trait) {
            if(position.numCoup <= 3) {
                evolution_t evolution = promptEvolution(position.numCoup, position);
                response = jouerEvolutionReq(client, position, evolution);
            }
            else {
                coup_t coup = {0, 0};
                do {
                    COULEUR(RED);
                    printf("Veuillez entrer le numéro de la case d'origine : ");
                    COLOR_RESET;
                    COULEUR(GREEN);
                    scanf("%d", &origine);
                    clearBuffer();
                    COULEUR(RED);
                    printf("Veuillez entrer le numéro de la case de destination : ");
                    COLOR_RESET;
                    COULEUR(GREEN);
                    scanf("%d", &destination);
                    clearBuffer();
                    COLOR_RESET;
                    coup.origine = (char) origine;
                    coup.destination = (char) destination;
                } while(!estValide(position, origine, destination));

                aotp_response_t response = jouerCoupReq(client, position, coup);
            }
        }else {
            recv_data(client->socket, &response, (serialize_t) response2Struct);
        }

        // La partie est terminée si le code de la réponse est AOTP_PARTY_FINISHED
        successResponse = handleResponse(response);
    }
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
    //system("clear");
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
            
            // On attend la réponse PARTY_STARTED
            //pthread_t * threadAffichage;
            //pthread_create(threadAffichage, NULL, afficherEnAttente, "En attente de la partie");

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
