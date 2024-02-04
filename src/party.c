/* ------------------------------------------------------------------------ */
/*            M A N I P U L A T I O N    D E    P A R T I E S               */
/* ------------------------------------------------------------------------ */

#include "party.h"
#include <stdlib.h>

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
party_t *partyInit(party_id_t id, client_t *host, party_state_t state) {
    party_t *party = malloc(sizeof(party_t));
    party->id = id;
    party->state = state;
    strcpy(party->host_pseudo, host->pseudo);
    strcpy(party->host_ip, host->ip);
    party->host_port = host->port;
    return party;
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

/**
 * \fn party_id_t generatePartyId(list_party_t *list);
 * \brief Fonction de generation d'un identifiant unique pour une partie
 * \return Identifiant unique
*/
party_id_t generatePartyId(list_party_t *list) {
    // On compte le nombre de parties dans la liste
    int nbParties = 0;
    list_party_t *current = list;
    while(current != NULL) {
        nbParties++;
        current = current->next;
    }
    // On genere un identifiant unique
    party_id_t id = nbParties + 1;
    return id;
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