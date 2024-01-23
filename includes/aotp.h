/**
 * \file proto.h
 * \brief Fichier d'en-tête de la libriairie de notre protocole AOTP (Avalam Online Transfer Protocol)
 * 
 * 
*/
#ifndef AOTP_H
#define AOTP_H

#define DEFAULT_AOTP_PORT 12345 /*!< Port par defaut du protocole */


#define AOTP_STRUCT_SEPARATOR '|' /*!< Separateur de champs de la requete */
#define AOTP_EMPTY_LINE "\r\n" /*!< Ligne vide de la requete */
#define AOTP_HEADER_SEPARATOR ':' /*!< Separateur de l'entete de la requete */

#include "session.h"
#include "avalam.h"

/* ------------------------------------------------------------------------ */
/*              D É F I N I T I O N S   D E   T Y P E S                     */
/* ------------------------------------------------------------------------ */


typedef int party_id_t; /*!< Identifiant d'une partie */

/**
 * \struct party_state_t 
 * \brief Structure de l'etat d'une partie
*/
typedef enum {
    PARTY_WAITING = 0, /*!< Partie en attente de joueurs */
    PARTY_PLAYING, /*!< Partie en cours */
    PARTY_FINISHED, /*!< Partie terminee */
} party_state_t; /*!< Structure de l'etat d'une partie */


/**
 * \enum AOTP_REQUEST
 * \brief Enumeration des requetes du protocole
 * 
*/
typedef enum {
    // Les requetes vers le serveur d'enregistrement
    AOTP_CONNECT=200, /*!< Requete de connexion */
    AOTP_DISCONNECT, /*!< Requete de deconnexion */
    AOTP_CREATE_PARTY, /*!< Requete de creation de partie */
    AOTP_JOIN_PARTY, /*!< Requete de connexion a une partie */
    AOTP_LIST_PARTIES, /*!< Requete de liste des parties */
    AOTP_UPDATE_PARTY_STATE, /*!< Requete de mise a jour de l'etat d'une partie */

    // Les requetes vers un hote de partie
    AOTP_CONNECT_TO_PARTY = 300, /*!< Requete de connexion a une partie */
    AOTP_SET_READY, /*!< Requete pour changer d'etat en pret */
    AOTP_SEND_MOVE, /*!< Requete pour envoyer un coup */
    AOTP_SEND_EVOLUTION, /*!< Requete pour envoyer un coup d'evolution */
    AOTP_LEAVE_GAME, /*!< Requete de deconnexion d'une partie */
} AOTP_REQUEST; /*!< Enumeration des requetes du protocole */

/**
 * \enum AOTP_RESPONSE
 * \brief Enumeration des codes de retour du protocole
*/
typedef enum {
    // Code de retour du serveur d'enregistrement
    AOTP_OK = 200, /*!< Code de retour OK */


    AOTP_NOK = 400, /*!< Code de retour NOK */
} AOTP_RESPONSE; /*!< Enumeration des codes de retour du protocole */

/**
 * \struct client_t
 * \brief Structure d'un client
*/
typedef struct {
    short id; /*!< Identifiant du client */
    char pseudo[20]; /*!< Pseudo du client */
    socket_t socket; /*!< Socket du client */
} client_t; /*!< Structure d'un client */

/**
 * \struct aotp_request
 * \brief Structure de requete du protocole
*/
typedef struct {
    AOTP_REQUEST action; /*!< Code de la requete */
    short client_id; /*!< Identifiant du client */
    char pseudo[20]; /*!< Pseudo du client */
    party_id_t party_id; /*!< Identifiant de la partie */
    party_state_t party_state; /*!< Etat de la partie */
    coup_t coup; /*!< Coup a jouer */
    evolution_t evolution; /*!< Evolution a jouer */
} aotp_request_t; /*!< Structure de requete du protocole */

/**
 * \struct party_t
*/
typedef struct {
    party_id_t id; /*!< Identifiant de la partie */
    char host_ip[16]; /*!< Adresse IP de l'hote */
    short host_port; /*!< Port de l'hote */
    party_state_t state; /*!< Etat de la partie */
} party_t; /*!< Structure d'une partie */

/* ------------------------------------------------------------------------ */
/*            P R O T O T Y P E S    D E    F O N C T I O N S               */
/* ------------------------------------------------------------------------ */

/**
 * \fn void requestHandler(socket_t *socket, aotp_request_t *requestData)
 * \brief Fonction de gestion des requetes du protocole
 * Pour chaque requete il y un traitement specifique a effectuer
*/
void requestHandler(socket_t *socket, aotp_request_t *requestData);

/**
 * \fn void struct2Request(aotp_request_t *request, char *buffer);
 * \brief Fonction de conversion d'une structure en requete
 * \param request Requete a convertir
 * \param buffer Buffer de conversion
 * 
*/
void struct2Request(aotp_request_t *request, char *buffer);

/**
 * \fn void request2Struct(char *buffer, aotp_request_t *request);
 * \brief Fonction de conversion d'une requete en structure
 * \param request la structure resultante de la conversion
 * \param buffer requete a convertir
*/
void request2Struct(char *buffer, aotp_request_t *request);

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
void clientInit(client_t *client, short id, char *pseudo, socket_t socket);

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
void partyInit(party_t *party, party_id_t id, client_t *host, party_state_t state);

/**
 * \fn void partyToString(party_t *party, char *buffer);
 * \brief Fonction de conversion d'une partie en chaine de caracteres
 * \param party Partie a convertir
 * \param buffer Chaine de caracteres resultante
*/
void partyToString(party_t *party, char *buffer);

/**
 * \fn void stringToParty(char *buffer, party_t *party);
 * \brief Fonction de conversion d'une chaine de caracteres en partie
 * \param buffer Chaine de caracteres a convertir
 * \param party Partie resultante
*/
void stringToParty(char *buffer, party_t *party);

/* ------------------------------------------------------------------------ */
/*            M A N I P U L A T I O N    D E    C O U P S                   */
/* ------------------------------------------------------------------------ */

/**
 * \fn void coupToString(coup_t *coup, char *buffer);
 * \brief Fonction de conversion d'un coup en chaine de caracteres
 * \param coup Coup a convertir
 * \param buffer Chaine de caracteres resultante
*/
void coupToString(coup_t *coup, char *buffer);

/**
 * \fn void stringToCoup(char *buffer, coup_t *coup);
 * \brief Fonction de conversion d'une chaine de caracteres en coup
 * \param buffer Chaine de caracteres a convertir
 * \param coup Coup resultante
*/
void stringToCoup(char *buffer, coup_t *coup);

/* ------------------------------------------------------------------------ */
/*            M A N I P U L A T I O N    D ' E V O L U T I O N S           */
/* ------------------------------------------------------------------------ */

/**
 * \fn void evolutionToString(evolution_t *evolution, char *buffer);
 * \brief Fonction de conversion d'une evolution en chaine de caracteres
 * \param evolution Evolution a convertir
 * \param buffer Chaine de caracteres resultante
*/
void evolutionToString(evolution_t *evolution, char *buffer);

/**
 * \fn void stringToEvolution(char *buffer, evolution_t *evolution);
 * \brief Fonction de conversion d'une chaine de caracteres en evolution
 * \param buffer Chaine de caracteres a convertir
 * \param evolution Evolution resultante
*/
void stringToEvolution(char *buffer, evolution_t *evolution);


#endif