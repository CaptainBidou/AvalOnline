#include "client.h"


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
client_t *initClient(int id, char *pseudo, client_state_t state, socket_t *socket) {
    client_t *client = malloc(sizeof(client_t));
    client->id = id;
    strcpy(client->pseudo, pseudo);
    client->state = state;
    client->socket = socket;
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
    while (current != NULL){
        if (current->client->id == client_id){
            if (previous == NULL) *head = current->next;
            else previous->next = current->next;
            free(current);
            break;
        }
        previous = current;
        current = current->next;
    }
}

/**
 * \fn int generateClientId() 
 * \brief Fonction de generation d'un identifiant unique pour un client
 * \return Identifiant unique
 */
int generateClientId() {
    // On recupere le timestamp actuel
    time_t timestamp = time(NULL);
    // On concatene les deux valeurs
    int id = timestamp;

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
 * int is2PlayersReady(list_client_t *clients);
 * \brief Fonction de vérification de l'état des joueurs
 * \param clients Liste des clients
 * \return 1 si les deux joueurs sont prêts, 0 sinon
 */
int is2PlayersReady(list_client_t *clients) {
    int nbReady = 0;
    list_client_t *current = clients;
    while(current != NULL) {
        if(current->client->state == CLIENT_READY) nbReady++;
        current = current->next;
    }
    return nbReady == 2;
}
