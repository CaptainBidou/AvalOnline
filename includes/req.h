/* ------------------------------------------------------------------------ */
/*                   E N T Ê T E S    S T A N D A R D S                     */
/* ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "aotp.h"
#include "mysyscall.h"
#include "design.h"
//variables globales
/* ------------------------------------------------------------------------ */
/*              D É F I N I T I O N S   D E   V A R I A B L E S             */
/* ------------------------------------------------------------------------ */




/* ------------------------------------------------------------------------ */
/*                   P R O T O T Y P E S    D E    F O N C T I O N S         */
/* ------------------------------------------------------------------------ */

/**
* \fn void createPartyReq(char *hostIp, short hostPort)
* \brief Crée une partie
* \param hostIp Adresse IP de l'hôte
* \param hostPort Port de l'hôte
* \return Réponse du serveur
*/
aotp_response_t createPartyReq(char *hostIp, short hostPort, char *serverAddress,short serverPort, client_t *client);

/**
* \fn aotp_response_t listPartyReq()
* \brief Requête de liste des parties
* \return Réponse du serveur
*/
aotp_response_t listPartyReq(char *serverAddress,short serverPort, client_t *client);

/**
* \fn aotp_response_t connReq(char *pseudo)
* \brief Requête de connexion
* \param pseudo Pseudo du client
* \return Réponse du serveur
*/
aotp_response_t connReq(char *pseudo,char *serverAddress,short serverPort, client_t *client);
/**
* \fn aotp_response_t jouerCoupReq(client_t *client, position_t p, char origine, char destination)
* \brief Requête pour jouer un coup classique
* \param client Informations du client
* \param p Position actuelle
* \param origine Case d'origine
* \param destination Case de destination
* \return Réponse du serveur
*/
aotp_response_t jouerCoupReq(client_t *client, position_t p, coup_t coup);

/**
* \fn aotp_response_t jouerEvolutionReq(client_t *client, position_t p, evolution_t evolution)
* \brief Requête pour jouer un coup évolution
* \param client Informations du client
* \param p Position actuelle
* \param evolution Evolution
* \return Réponse du serveur
*/
aotp_response_t jouerEvolutionReq(client_t *client, position_t p, evolution_t evolution);

/**
* \fn aotp_response_t requestJoinParty(client_t *client, party_id_t partyId)
* \brief Requête pour rejoindre une partie
* \param client Informations du client
* \param partyId Identifiant de la partie
* \return Réponse du serveur
*/
aotp_response_t requestJoinParty(client_t *client, party_id_t partyId, list_party_t*parties);
/**
* \fn aotp_response_t requestReady(client_t *client)
* \brief Requête pour indiquer que le client est prêt
* \param client Informations du client
* \return Réponse du serveur
*/
aotp_response_t requestReady(client_t *client);

/**
 * \fn void playGame(client_t *client)
 * \brief Fonction de déroulement du jeu pour un client
 * \param client Informations du client
 * 
 * Cette fonction fait une requête pour connaitre l'état de la partie
 * Si l'état est WAITING, on demande au client s'il est prêt
 * Si l'état est PLAYING, le client est spectateur et se mets dans une boucle d'attente de la position
*/
//void joinGame(client_t *client, party_state_t state);


