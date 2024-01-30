#include "aotp.h"

/**
 * \fn void connectClientToHost(socket, requestData);
 * \brief Fonction de création d'une partie
 * \param socket Socket du client
 * \param requestData Requete de connexion du client
 */
void connectClientToHost(socket_t *socket, aotp_request_t *requestData);


/**
 * \fn party_t *createParty(socket, requestData);
 * \brief Fonction de création d'une partie
 * \param socket Socket du client
 * \param requestData Requete de connexion du client
 * \return party_t *party
 */
party_t *createParty(socket_t *socket, aotp_request_t *requestData);

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
    client_t *client = malloc(sizeof(client_t));
    clientInit(client, -1, requestData->pseudo, *socket);

    printf("Client connecté : [%d] %s\n", client->id, client->pseudo);
    // Ajout du client a la liste des clients connectes
    addClient(clients, client);

    // Creation de la reponse de connexion
    aotp_response_t *response = malloc(sizeof(aotp_response_t));
    // On remplit la réponse
    initResponse(response, AOTP_OK, NULL, NULL);
    response->client_id = client->id;
    // Envoi de la reponse
    send_data(socket, response, (serialize_t) struct2Response);

    // Liberation de la memoire
    free(response);
    free(requestData);
}

/**
 * \fn void requestHandler(socket_t *socket, aotp_request_t *requestData)
 * \brief Fonction de gestion des requetes du protocole
 * Pour chaque requete il y un traitement specifique a effectuer
*/
void requestHandler(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, list_party_t **parties) {
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
        break;

    case AOTP_CREATE_PARTY:
        // Creer une partie et informer le serveur d'enregistrement
        party_t *party = createParty(socket, requestData);

        // ajoute la partie a la liste des parties
        addParty(parties, party);

        break;

    case AOTP_JOIN_PARTY:
        // TODO : Mettre en relation le client avec l'hote de la partie
        connectClientToHost(socket, requestData);
        break;

    default:
        break;
    }
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
        // TODO : Ajouter les autres cas
        // Passe à la ligne suivante
        body = strtok_r(NULL, "\r\n", &saveptr);
    }
}

/* ------------------------------------------------------------------------ */
/*            M A N I P U L A T I O N    D E    C L I E N T S               */
/* ------------------------------------------------------------------------ */

/**
 * \fn void clientInit(client_t *client, int id, char *pseudo, socket_t socket);
 * \brief Fonction d'initialisation d'un client
 * \param client Client a initialiser
 * \param id Identifiant du client
 * \param pseudo Pseudo du client
 * \param socket Socket du client
 */
void clientInit(client_t *client, int id, char *pseudo, socket_t socket) {
    // Si l'id est -1, on genere un id unique
    if(id == -1) {
        id = generateClientId();
    }
    client->id = id;
    strcpy(client->pseudo, pseudo);
    client->socket = socket;
}

/* ------------------------------------------------------------------------ */
/*            M A N I P U L A T I O N    D E    P A R T I E S               */
/* ------------------------------------------------------------------------ */

/**
 * \fn void partyInit(party_t *party, party_id_t id, client_t *host, party_state_t state);
 * \brief Fonction d'initialisation d'une partie
 * \param party Partie a initialiser
 * \param id Identifiant de la partie
 * \param host Hote de la partie
 * \param state Etat de la partie
 */
void partyInit(party_t *party, party_id_t id, client_t *host, party_state_t state)
{
    party->id = id;
    // Récupération de l'adresse IP de l'hote
    if (host != NULL)
    {
        strcpy(party->host_ip, inet_ntoa(host->socket.localAddr.sin_addr));
        party->host_port = host->socket.localAddr.sin_port;
    }
    party->state = state;
}

/**
 * \fn void partyToString(party_t *party, char *buffer);
 * \brief Fonction de conversion d'une partie en chaine de caracteres
 * \param party Partie a convertir
 * \param buffer Chaine de caracteres resultante
 */
void partyToString(party_t *party, char *buffer)
{
    sprintf(buffer, "%d %s %d %d\r\n", party->id, party->host_ip, party->host_port, party->state);
}

/**
 * \fn void stringToParty(char *buffer, party_t *party);
 * \brief Fonction de conversion d'une chaine de caracteres en partie
 * \param buffer Chaine de caracteres a convertir
 * \param party Partie resultante
 */
void stringToParty(char *buffer, party_t *party)
{
    sscanf(buffer, "%d %s %hd %d\r\n", &party->id, party->host_ip, &party->host_port, (int *)&party->state);
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
 * \fn list_client_t *initClientList(client_t *client);
 * \brief Fonction d'initialisation d'une liste de clients
 * \param list Liste de clients a initialiser
 */
list_client_t *initClientList(client_t *client)
{
    list_client_t *list = malloc(sizeof(list_client_t));
    list->client = client;
    list->next = NULL;
    return list;
}

/**
 * \fn void addClient(list_client_t **head, client_t *client);
 * \brief Fonction d'ajout d'un client a une liste de clients
 * \param list Liste de clients
 * \param client Client a ajouter
 */
void addClient(list_client_t **head, client_t *client)
{
    list_client_t *newClient = malloc(sizeof(list_client_t));
    newClient->client = client;
    newClient->next = NULL;
    if (*head == NULL)
    {
        *head = newClient;
    }
    else
    {
        list_client_t *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newClient;
    }
}

/**
 * \fn void removeClient(list_client_t *list, client_t *client);
 * \brief Fonction de suppression d'un client d'une liste de clients
 * \param list Liste de clients
 * \param client Client a supprimer
 */
void removeClient(list_client_t **head, int client_id) {
    list_client_t *current = *head;
    list_client_t *previous = NULL;
    while (current != NULL)
    {
        if (current->client->id == client_id)
        {
            if (previous == NULL)
            {
                *head = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            break;
        }
        previous = current;
        current = current->next;
    }
}

/**
 * \fn void initResponse(aotp_response_t *response, AOTP_RESPONSE code, list_party_t *parties, position_t *position);
 * \brief Fonction d'initialisation d'une reponse
 * \param response Reponse a initialiser
 * \param code Code de retour de la reponse
 * \param parties Parties a retourner (optionnel)
 * \param position Position a retourner (optionnel)
 */
void initResponse(aotp_response_t *response, AOTP_RESPONSE code, list_party_t *parties, position_t *position)
{
    response->code = code;
    response->parties = parties;
    response->position = position;
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
    sprintf(buffer, "%d %d\r\n", response->code, response->client_id);

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
        sprintf(buffer, "%s%d %d\r\n", buffer, position->trait, position->numCoup);
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

    // Recuperation du header
    char *header = strtok_r(bufferCopy, "\r\n", &saveptr);
    sscanf(header, "%d %d", (int *)&response->code, &response->client_id);
    // Traitement des paramètres optionnels

    // Traitement des parties
    body = strtok_r(NULL, "\r\n", &saveptr);
    if(body == NULL) return;

    // Si la ligne est "PARTIES", on traite les parties
    if (strcmp(body, "PARTIES") == 0) {
        // On passe à la ligne suivante
        body = strtok_r(NULL, "\r\n", &saveptr);
        while (body != NULL) {
            // Si la ligne est "FIN_PARTIES", on a fini de traiter les parties
            if (strcmp(body, "FIN_PARTIES") == 0) break;

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
    if (strcmp(body, "POSITION") == 0) {
        // On passe à la ligne suivante
        body = strtok_r(NULL, "\r\n", &saveptr);
        // Conversion de la chaine de caractères en position
        position_t *position = malloc(sizeof(position_t));
        sscanf(body, "%hhd %hhd", &position->trait, &position->numCoup);
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
 * \fn list_party_t initPartyList(party_t *party);
 * \brief Fonction d'initialisation d'une liste de parties
 * \param list Liste de parties a initialiser
 * \param party Partie a ajouter (optionnel)
 */
list_party_t *initPartyList(party_t *party) {
    list_party_t *list = malloc(sizeof(list_party_t));
    list->party = party;
    list->next = NULL;
    return list;
}

/**
 * \fn void addParty(list_party_t **head, party_t *party);
 * \brief Fonction d'ajout d'une partie a une liste de parties
 * \param head Liste de parties
 * \param party Partie a ajouter
 */
void addParty(list_party_t **head, party_t *party) {
    list_party_t *newParty = initPartyList(party);
    if (*head == NULL) *head = newParty;
    else {
        list_party_t *current = *head;
        while (current->next != NULL) current = current->next;
        current->next = newParty;
    }
}

/**
 * \fn void removeParty(list_party_t *list, party_t *party);
 * \brief Fonction de suppression d'une partie d'une liste de parties
 * \param list Liste de parties
 * \param party Partie a supprimer
 */
void removeParty(list_party_t **list, party_t *party) {
    list_party_t *current = *list;
    list_party_t *previous = NULL;
    
    while (current != NULL) {
        if (current->party->id == party->id) {
            if (previous == NULL) *list = current->next;
            else previous->next = current->next;
            free(current);
            break;
        }
        previous = current;
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
party_t *createParty(socket_t *socket, aotp_request_t *requestData) {
    // TODO : créer la partie
    return NULL;
}

/**
 * \fn void connectClientToHost(socket, requestData);
 * \brief Fonction de création d'une partie
 * \param socket Socket du client
 * \param requestData Requete de connexion du client
 */
void connectClientToHost(socket_t *socket, aotp_request_t *requestData) {
    // TODO : connecter le client à l'hote
}

/**
 * \fn int generateClientId() 
 * \brief Fonction de generation d'un identifiant unique pour un client
 * \return Identifiant unique
 */
int generateClientId() {
    // On recupere le timestamp actuel
    time_t timestamp = time(NULL);
    // On recupere l'identifiant du processus
    int pid = getpid();

    // On concatene les deux valeurs
    int id = timestamp + pid;

    return id;
}

/*
 * \fn client_t *getClientById(list_client_t *list, int id);
 * \brief Fonction de recuperation d'un client par son identifiant
 * \param list Liste de clients
 * \param id Identifiant du client
 * \return Client correspondant a l'identifiant ou NULL
 */
client_t *getClientById(list_client_t *list, int id) {
    list_client_t *current = list;
    while(current != NULL) {
        if(current->client->id == id) {
            return current->client;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * \fn party_t *getPartyById(list_party_t *list, party_id_t id);
 * \brief Fonction de recuperation d'une partie par son identifiant
 * \param list Liste de parties
 * \param id Identifiant de la partie
 * \return Partie correspondante a l'identifiant ou NULL
 */
party_t *getPartyById(list_party_t *list, party_id_t id) {
    list_party_t *current = list;
    while(current != NULL) {
        if(current->party->id == id) {
            return current->party;
        }
        current = current->next;
    }
    return NULL;
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
 * \fn char *partyState2String(party_state_t state);
 * \brief Transforme un status de partie en chaine de caractères
 * \param state Etat de la partie
 * \return Chaine de caractères correspondant à l'état de la partie
 * \note la chaine de caractères est allouée dynamiquement, il faut donc la libérer après utilisation
 */ 
char *partyState2String(party_state_t state) {
    char *str = malloc(sizeof(char) * 20);
    switch (state)
    {
    case PARTY_PLAYING:
        strcpy(str, "PLAYING");
        break;

    case PARTY_WAITING:
        strcpy(str, "WAITING");
        break;

    case PARTY_FINISHED:
        strcpy(str, "FINISHED");
        break;
    default:
        break;
    }
    return str;
}