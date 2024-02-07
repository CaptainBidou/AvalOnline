/**
 * \file design.h
 * \brief Fichier d'entête pour l'interface
 */
#include <stdlib.h>
#include <stdio.h>
#include "aotp.h"
/* ------------------------------------------------------------------------ */
/*              D É F I N I T I O N S   D E   C O N S T A N T E S           */
/* ------------------------------------------------------------------------ */
#define RED 31
#define GREEN 32
#define BLUE 34

/* ------------------------------------------------------------------------ */
/*                      M A C R O - F O N C T I O N S                       */
/* ------------------------------------------------------------------------ */

#define COULEUR(i) printf("\e[%dm", i)
#define COLOR_RESET printf("\e[0m")

/* ------------------------------------------------------------------------ */
/*            P R O T O T Y P E S    D E    F O N C T I O N S               */
/* ------------------------------------------------------------------------ */
/**
* \fn void clearBuffer()
* \brief Vide le buffer
*/
void clearBuffer();

/**
* \fn void menu()
* \brief Affiche le menu client
*/
void menu();

/**
* \fn void loadingBar()
* \brief Affiche une barre de chargement totalement inutile
*/
void loadingBar();

/**
* \fn void getPseudo(char *pseudo)
* \brief Demande le pseudo du client
* \param pseudo Pseudo du client
*/
void getPseudo(char *pseudo);

/**
* \fn void afficherParties()
* \param parties Liste des parties
* \brief Affiche les parties en cours
*/
void afficherParties(list_party_t *parties);

/**
* \fn void promptHostIpPort(char *hostIp, short *hostPort)
* \brief Demande à l'utilisateur l'adresse IP et le port pour héberger une partie
* \param hostIp Adresse IP de l'hôte
* \param hostPort Port de l'hôte
*/
void promptHostIpPort(char *hostIp, short *hostPort);

/**
* \fn void afficherEnAttente(char * message)
* \brief Affiche un message avec une animation de chargement
* \param message Message à afficher
*/
void afficherEnAttente(char * message);

/**
* \fn evolution_t promptEvolution(int numCoup)
* \brief Demande à l'utilisateur de choisir une évolution
* \param numCoup Numéro du coup
* \param position Position actuelle
* \return Evolution
*/
evolution_t promptEvolution(int numCoup, position_t position);


/**
 * \fn void showClientState(client_state_t state)
 * \brief Affiche l'état du client ainsi que le nom du fichier js
 * \param state Etat du client
 * 
*/
void showClientState(client_state_t state);