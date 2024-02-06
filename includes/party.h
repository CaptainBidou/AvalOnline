/**
 * \file party.h
 * \brief librairie regroupant les fonctions de la partie
*/

#ifndef PARTY_H
#define PARTY_H

#include "client.h"

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
} list_party_t;        


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
 * \fn party_t *getPartyById(list_party_t *list, party_id_t id);
 * \brief Fonction de recuperation d'une partie par son identifiant
 * \param list Liste de parties
 * \param id Identifiant de la partie
 * \return Partie correspondante a l'identifiant ou NULL
 */
party_t *getPartyById(list_party_t *list, party_id_t id);

/**
 * \fn char *partyState2String(party_state_t state);
 * \brief Transforme un status de partie en chaine de caractères
 * \param state Etat de la partie
 * \return Chaine de caractères correspondant à l'état de la partie
 * \note la chaine de caractères est allouée dynamiquement, il faut donc la libérer après utilisation
 */ 
char *partyState2String(party_state_t state);

/**
 * \fn void freePartyList(list_party_t **head);
 * \brief Fonction de liberation de la memoire allouee pour une liste de parties
 * \param head Liste de parties
 */
void freePartyList(list_party_t **head);

#endif