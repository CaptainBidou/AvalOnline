#include "aotp.h"

/**
 * \fn void requestHandler(socket_t *socket, aotp_request_t *requestData)
 * \brief Fonction de gestion des requetes du protocole
 * Pour chaque requete il y un traitement specifique a effectuer
*/
void requestHandler(socket_t *socket, aotp_request_t *requestData) {
    AOTP_REQUEST action = requestData->action;
    switch (action) {
    case AOTP_CONNECT:
        // TODO : Ajouter le client a la liste des clients connectes
        break;
    
    case AOTP_DISCONNECT:
        // TODO : Retirer le client de la liste des clients connectes
        break;

    case AOTP_CREATE_PARTY:
        // TODO : Creer une partie et informer le serveur d'enregistrement
        break;

    case AOTP_JOIN_PARTY:
        // TODO : Le serveur d'enregistrement mettre en relation le client avec l'hote de la partie

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
void struct2Request(aotp_request_t *request, char *buffer) {
    // On commence par écrire le header de la requête
    sprintf(buffer, "%d\r\n", request->action);

    // On écrit ensuite le body de la requête

    // écriture du client
    if (request->client_id != 0) {
        sprintf(buffer, "%sclient_t %hd %s\r\n", buffer, request->client_id, request->pseudo);
    }

    // écriture de l'id de la partie
    if (request->party_id != 0) {
        sprintf(buffer, "%sparty_id_t %d\r\n", buffer, request->party_id);
    }

    // TODO : Ajouter les autres cas
}

/**
 * \fn void request2Struct(char *buffer, aotp_request_t *request);
 * \brief Fonction de conversion d'une requete en structure
 * \param request la structure resultante de la conversion
 * \param buffer requete a convertir
*/
void request2Struct(char *buffer, aotp_request_t *request) {
    // Copie du buffer dans une variable locale
    char *bufferCopy = malloc(strlen(buffer) * sizeof(char));
    strcpy(bufferCopy, buffer);

    // Recuperation du header
    char *header = strtok(bufferCopy, AOTP_EMPTY_LINE);
    sscanf(header, "%d:%20s", &request->action, request->pseudo);

    // Traitement du body
    char *body = strtok(NULL, AOTP_EMPTY_LINE);

    while (body != NULL && body[0] != '\0') {
        if (strncmp(body, "client_t", strlen("client_t")) == 0) {
            sscanf(body, "client_t %hd %20s", &request->client_id, request->pseudo);
        }

        if (strncmp(body, "party_id_t", strlen("party_id_t")) == 0) {
            sscanf(body, "party_id_t %d", &request->party_id);
        }

        // TODO : Ajouter les autres cas 
        // Passe à la ligne suivante
        body = strtok(NULL, "\r\n");
    }
}

/* ------------------------------------------------------------------------ */
/*            M A N I P U L A T I O N    D E    C L I E N T S               */
/* ------------------------------------------------------------------------ */

/**
 * \fn void clientInit(client_t *client, short id, char *pseudo, socket_t socket);
 * \brief Fonction d'initialisation d'un client
 * \param client Client a initialiser
 * \param id Identifiant du client
 * \param pseudo Pseudo du client
 * \param socket Socket du client
*/
void clientInit(client_t *client, short id, char *pseudo, socket_t socket) {
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
void partyInit(party_t *party, party_id_t id, client_t *host, party_state_t state) {

}

/**
 * \fn void partyToString(party_t *party, char *buffer);
 * \brief Fonction de conversion d'une partie en chaine de caracteres
 * \param party Partie a convertir
 * \param buffer Chaine de caracteres resultante
*/
void partyToString(party_t *party, char *buffer) {

}

/**
 * \fn void stringToParty(char *buffer, party_t *party);
 * \brief Fonction de conversion d'une chaine de caracteres en partie
 * \param buffer Chaine de caracteres a convertir
 * \param party Partie resultante
*/
void stringToParty(char *buffer, party_t *party) {

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
void coupToString(coup_t *coup, char *buffer) {

}

/**
 * \fn void stringToCoup(char *buffer, coup_t *coup);
 * \brief Fonction de conversion d'une chaine de caracteres en coup
 * \param buffer Chaine de caracteres a convertir
 * \param coup Coup resultante
*/
void stringToCoup(char *buffer, coup_t *coup) {

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
void evolutionToString(evolution_t *evolution, char *buffer) {

}

/**
 * \fn void stringToEvolution(char *buffer, evolution_t *evolution);
 * \brief Fonction de conversion d'une chaine de caracteres en evolution
 * \param buffer Chaine de caracteres a convertir
 * \param evolution Evolution resultante
*/
void stringToEvolution(char *buffer, evolution_t *evolution) {

}
