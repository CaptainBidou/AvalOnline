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


list_party_t *parties = NULL; // Liste des parties en cours
party_t *myParty = NULL; // Informations de la partie de l'utilisateur

list_client_t *players = NULL; // Liste des joueurs connectés
client_t *client = NULL; // Informations du client
void createPartyReq(char *serverAddress, short serverPort, char *hostIp, short hostPort);

// Fonctions propre à l'interface
void clearBuffer();
void menu();
void loadingBar();
void getPseudo(char *pseudo);
void afficherParties();
void promptHostIpPort(char *hostIp, short *hostPort);
void playGame(client_t *client, party_state_t state);

// Thread du serveur d'hébergement
void host(char *hostIp, short hostPort);
// Requete vers le serveur d'enregistrement
void requestJoinParty(client_t *client, party_id_t partyId);
list_party_t* listPartyReq(char *pseudo,char *serverAddress, short serverPort);
void connReq(char *pseudo, char *serverAddress, short serverPort);

// Requete vers le serveur de jeu
position_t* jouerCoupReq(client_t *client, position_t p, char origine, char destination);
position_t* jouerEvolutionReq(client_t *client, position_t p, char origine, char destination);

//serveur de jeu 
void jouerPartyHost(socket_t * jaune,socket_t* rouge );

socket_t *host_se, *host_sd; // Socket d'écoute et socket de dialogue

int main(int argc, char *argv[]) {
    char *serverAddress;
    short serverPort;
    client = initClient(-1, "", CLIENT_UNKOWN, NULL);

    // Récupération des arguments
    getServerAddress(argc, argv, &serverAddress, &serverPort);
    getPseudo(client->pseudo);
    system("clear");
    connReq(client->pseudo, serverAddress, serverPort);

    parties = listPartyReq(client->pseudo,serverAddress,serverPort);

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
                createPartyReq(serverAddress, serverPort, hostIp, hostPort);

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
                printf("Num partie : %d\n", numPartie);
                requestJoinParty(client, numPartie);
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
    myParty = response->parties->party;
    if(response->code == AOTP_OK) {
        printf("Partie créée !\n");
        // Creation du thread d'hote
        host(hostIp, hostPort);
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

void *handleClient(void *arg) {
    printf("Client connecté\n");
    socket_t *sd = (socket_t *) arg;
    aotp_request_t *request = malloc(sizeof(aotp_request_t));
    char *buffer = malloc(sizeof(buffer_t));
    int stillConnected = 1;
    // Reception de la requete
    while(stillConnected) {
        recv_data(sd, request, (serialize_t) request2Struct);
        stillConnected = requestHandler(sd, request, &players, &parties);
    }

    // Lorsque c'est le client qui héberge on garde la socket de dialogue pour les échanges tant que le client ne quitte pas
    // La socket est fermée dans le handler de requête
    // Todo : Vérifier si le client a quitté et fermer la socket
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
    // Fermeture de la socket
    freeSocket(host_se);
}

position_t* jouerCoupReq(client_t *client, position_t p, char origine, char destination){
    p = jouerCoup(p,origine,destination);

    // Création de la socket
    socket_t *socket = client->socket;
    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_SEND_MOVE);
    request->client_id = client->id;
    request->coup->destination = destination;
    request->coup->origine = origine;
    send_data(socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t *response = malloc(sizeof(aotp_response_t));
    recv_data(socket, response, (serialize_t) response2Struct);

    // Vérification de la réponse
    // TODO : Remplacer par un handler de réponse
    
    //afficher les parties de la liste

    position_t* tmp = response->position;

    // Libération de la mémoire
    
    free(request);
    freeSocket(socket);
    free(response);

    return tmp;
    
}

position_t* jouerEvolutionReq(client_t *client, position_t p, char origine, char destination){

    p = jouerCoup(p,origine,destination);
    // Création de la socket
    socket_t *socket = client->socket;

    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_SEND_MOVE);
    request->client_id = client->id;
    request->coup->destination = destination;
    request->coup->origine = origine;
    send_data(socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t *response = malloc(sizeof(aotp_response_t));
    recv_data(socket, response, (serialize_t) response2Struct);

    // Vérification de la réponse
    // TODO : Remplacer par un handler de réponse
    
    //afficher les parties de la liste
    position_t* tmp = response->position;

    // Libération de la mémoire
    
    free(request);
    free(response);

    return tmp;
    
}

void requestJoinParty(client_t *client, party_id_t partyId) {
    // Récupération de la partie
    party_t *party = getPartyById(parties, partyId);
    // Création de la socket
    client->socket = connectToServer(party->host_ip, party->host_port);

    // Envoi de la requête de connexion
    aotp_request_t *request = createRequest(AOTP_JOIN_PARTY);
    request->client_id = client->id;
    send_data(client->socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t *response = malloc(sizeof(aotp_response_t));
    recv_data(client->socket, response, (serialize_t) response2Struct);

    // Vérification de la réponse
    // TODO : Remplacer par un handler de réponse
    if(response->code == AOTP_OK) {
        printf("Connexion à la partie réussie !\n");
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
void playGame(client_t *client, party_state_t state) {
    system("clear");
    switch (state) {
        case PARTY_PLAYING:
            COULEUR(RED);
            printf("La partie est en cours ! Vous êtes spectateur\n");
            // TODO : Spectateur à mettre dans une fonction
            position_t *position = malloc(sizeof(position_t));
            *position = getPositionInitiale();
            while(getCoupsLegaux(*position).nb > 0) {
                // On attend la position du serveur d'hébergement
            }
            break;
        
        case PARTY_WAITING:
            COULEUR(RED);
            printf("La partie est en attente de joueurs, êtes-vous prêt ? (o/n)\n");
            COLOR_RESET;
            COULEUR(GREEN);
            char choix;
            scanf("%c", &choix);
            clearBuffer();
            COLOR_RESET;
            if(choix == 'o') {
                // TODO : Prêt
            }
            // Mode spectateur 
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
/**
 * \fn jouerPartyHost(socket_t * jaune,socket_t* rouge )
 * \brief joue une partie en tant qu'hôte
 * \param jaune Socket du joueur jaune
 * \param rouge Socket du joueur rouge
*/
void jouerPartyHost(socket_t * jaune,socket_t* rouge ) {
    // pas oublier : les jaunes ont le trait en premier
    char numCase;
    position_t p;
    coup_t coup;
    score_t s;

    //TODO : Req pour dire au jaune qu'il choisis son evolution bonus 
    //char numCase = getJoueurBonus(jaune);
    placerEvolutionPionParPion(numCase,p.evolution.bonusJ);
    //TODO : req pour envoyer la position au jaune 
    //send_position(jaune,p);
    //TODO : req pour envoyer la position au rouge
    //send_position(rouge,p);

    //TODO : Req pour dire au rouge qu'il choisis son evolution malus
    //numCase = getJoueurMalus(rouge);
    placerEvolutionPionParPion(numCase,p.evolution.malusR);
    //TODO : req pour envoyer la position au jaune 
    //send_position(jaune,p);
    //TODO : req pour envoyer la position au rouge
    //send_position(rouge,p);

 
    //TODO : req pour dire au jaune qu'il choisis son evolution malus
    //numCase = getJoueurMalus(jaune);
    placerEvolutionPionParPion(numCase,p.evolution.malusJ);
    //TODO : req pour envoyer la position au jaune 
    //send_position(jaune,p);
    //TODO : req pour envoyer la position au rouge
    //send_position(rouge,p);

    //TODO : req pour dire au rouge qu'il choisis son evolution bonus
    //numCase = getJoueurBonus(rouge);
    placerEvolutionPionParPion(numCase,p.evolution.bonusR);
    //TODO : req pour envoyer la position au jaune 
    //send_position(jaune,p);
    //TODO : req pour envoyer la position au rouge
    //send_position(rouge,p);

    while(getCoupsLegaux(p).nb > 0){

        //TODO : req pour dire au jaune de jouer son coup
        //send_trait(jaune);
        //TODO : attendre la réponse du jaune 
        //coup = getCoup(jaune);
        p= jouerCoup(p,coup.origine,coup.destination);

        //TODO : req pour envoyer la position au jaune
        //send_position(jaune,p);
        //TODO : req pour envoyer la position au rouge
        //send_position(rouge,p);

        //TODO : req pour dire au rouge de jouer son coup
        //send_trait(rouge);
        //TODO : attendre la réponse du rouge
        //coup = getCoup(rouge);
        p= jouerCoup(p,coup.origine,coup.destination);

        //TODO : req pour envoyer la position au jaune
        //send_position(jaune,p);
        //TODO : req pour envoyer la position au rouge
        //send_position(rouge,p);

        //on imagine que l'évaluation du score est faite par les clients à chaque étape
    }
    //Partie terminée
    
    //TODO : req pour prévenenir le rouge que la partie est terminée
    //send_fin(rouge);
    //TODO : req pour prévenenir le jaune que la partie est terminée 
    //send_fin(jaune);
    
    //TODO : attendre la réponse du client pour savoir si il veut rejouer ou non
    //char choix = getChoix(jaune);

    //TODO : en fonction du choix du client on recommence ou on arrête
    //if(choix == 'o') jouerPartyHost(jaune,rouge);
    //else{tuerServeur(); }
    


}