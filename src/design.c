#include "design.h"

/**
* \fn void clearBuffer()
* \brief Vide le buffer
*/
void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/**
* \fn void menu()
* \brief Affiche le menu client
*/
void menu() {
    COULEUR(RED);
    printf("\n\nQue voulez-vous faire ?\n");
    printf("1. Créer une partie\n");
    printf("2. Rejoindre une partie\n");
    printf("3. Quitter\n");
    COLOR_RESET;
}

/**
* \fn void loadingBar()
* \brief Affiche une barre de chargement totalement inutile
*/
void loadingBar()
{
    // Barre de chargement
    COULEUR(BLUE);
    printf("\nRécupération des données en cours...\n");
    for (int i = 0; i < 100; i++) {
        printf("\r");
        printf("[");
        for (int j = 0; j < i; j++) printf("=");

        for (int j = 0; j < 100 - i; j++) printf(" ");
        printf("] %d%%", i);
        fflush(stdout);
        usleep(1000);
    }
    printf("\n");
    COLOR_RESET;
}


/**
* \fn void getPseudo(char *pseudo)
* \brief Demande le pseudo du client
* \param pseudo Pseudo du client
*/
void getPseudo(char *pseudo)
{
    system("clear");
    COULEUR(RED);
    printf("------ Bienvenue sur AvalOnline ! ------ \n\n");
    COLOR_RESET;
    COULEUR(RED);
    printf("Veuillez entrer votre pseudo : ");
    COLOR_RESET;
    COULEUR(GREEN);
    fgets(pseudo, 20, stdin);
    COLOR_RESET;
    // Suppression du \n à la fin du pseudo
    int i = 0;
    while (pseudo[i] != '\n')
    {
        i++;
    }
    pseudo[i] = '\0';
}

/**
* \fn void afficherParties()
* \brief Affiche les parties en cours
*/
void afficherParties(list_party_t *parties) {
    printf("\n\nParties en cours :\n");
    // afficher les parties de la liste
    if(parties == NULL) {
        COULEUR(BLUE);
        printf("\e[0mAucune partie en cours\n");
        return;
    }

    list_party_t *current = parties;
    while(current != NULL) {
        COULEUR(BLUE);
        char *status = partyState2String(current->party->state);
        printf("%3d. \e[0m Partie de %-15s %s\n", current->party->id, current->party->host_pseudo, status);
        current = current->next;
    }

}
/**
* \fn void promptHostIpPort(char *hostIp, short *hostPort)
* \brief Demande à l'utilisateur l'adresse IP et le port pour héberger une partie
* \param hostIp Adresse IP de l'hôte
* \param hostPort Port de l'hôte
*/
void promptHostIpPort(char *hostIp, short *hostPort) {
    COULEUR(RED);
    printf("Veuillez entrer l'adresse IP de votre machine : ");
    COLOR_RESET;
    COULEUR(GREEN);
    fgets(hostIp, 16, stdin);
    COLOR_RESET;
    COULEUR(RED);
    printf("Veuillez entrer le port sur lequel vous souhaitez héberger la partie : ");
    COLOR_RESET;
    COULEUR(GREEN);
    scanf("%hd", hostPort);
    COLOR_RESET;
}
/**
* \fn void afficherEnAttente(char * message)
* \brief Affiche un message avec une animation de chargement
* \param message Message à afficher
*/
void afficherEnAttente(char * message) {
    char animation[] = {'|', '/', '-', '\\'};
    int i = 0;
    system("clear");
    printf("\033[1;31m"); // Changement de couleur du texte en rouge
    
    while (1) {
        printf("\r%s %c ",message, animation[i]);
        fflush(stdout); // Rafraîchissement de la sortie standard
        usleep(200000); // Pause de 200 millisecondes (0.2 seconde)
        i = (i + 1) % 4; // Pour faire tourner l'animation
    }
}

/**
* \fn evolution_t promptEvolution(int numCoup)
* \brief Demande à l'utilisateur de choisir une évolution
* \param numCoup Numéro du coup
* \return Evolution
*/
evolution_t promptEvolution(int numCoup, position_t position) {
    evolution_t evolution = {0, 0, 0, 0};
    int numCase;
    COULEUR(RED);
    switch (numCoup) {
        case 0:
            printf("Veuillez choisir le bonus jaune : ");
            scanf("%d", &numCase);
            evolution.bonusJ = (char) numCase;
        break;

        case 1:
            printf("Veuillez choisir le bonus rouge : ");
            scanf("%d", &numCase);
            evolution.bonusR = (char) numCase;
        break;

        case 2:
            printf("Veuillez choisir le malus rouge : ");
            scanf("%d", &numCase);
            evolution.malusR = (char) numCase;
        break;

        case 3:
            printf("Veuillez choisir le malus jaune : ");
            scanf("%d", &numCase);
            evolution.malusJ = (char) numCase;
        break;
    }
    COLOR_RESET;
    return evolution;
}

/**
 * \fn void showClientState(client_state_t state)
 * \brief Affiche l'état du client
 * \param state Etat du client
 * 
*/
void showClientState(client_state_t state) {
    COULEUR(RED);
    switch (state) {
        case CLIENT_TRAIT_RED:
            printf("Vous êtes ROUGE\n");
            break;
        case CLIENT_TRAIT_YELLOW:
            printf("Vous êtes JAUNE\n");
            break;
        case CLIENT_SPECTATOR:
            printf("Vous êtes SPECTATEUR\n");
            break;
        default:
            break;
    }
    COLOR_RESET;
    COULEUR(BLUE);
    printf("Pour suivre la partie, veuillez ouvrir la page suivante : \n");
    printf("web/avalam.html\n");
    printf("[DEBUG] selectionner le fichier web/js/avalonline-%d.js\n", getpid());
}