#include "req.h"

/**
* \fn void createPartyReq(char *hostIp, short hostPort)
* \brief Crée une partie
* \param hostIp Adresse IP de l'hôte
* \param hostPort Port de l'hôte
* \return Réponse du serveur
*/
aotp_response_t createPartyReq(char *hostIp, short hostPort, char *serverAddress,short serverPort, client_t *client) {
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
    freeRequest(request);
    free(socket);
    return response;
}


/**
* \fn aotp_response_t listPartyReq()
* \brief Requête de liste des parties
* \return Réponse du serveur
*/
aotp_response_t listPartyReq(char *serverAddress,short serverPort, client_t *client) {
    // Envoi de la requête de connexion
    socket_t *socket = connectToServer(serverAddress, serverPort);
    aotp_request_t *request = createRequest(AOTP_LIST_PARTIES);
    request->client_id = client->id;
    send_data(socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(socket, &response, (serialize_t) response2Struct);

    // Libération de la mémoire
    freeRequest(request);
    free(socket);
    
    return response;
}

/**
* \fn aotp_response_t connReq(char *pseudo)
* \brief Requête de connexion
* \param pseudo Pseudo du client
* \return Réponse du serveur
*/
aotp_response_t connReq(char *pseudo,char *serverAddress,short serverPort, client_t *client) {
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
    freeRequest(request);
    free(socket);

    return response;
}


/**************************************************************************************************/
// Requetes vers le serveur de jeu

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
    request->coup.origine = origine;
    request->coup.destination = destination;
    send_data(client->socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(client->socket, &response, (serialize_t) response2Struct);

    // Libération de la mémoire
    freeRequest(request);

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
    request->evolution = evolution;
    send_data(client->socket, request, (serialize_t) struct2Request);

    // Réception de la réponse
    aotp_response_t response;
    recv_data(client->socket, &response, (serialize_t) response2Struct);

    // Libération de la mémoire
    freeRequest(request);

    return response;
}

/**
* \fn aotp_response_t requestJoinParty(client_t *client, party_id_t partyId)
* \brief Requête pour rejoindre une partie
* \param client Informations du client
* \param partyId Identifiant de la partie
* \return Réponse du serveur
*/
aotp_response_t requestJoinParty(client_t *client, party_id_t partyId, list_party_t* parties) {
    
    

    // Récupération de la partie
    party_t *party = getPartyById(parties, partyId);
    if(party == NULL) {
        aotp_response_t response;
        response.code = AOTP_BAD_REQUEST;
        COULEUR(RED);
        printf("ERREUR : La partie n'existe pas !\n");
        COLOR_RESET;
        return response;
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
    
    freeRequest(request);
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
/*void joinGame(client_t *client, party_state_t state) {
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
}*/
