#include "aotp.h"

party_id_t generatePartyId(list_party_t *list);

void setReady(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position);
void connectClientToHost(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, list_party_t **parties);
void playMove(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position);
void playEvolution(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position);
void sendPositionToClients(list_client_t *clients, position_t *position);


/**
* \fn void playMove(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position);
* \brief Gestion d'un coup joué par un client
* \param socket Socket du client
* \param requestData Requete du client
* \param clients Liste des clients
* \param position Position actuelle
* \note Cette fonction est appelée lorsqu'un client joue un coup
* \warning Cette fonction modifie la position actuelle
*/
void playMove(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position) {
    // On récupère le coup joué
    coup_t *coup = requestData->coup;

    // On récupère le client
    client_t *client = getClientById(*clients, requestData->client_id);
    if(client == NULL) {
        // TODO : Renvoyer une erreur au client
        sendResponse(socket, AOTP_ERR_CONNECT, NULL, NULL, 0, CLIENT_UNKOWN);
    }
    // On vérifie que le client a le trait
    if(client->state != position->trait) {
        // TODO : Renvoyer une erreur au client
        sendResponse(socket, AOTP_ERR_CONNECT, NULL, NULL, 0, client->state);
    }

    // On joue le coup
    position_t newPosition = jouerCoup(*position, coup->origine, coup->destination);
    if(newPosition.numCoup == position->numCoup) {
        sendResponse(socket, AOTP_ERR_CONNECT, NULL, NULL, 0, CLIENT_SPECTATOR);
    }

    // On envoie la nouvelle position à tous les clients
    *position = newPosition;
    sendPositionToClients(*clients, position);
}


/**
* \fn void playEvolution(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position);
* \brief Gestion d'une évolution jouée par un client
* \param socket Socket du client
* \param requestData Requete du client
* \param clients Liste des clients
* \param position Position actuelle
* \note Cette fonction est appelée lorsqu'un client joue une évolution
* \warning Cette fonction modifie la position actuelle
*/
void playEvolution(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position) {
    // On récupère l'évolution jouée
    evolution_t *evolution = requestData->evolution;

    // Nouvelle position (TODO : copyPosition)
    position_t newPosition;
    newPosition = *position;

    // CHECK : On vérifie que l'évolution est valide (TODO) crée une fonction pour ça jouerCoupEvolution
    newPosition = jouerEvolution(*position, *evolution);

    // On envoie la nouvelle position à tous les clients
    *position = newPosition; // on met à jour la position
    sendPositionToClients(*clients, position);
    
}

/**
 * \fn void sendPositionToClients(list_client_t *clients, position_t *position);
 * \brief Fonction d'envoi de la position à tous les clients
 * \param clients Liste des clients
 * \param position Position à envoyer
*/
void sendPositionToClients(list_client_t *clients, position_t *position) {
    list_client_t *current = clients;
    while(current != NULL) {
        sendResponse(current->client->socket, AOTP_POSITION_UPDATED, NULL, position, 0, current->client->state);
        current = current->next;
    }

}
/**
 * \fn party_t *createParty(socket, requestData);
 * \brief Fonction de création d'une partie
 * \param socket Socket du client
 * \param requestData Requete de connexion du client
 * \return party_t *party
 */
party_t *createParty(aotp_request_t *requestData, list_party_t **parties, list_client_t **clients);

/**
 * \fn void listPartiesRep(socket, parties, requestData->client_id);
 * \brief Renvoie la liste des parties
 * \param socket Socket du client
 * \param parties Liste des parties
 * \param client_id Identifiant du client
 * \return Chaine de caractères correspondant aux parties
 * \note la chaine de caractères est allouée dynamiquement, il faut donc la libérer après utilisation
 */ 
void listPartiesRep(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, list_party_t **parties);

/**
 * \fn void connectHandler(socket_t *socket, aotp_request_t *requestData);
 * \brief Fonction de gestion de la connexion d'un client
 * \param socket Socket du client
 * \param requestData Requete de connexion du client
 * \note Cette fonction est appelee lors de la reception d'une requete de connexion
 * \warning lors de la creation d'un client, une allocation dynamique est effectuee
*/
void connectHandler(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, list_party_t **parties) {
    // Creation d'un nouveau client
    client_t *client = initClient(generateClientId(), requestData->pseudo, CLIENT_UNKOWN, socket);
    printf("Client connecté : [%d] %s\n", client->id, client->pseudo);
    // Ajout du client a la liste des clients connectes
    addClient(clients, client);
    sendResponse(socket, AOTP_CONN_OK, NULL, NULL, client->id, client->state);
}

/**
 * \fn void requestHandler(socket_t *socket, aotp_request_t *requestData)
 * \brief Fonction de gestion des requetes du protocole
 * Pour chaque requete il y un traitement specifique a effectuer
 */
int requestHandler(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, list_party_t **parties, position_t *position) {
    AOTP_REQUEST action = requestData->action;
    switch (action)
    {
    case AOTP_CONNECT:
        //connecte le client et l'ajoute a la liste des clients
        connectHandler(socket, requestData, clients, parties);
        break;

    case AOTP_DISCONNECT:
        // supprime le client de la liste
        removeClient(clients, requestData->client_id);
        sendResponse(socket, AOTP_OK, NULL, NULL, 0, CLIENT_UNKOWN);
        return 0;
        break;

    case AOTP_CREATE_PARTY:
        // Creer une partie et informer le serveur d'enregistrement
        party_t *party = createParty(requestData, parties, clients);
        // ajoute la partie a la liste des parties
        addParty(parties, party);
        // envoie la reponse au client
        list_party_t *playerParty = NULL;
        addParty(&playerParty, party);
        sendResponse(socket, AOTP_PARTY_CREATED, playerParty, NULL, 0, CLIENT_SPECTATOR);
        removeParty(&playerParty, party);
        break;
    case AOTP_LIST_PARTIES:
        // TODO : Renvoyer la liste des parties
        listPartiesRep(socket,requestData,clients,parties);
        break;

    /**************************************************************************/
    /*                          REQUETES D'HOTE                               */
    /**************************************************************************/
    case AOTP_JOIN_PARTY:
        connectClientToHost(socket, requestData, clients, parties);
        break;

    case AOTP_SET_READY:
        // Change l'etat du client en prêt, et lance la partie si tous les clients sont prêts
        setReady(socket, requestData, clients, position);
        break;


    case AOTP_SEND_EVOLUTION:
        // TODO : Envoie l'evolution et met a jour la position à tous les clients
        playEvolution(socket, requestData, clients, position);
        break;

    case AOTP_SEND_MOVE:
        // TODO : Envoie le coup et met a jour la position à tous les clients
        playMove(socket, requestData, clients, position);
        break;

    default:
        break;
    }
    
    return 1;
}

/**
 * \fn void struct2Request(aotp_request_t *request, char *buffer);
 * \brief Fonction de conversion d'une structure en requete
 * \param request Requete a convertir
 * \param buffer Buffer de conversion
 *
 */
void struct2Request(aotp_request_t *request, char *buffer)
{
    // On commence par écrire le header de la requête
    sprintf(buffer, "%d\r\n", request->action);
    // On écrit ensuite le body de la requête

    // écriture du client
    if (request->client_id != 0) sprintf(buffer, "%sclient_t %d %s\r\n", buffer, request->client_id, request->pseudo);

    // écriture de l'id de la partie
    if (request->party_id != 0) sprintf(buffer, "%sparty_id_t %d\r\n", buffer, request->party_id);

    // écriture de l'ip de l'hote
    if (strlen(request->host_ip) > 0) sprintf(buffer, "%shost_ip %s\r\n", buffer, request->host_ip);

    // écriture du port de l'hote
    if (request->host_port != 0) sprintf(buffer, "%shost_port %d\r\n", buffer, request->host_port);

    // TODO : Ajouter les autres cas
}

/**
 * \fn void request2Struct(char *buffer, aotp_request_t *request);
 * \brief Fonction de conversion d'une requete en structure
 * \param request la structure resultante de la conversion
 * \param buffer requete a convertir
 */
void request2Struct(char *buffer, aotp_request_t *request)
{
    // Copie du buffer dans une variable locale
    char *bufferCopy = malloc(strlen(buffer) * sizeof(char));
    char *saveptr;
    strcpy(bufferCopy, buffer);

    // Recuperation du header
    char *header = strtok_r(bufferCopy, "\r\n", &saveptr);
    sscanf(header, "%d", (int *)&request->action);

    // Traitement du body
    char *body = strtok_r(NULL, "\r\n", &saveptr);

    while (body != NULL) {

        if (strncmp(body, "client_t", strlen("client_t")) == 0) sscanf(body, "client_t %d %20s", &request->client_id, request->pseudo);
        if (strncmp(body, "party_id_t", strlen("party_id_t")) == 0) sscanf(body, "party_id_t %d", &request->party_id);
        if (strncmp(body, "host_ip", strlen("host_ip")) == 0) sscanf(body, "host_ip %s", request->host_ip);
        if (strncmp(body, "host_port", strlen("host_port")) == 0) sscanf(body, "host_port %hd", &request->host_port);

        // TODO : Ajouter les autres cas
        // Passe à la ligne suivante
        body = strtok_r(NULL, "\r\n", &saveptr);
    }

    // Liberation de la memoire
    free(bufferCopy);
}



/**
 * \fn void partyToString(party_t *party, char *buffer);
 * \brief Fonction de conversion d'une partie en chaine de caracteres
 * \param party Partie a convertir
 * \param buffer Chaine de caracteres resultante
 */
void partyToString(party_t *party, char *buffer)
{
    sprintf(buffer, "%d %s %s %d %d\r\n", party->id, party->host_pseudo,party->host_ip, party->host_port, party->state);
}

/**
 * \fn void stringToParty(char *buffer, party_t *party);
 * \brief Fonction de conversion d'une chaine de caracteres en partie
 * \param buffer Chaine de caracteres a convertir
 * \param party Partie resultante
 */
void stringToParty(char *buffer, party_t *party)
{
    sscanf(buffer, "%d %s %s %hd %d\r\n", &party->id,party->host_pseudo, party->host_ip, &party->host_port, (int *)&party->state);
}

/* ------------------------------------------------------------------------ */
/*            M A N I P U L A T I O N    D E    C O U P S                   */
/* ------------------------------------------------------------------------ */

/**
 * \fn void coupToString(coup_t *coup, char *buffer);
 * \brief Fonction de conversion d'un coup en chaine de caracteres
 * \param coup Coup a convertir
 * \param buffer Chaine de caracteres resultante
 */
void coupToString(coup_t *coup, char *buffer)
{
    // Un coup est represente par deux caracteres : origine et destination
    sprintf(buffer, "%c:%c", coup->origine, coup->destination);
}

/**
 * \fn void stringToCoup(char *buffer, coup_t *coup);
 * \brief Fonction de conversion d'une chaine de caracteres en coup
 * \param buffer Chaine de caracteres a convertir
 * \param coup Coup resultante
 */
void stringToCoup(char *buffer, coup_t *coup)
{
    sscanf(buffer, "%c %c", &coup->origine, &coup->destination);
}

/* ------------------------------------------------------------------------ */
/*            M A N I P U L A T I O N    D ' E V O L U T I O N S           */
/* ------------------------------------------------------------------------ */

/**
 * \fn void evolutionToString(evolution_t *evolution, char *buffer);
 * \brief Fonction de conversion d'une evolution en chaine de caracteres
 * \param evolution Evolution a convertir
 * \param buffer Chaine de caracteres resultante
 */
void evolutionToString(evolution_t *evolution, char *buffer)
{
    sprintf(buffer, "%c %c %c %c", evolution->bonusJ, evolution->malusJ, evolution->bonusR, evolution->malusR);
}

/**
 * \fn void stringToEvolution(char *buffer, evolution_t *evolution);
 * \brief Fonction de conversion d'une chaine de caracteres en evolution
 * \param buffer Chaine de caracteres a convertir
 * \param evolution Evolution resultante
 */
void stringToEvolution(char *buffer, evolution_t *evolution)
{
    sscanf(buffer, "%c %c %c %c", &evolution->bonusJ, &evolution->malusJ, &evolution->bonusR, &evolution->malusR);
}

/**
 * \fn void initResponse(aotp_response_t *response, AOTP_RESPONSE code, list_party_t *parties, position_t *position);
 * \brief Fonction d'initialisation d'une reponse
 * \param response Reponse a initialiser
 * \param code Code de retour de la reponse
 * \param parties Parties a retourner (optionnel)
 * \param position Position a retourner (optionnel)
 */
void initResponse(aotp_response_t *response, AOTP_RESPONSE code, list_party_t *parties, position_t *position, int client_id, client_state_t client_state)
{
    response->code = code;
    response->parties = parties;
    response->position = position;
    response->client_id = client_id;
    response->client_state = client_state;
}

/**
 * \fn void struct2Response(aotp_response_t *response, char *buffer);
 * \brief Fonction de conversion d'une structure en reponse sous le format aotp
 * \param response Reponse a convertir
 * \param buffer Buffer de conversion
 */
void struct2Response(aotp_response_t *response, char *buffer)
{
    // On commence par écrire le header de la requête
    sprintf(buffer, "%d %d %d\r\n", response->code, response->client_id, response->client_state);
    
    // Ecriture de la ligne vide
    sprintf(buffer, "%sPARTIES\r\n", buffer);
    // Ecriture des parties
    // Parcours de la liste des parties
    list_party_t *current = response->parties;
    while (current != NULL) {
        // Conversion de la partie en chaine de caractères
        char *partie = malloc(sizeof(char) * 100);
        partyToString(current->party, partie);
        // Ecriture de la partie dans le buffer
        sprintf(buffer, "%s%s", buffer, partie);
        // Passage à la partie suivante
        current = current->next;
        free(partie);
    }
    // Ecriture de la ligne vide
    sprintf(buffer, "%sFIN_PARTIES\r\n", buffer);
    // Ecrire la position du joueur
    sprintf(buffer, "%sPOSITION\r\n", buffer);
    position_t *position = response->position;
    if(position != NULL) {
        sprintf(buffer, "%s%d %d %d %d %d %d\r\n", buffer, position->trait, position->numCoup, position->evolution.bonusJ, position->evolution.malusJ, position->evolution.bonusR, position->evolution.malusR);
        // Ecriture des colonnes
        for (int i = 0; i < NBCASES; i++) sprintf(buffer, "%s%d %d\r\n", buffer, position->cols[i].nb, position->cols[i].couleur);
    }
    // Ecriture de la ligne vide
    sprintf(buffer, "%sFIN_POSITION\r\n", buffer);
}

/**
 * \fn void response2Struct(char *buffer, aotp_response_t *response);
 * \brief Fonction de conversion d'une reponse sous le format aotp en structure
 * \param buffer Reponse a convertir
 * \param response Structure resultante
 *
 */
void response2Struct(char *buffer, aotp_response_t *response) {
    // Copie du buffer dans une variable locale
    char *bufferCopy = malloc(strlen(buffer) * sizeof(char));
    char *saveptr;
    char *body;
    strcpy(bufferCopy, buffer);
    response->parties = NULL;
    response->position = NULL;
    response->client_id = 0;
    response->client_state = CLIENT_UNKOWN;

    // Recuperation du header
    char *header = strtok_r(bufferCopy, "\r\n", &saveptr);
    sscanf(header, "%d %d %d", (int *)&response->code, &response->client_id, (int *)&response->client_state);
    // Traitement des paramètres optionnels

    // Traitement des parties
    body = strtok_r(NULL, "\r\n", &saveptr);
    if(body == NULL) return;

    // Si la ligne est "PARTIES", on traite les parties
    if (strncmp(body, "PARTIES", strlen("PARTIES")) == 0) {
        // On passe à la ligne suivante
        body = strtok_r(NULL, "\r\n", &saveptr);
        while (body != NULL) {
            // Si la ligne est "FIN_PARTIES", on a fini de traiter les parties
            if (strncmp(body, "FIN_PARTIES", strlen("FIN_PARTIES")) == 0) {
                body = strtok_r(NULL, "\r\n", &saveptr);
                break;
            }

            // Conversion de la chaine de caractères en partie
            party_t *party = malloc(sizeof(party_t));
            stringToParty(body, party);

            // Ajout de la partie à la liste des parties
            addParty(&response->parties, party);
            // Passe à la ligne suivante
            body = strtok_r(NULL, "\r\n", &saveptr);
        }
    }
    // Si la ligne est "POSITION", on traite la position
    if (body != NULL && strncmp(body, "POSITION", strlen("POSITION")) == 0) {
        // On passe à la ligne suivante
        body = strtok_r(NULL, "\r\n", &saveptr);

        // Si on trouve la ligne "FIN_POSITION", on a fini de traiter la position
        if (strncmp(body, "FIN_POSITION", strlen("FIN_POSITION")) == 0) {
            return;
        }
        // Conversion de la chaine de caractères en position
        position_t *position = malloc(sizeof(position_t));

        sscanf(body, "%hhd %hhd %hhd %hhd %hhd %hhd", &position->trait, &position->numCoup, &position->evolution.bonusJ, &position->evolution.malusJ, &position->evolution.bonusR, &position->evolution.malusR);
        // Traitement des colonnes
        for (int i = 0; i < NBCASES; i++) {
            body = strtok_r(NULL, "\r\n", &saveptr);
            sscanf(body, "%hhd %hhd", &position->cols[i].nb, &position->cols[i].couleur);
        }
        // Ajout de la position à la réponse
        response->position = position;
    }
}

/**
 * \fn party_t *createParty(socket, requestData);
 * \brief Fonction de création d'une partie
 * \param socket Socket du client
 * \param requestData Requete de connexion du client
 * \return party_t *party
 */
party_t *createParty(aotp_request_t *requestData, list_party_t **parties, list_client_t **clients) {
    // On genere un identifiant unique pour la partie
    party_id_t id = generatePartyId(*parties);
    printf("Creation de la partie %d\n", id);
    // On recupere le client
    client_t *host = getClientById(*clients, requestData->client_id);
    // TODO : Renvoyer une erreur si le client n'existe pas
    if(host == NULL) return NULL;

    strcpy(host->ip, requestData->host_ip);
    host->port = requestData->host_port;

    // On cree la partie
    party_t *party = partyInit(id, host, PARTY_WAITING);

    return party;
}

/**
 * \fn void connectClientToHost(socket, requestData);
 * \brief Fonction de création d'une partie
 * \param socket Socket du client
 * \param requestData Requete de connexion du client
 */
void connectClientToHost(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, list_party_t **parties) {
    client_t *client = initClient(requestData->client_id, requestData->pseudo, CLIENT_SPECTATOR, socket);
    addClient(clients, client);
    // On recupere la partie
    if(*parties == NULL) {
        sendResponse(socket, AOTP_ERR_CONNECT, NULL, NULL, 0, CLIENT_SPECTATOR);
        return;
    }
    // On recupere la partie
    list_party_t *party = *parties;
    // On envoie une réponse au client
    sendResponse(socket, AOTP_PARTY_JOINED, party, NULL, 0, CLIENT_SPECTATOR);
}




/**
 * \fn aotp_request_t *createRequest(AOTP_REQUEST action);
 * \brief Fonction de creation d'une requete
 * \param action Code de la requete
 */
aotp_request_t *createRequest(AOTP_REQUEST action) {
    aotp_request_t *request = malloc(sizeof(aotp_request_t));
    request->action = action;
    return request;
}


/**
 * \fn void listPartiesRep(socket, parties, requestData->client_id);
 * \brief Renvoie la liste des parties
 * \param socket Socket du client
 * \param parties Liste des parties
 * \param client_id Identifiant du client
 * \return Chaine de caractères correspondant aux parties
 * \note la chaine de caractères est allouée dynamiquement, il faut donc la libérer après utilisation
 */ 
void listPartiesRep(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, list_party_t **parties){

    // on verifie que le client existe 
    client_t *client = getClientById(*clients, requestData->client_id);
    if(client == NULL) {
        sendResponse(socket, AOTP_ERR_CONNECT, NULL, NULL, 0, CLIENT_UNKOWN);
        return;
    }
    // Envoie de la liste des parties
    sendResponse(socket, AOTP_PARTY_LIST_RETREIVED, *parties, NULL, 0, CLIENT_UNKOWN);
}

/**
* \fn void setReady(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position);
* \brief Change l'etat du client en prêt, et lance la partie si tous les clients sont prêts
* \param socket Socket du client
* \param requestData Requete du client
* \param clients Liste des clients
* \param position Position actuelle
* \note Cette fonction est appelée lorsqu'un client est prêt à jouer
*/
void setReady(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, position_t *position) {
    // On recupere le client
    client_t *client = getClientById(*clients, requestData->client_id);
    if(client == NULL) {
        // TODO : Renvoyer une erreur au client
        sendResponse(socket, AOTP_ERR_CONNECT, NULL, NULL, 0, CLIENT_SPECTATOR);
    }
    // On change l'etat du client
    client->state = CLIENT_READY;

    // On envoie la reponse au client
    sendResponse(socket, AOTP_PLAYER_STATE_UPDATED, NULL, NULL, 0, client->state);

    // On verifie que 2 joueurs sont prêts :
    if(is2PlayersReady(*clients)) {
        // On recupere les deux joueurs
        list_client_t *current = *clients;
        client_t *player1 = NULL;
        client_t *player2 = NULL;
        while(current != NULL) {
            if(current->client->state == CLIENT_READY) {
                if(player1 == NULL) player1 = current->client;
                else player2 = current->client;
            }
            current = current->next;
        }
        // On envoie une reponse aux deux joueurs
        // On change l'etat des joueurs pour leur donner le trait
        player1->state = CLIENT_TRAIT_RED;
        player2->state = CLIENT_TRAIT_YELLOW;

        // On envoie une réponse à chaque client pour leur donner leur trait et leur informer que la partie commence
        current = *clients;
        while(current != NULL) {
            sendResponse(current->client->socket, AOTP_PARTY_STARTED, NULL, position, client->id, current->client->state);
            current = current->next;
        }

    }

}

/**
 * \fn void sendResponse(AOTP_RESPONSE code, list_party_t *parties, position_t *position);
 * \brief Fonction d'envoi d'une reponse
 * \param code Code de retour de la reponse
 * \param parties Parties a retourner (optionnel)
 * \param position Position a retourner (optionnel)
 */
void sendResponse(socket_t *socket, AOTP_RESPONSE code, list_party_t *parties, position_t *position, int client_id, client_state_t client_state) {    // Creation de la reponse de connexion
    aotp_response_t response;
    // On remplit la réponse
    initResponse(&response, code, parties, position, client_id, client_state);
    // Envoi de la reponse
    send_data(socket, &response, (serialize_t) struct2Response);
}