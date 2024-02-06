/**
 * \file client.h
 * \brief librairie regroupant les fonctions du client
*/

#ifndef CLIENT_H
#define CLIENT_H

#include "session.h"
#include "time.h"
#include "avalam.h"

/**
 * \struct client_state_t
 * \brief Structure de l'etat d'un client
 */
typedef enum {
    CLIENT_UNKOWN = -2, /*!< Etat inconnu */
    CLIENT_SPECTATOR = -1,  /*!< Spectateur */
    CLIENT_READY = 0,      /*!< Pret */
    CLIENT_TRAIT_YELLOW = JAU, /*!< Trait jaune */
    CLIENT_TRAIT_RED = ROU,  /*!< Trait rouge */
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
 * \param state Etat du client
 * \return Client initialise
 */
client_t *initClient(int id, char *pseudo, client_state_t state, socket_t *socket);


/**
 * \fn void addClient(list_client_t **head, client_t *client);
 * \brief Fonction d'ajout d'un client a une liste de clients
 * \param head Liste de clients
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
 * \fn list_client_t *initClientList(client_t *client);
 * \brief Fonction d'initialisation d'une liste de clients
 * \param List Liste de clients
 * \return Liste de clients initialisée
 */
list_client_t *initClientList(client_t *client);


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
 * \fn int is2PlayersReady(list_client_t *clients);
 * \brief Fonction de vérification de l'état des joueurs
 * \param clients Liste des clients
 * \return 1 si les deux joueurs sont prêts, 0 sinon
 */
int is2PlayersReady(list_client_t *clients);

#endif