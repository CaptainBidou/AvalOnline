/**
 * \file proto.h
 * \brief Fichier d'en-tête de la libriairie de notre protocole AOTP (Avalam Online Transfer Protocol)
 */
#ifndef AOTP_H
#define AOTP_H

#define DEFAULT_AOTP_PORT 12345     /*!< Port par defaut du protocole */
#define DEFAULT_AOTP_IP "127.0.0.1" /*< ip par défaut*/
#define DEFAULT_AOTP_MAX_CLIENTS 10 /*!< Nombre maximum de clients par defaut */
#define AOTP_STRUCT_SEPARATOR '|'   /*!< Separateur de champs de la requete */
#define AOTP_EMPTY_LINE "::"        /*!< Ligne vide de la requete */
#define AOTP_HEADER_SEPARATOR ':'   /*!< Separateur de l'entete de la requete */

#include "session.h"
#include "avalam.h"
#include "data.h"
#include "time.h"

/* ------------------------------------------------------------------------ */
/*              D É F I N I T I O N S   D E   T Y P E S                     */
/* ------------------------------------------------------------------------ */

typedef int party_id_t; /*!< Identifiant d'une partie */

/**
 * \struct party_state_t
 * \brief Structure de l'etat d'une partie
 */
typedef enum
{
    PARTY_WAITING = 0, /*!< Partie en attente de joueurs */
    PARTY_PLAYING,     /*!< Partie en cours */
    PARTY_FINISHED,    /*!< Partie terminee */
    PARTY_UNKOWN,      /*!< Etat inconnu */
} party_state_t;       /*!< Structure de l'etat d'une partie */

/**
 * \enum AOTP_REQUEST
 * \brief Enumeration des requetes du protocole
 *
 */
typedef enum
{
    // Les requetes vers le serveur d'enregistrement
    AOTP_CONNECT = 200,      /*!< Requete de connexion */
    AOTP_DISCONNECT,         /*!< Requete de deconnexion */
    AOTP_CREATE_PARTY,       /*!< Requete de creation de partie */
    AOTP_JOIN_PARTY,         /*!< Requete de connexion a une partie */
    AOTP_LIST_PARTIES,       /*!< Requete de liste des parties */
    AOTP_UPDATE_PARTY_STATE, /*!< Requete de mise a jour de l'etat d'une partie */

    // Les requetes vers un hote de partie
    AOTP_CONNECT_TO_PARTY = 300, /*!< Requete de connexion a une partie */
    AOTP_SET_READY,              /*!< Requete pour changer d'etat en pret */
    AOTP_SEND_MOVE,              /*!< Requete pour envoyer un coup */
    AOTP_SEND_EVOLUTION,         /*!< Requete pour envoyer un coup d'evolution */
    AOTP_LEAVE_GAME,             /*!< Requete de deconnexion d'une partie */
} AOTP_REQUEST;                  /*!< Enumeration des requetes du protocole */

/**
 * \enum AOTP_RESPONSE
 * \brief Enumeration des codes de retour du protocole
 */
typedef enum
{
    // Code de retour du serveur d'enregistrement
    AOTP_OK = 200, /*!< Code de retour OK */
    AOTP_CONN_OK = 201, /*!< Code de retour connexion OK */
    AOTP_PARTY_CREATED = 202, /*!< Code de retour partie creee */
    AOTP_PARTY_JOINED = 203, /*!< Code de retour partie rejointe */
    AOTP_PARTY_LIST_RETREIVED = 204, /*!< Code de retour liste des parties recuperee */
    AOTP_PARTY_STATE_UPDATED = 205, /*!< Code de retour etat de la partie mis a jour */
    

    AOTP_NOK = 400, /*!< Code de retour NOK */
    AOTP_STRING_OVERCHARGED = 401, /*!< Code de retour chaine de caractere surchargee */
    AOTP_ERR_CONNECT = 402, /*!< Code de retour erreur de connexion */

} AOTP_RESPONSE;    /*!< Enumeration des codes de retour du protocole */

/**
 * \struct client_state_t
 * \brief Structure de l'etat d'un client
 */
typedef enum {
    CLIENT_UNKOWN = 0, /*!< Etat inconnu */
    CLIENT_SPECTATOR,  /*!< Spectateur */
    CLIENT_READY,      /*!< Pret */
    CLIENT_TRAIT_RED,  /*!< Trait rouge */
    CLIENT_TRAIT_YELLOW, /*!< Trait jaune */
} client_state_t;      /*!< Structure de l'etat d'un client */

/**
 * \struct client_t
 * \brief Structure d'un client
 */
typedef struct {
    int id;        /*!< Identifiant du client */
    char pseudo[20]; /*!< Pseudo du client */
    char ip[16];    /*!< Adresse IP du client */
    short port;     /*!< Port du client */
    client_state_t state; /*!< Etat du client */
    socket_t *socket; /*!< Socket du client */
} client_t;          /*!< Structure d'un client */


/**
 * \struct client_node
 * \brief Structure d'une liste de clients
 * \note Liste chainee de clients
*/
typedef struct client_node {
    client_t *client;         /*!< Clients */
    struct client_node *next; /*!< Pointeur vers le prochain client */
} list_client_t;              /*!< Structure d'une liste de clients */

/**
 * \struct aotp_request
 * \brief Structure de requete du protocole
 */
typedef struct
{
    AOTP_REQUEST action;       /*!< Code de la requete */
    int client_id;           /*!< Identifiant du client */
    char pseudo[20];           /*!< Pseudo du client */
    char host_ip[16];          /*!< Adresse IP de l'hote */
    short host_port;           /*!< Port de l'hote */
    coup_t *coup;              /*!< Coup a jouer */
    evolution_t *evolution;    /*!< Evolution a jouer */
    party_state_t party_state; /*!< Etat de la partie */
    party_id_t party_id;       /*!< Identifiant de la partie */
} aotp_request_t;              /*!< Structure de requete du protocole */

/**
 * \struct party_t
 * \brief Structure representant une partie
 */
typedef struct
{
    party_id_t id;       /*!< Identifiant de la partie */
    char pseudo[20];     /*!< Pseudo de l'hote */
    char host_ip[16];    /*!< Adresse IP de l'hote */
    short host_port;     /*!< Port de l'hote */
    char host_pseudo[20]; /*!< Pseudo de l'hote */
    party_state_t state; /*!< Etat de la partie */
} party_t;               /*!< Structure d'une partie */

typedef struct party_node
{
    party_t *party;          /*!< Parties */
    struct party_node *next; /*!< Pointeur vers la prochaine partie */
} list_party_t;              /*!< Structure d'une liste de parties */

/**
 * \struct aotp_response
 * \brief Structure de reponse du protocole
 */
typedef struct
{
    AOTP_RESPONSE code;      /*!< Code de retour */
    char response_desc[100]; /*!< Description de la reponse */
    int client_id;
    list_party_t *parties;
    char host_ip[16];
    short host_port;
    position_t *position;
} aotp_response_t; /*!< Structure de reponse du protocole */

/* ------------------------------------------------------------------------ */
/*            P R O T O T Y P E S    D E    F O N C T I O N S               */
/* ------------------------------------------------------------------------ */

/**
 * \fn void requestHandler(socket_t *socket, aotp_request_t *requestData)
 * \brief Fonction de gestion des requetes du protocole
 * Pour chaque requete il y un traitement specifique a effectuer
 */
int requestHandler(socket_t *socket, aotp_request_t *requestData, list_client_t **clients, list_party_t **parties);

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
 * \fn void clientInit(client_t *client, int id, char *pseudo, socket_t socket);
 * \brief Fonction d'initialisation d'un client
 * \param client Client a initialiser
 * \param id Identifiant du client
 * \param pseudo Pseudo du client
 * \param socket Socket du client
 */
client_t *initClient(int id, char *pseudo, client_state_t state, socket_t *socket);

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
party_t *partyInit(party_id_t id, client_t *host, party_state_t state);

/**
 * \fn list_client_t *initClientList(client_t *client);
 * \brief Fonction d'initialisation d'une liste de clients
 * \param list Liste de clients a initialiser
 */
list_client_t *initClientList(client_t *client);

/**
 * \fn void addClient(list_client_t **head, client_t *client);
 * \brief Fonction d'ajout d'un client a une liste de clients
 * \param list Liste de clients
 * \param client Client a ajouter
 */
void addClient(list_client_t **head, client_t *client);

/**
 * \fn void removeClient(list_client_t *list, client_t *client);
 * \brief Fonction de suppression d'un client d'une liste de clients
 * \param list Liste de clients
 * \param client Client a supprimer
 */
void removeClient(list_client_t **head, int client_id);

/**
 * \fn void initResponse(aotp_response_t *response, AOTP_RESPONSE code, list_party_t *parties, position_t *position);
 * \brief Fonction d'initialisation d'une reponse
 * \param response Reponse a initialiser
 * \param code Code de retour de la reponse
 * \param parties Parties a retourner (optionnel)
 * \param position Position a retourner (optionnel)
 */
void initResponse(aotp_response_t *response, AOTP_RESPONSE code, list_party_t *parties, position_t *position, int client_id);

/**
 * \fn void struct2Response(aotp_response_t *response, char *buffer);
 * \brief Fonction de conversion d'une structure en reponse sous le format aotp
 * \param response Reponse a convertir
 * \param buffer Buffer de conversion
 */
void struct2Response(aotp_response_t *response, char *buffer);

/**
 * \fn void response2Struct(char *buffer, aotp_response_t *response);
 * \brief Fonction de conversion d'une reponse sous le format aotp en structure
 * \param buffer Reponse a convertir
 * \param response Structure resultante
 *
 */
void response2Struct(char *buffer, aotp_response_t *response);

/**
 * \fn list_party_t initPartyList(party_t *party);
 * \brief Fonction d'initialisation d'une liste de parties
 * \param list Liste de parties a initialiser
 * \param party Partie a ajouter (optionnel)
 */
list_party_t *initPartyList(party_t *party);

/**
 * \fn void addParty(list_party_t **head, party_t *party);
 * \brief Fonction d'ajout d'une partie a une liste de parties
 * \param head Liste de parties
 * \param party Partie a ajouter
 */
void addParty(list_party_t **head, party_t *party);

/**
 * \fn void removeParty(list_party_t *list, party_t *party);
 * \brief Fonction de suppression d'une partie d'une liste de parties
 * \param list Liste de parties
 * \param party Partie a supprimer
 */
void removeParty(list_party_t **list, party_t *party);

/**
 * \fn short generateClientId() 
 * \brief Fonction de generation d'un identifiant unique pour un client
 * \return Identifiant unique
 */
int generateClientId();


/**
 * \fn client_t *getClientById(list_client_t *list, int id);
 * \brief Fonction de recuperation d'un client par son identifiant
 * \param list Liste de clients
 * \param id Identifiant du client
 * \return Client correspondant a l'identifiant ou NULL
 */
client_t *getClientById(list_client_t *list, int id);

/**
 * \fn party_t *getPartyById(list_party_t *list, party_id_t id);
 * \brief Fonction de recuperation d'une partie par son identifiant
 * \param list Liste de parties
 * \param id Identifiant de la partie
 * \return Partie correspondante a l'identifiant ou NULL
 */
party_t *getPartyById(list_party_t *list, party_id_t id);


/**
 * \fn aotp_request_t *createRequest(AOTP_REQUEST action);
 * \brief Fonction de creation d'une requete
 * \param action Code de la requete
 */
aotp_request_t *createRequest(AOTP_REQUEST action);


/**
 * \fn char *partyState2String(party_state_t state);
 * \brief Transforme un status de partie en chaine de caractères
 * \param state Etat de la partie
 * \return Chaine de caractères correspondant à l'état de la partie
 * \note la chaine de caractères est allouée dynamiquement, il faut donc la libérer après utilisation
 */ 
char *partyState2String(party_state_t state);

/**
 * \fn short generateClientId() 
 * \brief Fonction de generation d'un identifiant unique pour un client
 * \return Identifiant unique
 */
int generateClientId();


/**
 * \fn client_t *getClientById(list_client_t *list, int id);
 * \brief Fonction de recuperation d'un client par son identifiant
 * \param list Liste de clients
 * \param id Identifiant du client
 * \return Client correspondant a l'identifiant ou NULL
 */
client_t *getClientById(list_client_t *list, int id);

/**
 * \fn party_t *getPartyById(list_party_t *list, party_id_t id);
 * \brief Fonction de recuperation d'une partie par son identifiant
 * \param list Liste de parties
 * \param id Identifiant de la partie
 * \return Partie correspondante a l'identifiant ou NULL
 */
party_t *getPartyById(list_party_t *list, party_id_t id);


/**
 * \fn aotp_request_t *createRequest(AOTP_REQUEST action);
 * \brief Fonction de creation d'une requete
 * \param action Code de la requete
 */
aotp_request_t *createRequest(AOTP_REQUEST action);


/**
 * \fn char *partyState2String(party_state_t state);
 * \brief Transforme un status de partie en chaine de caractères
 * \param state Etat de la partie
 * \return Chaine de caractères correspondant à l'état de la partie
 * \note la chaine de caractères est allouée dynamiquement, il faut donc la libérer après utilisation
 */ 
char *partyState2String(party_state_t state);

/**
 * \fn void initRequest()
 * \brief Fonction qui créé une requête
 * \param AOTP_REQUEST action;       !< Code de la requete
 */
void initRequest(AOTP_REQUEST action);


/**
 * \fn void sendResponse(AOTP_RESPONSE code, list_party_t *parties, position_t *position);
 * \brief Fonction d'envoi d'une reponse
 * \param code Code de retour de la reponse
 * \param parties Parties a retourner (optionnel)
 * \param position Position a retourner (optionnel)
 */
void sendResponse(socket_t *socket, AOTP_RESPONSE code, list_party_t *parties, position_t *position, int client_id);

#endif
