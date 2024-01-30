#include "aotp.h"
#include "mysyscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define COULEUR(i)  printf("\e[%dm",i)
#define COLOR_RESET  printf("\e[0m")
#define RED  31 
#define GREEN  32 
#define BLUE 34 


list_party_t *parties;         // Liste des parties en cours


void clearBuffer();
void loadingBar();
void getPseudo(char *pseudo);

int main() {
    char pseudo[20];

    getPseudo(pseudo);
    system("clear");
    // TODO : Requete de connexion


    // TODO : Récupération de la réponse avec la liste des parties en cours


    // Barre de chargement
    loadingBar();
    system("clear");

    COULEUR(RED);
    printf("\n\nBienvenue \e[%dm%s, \e[%dmvoici la liste des parties en cours !\n", GREEN, pseudo, RED);

    // Affichage des parties en cours
    COULEUR(BLUE);
    printf("\n\nParties en cours :\n");
    // TODO : AFFICHER PARTIES EN COURS
    sleep(0.7);
    printf("1. \e[0mPartie 1\n");
    sleep(0.7);
    COULEUR(BLUE);
    printf("2. \e[0mPartie 2\n");
    sleep(0.7);
    COULEUR(BLUE);
    printf("3. \e[0mPartie 3\n");
    sleep(0.7);
    printf("\n\n");
    COLOR_RESET;

    COULEUR(RED);
    printf("Que voulez-vous faire ?\n\n");
    printf("1. Créer une partie\n");
    printf("2. Rejoindre une partie\n");
    printf("3. Quitter\n\n");
    printf("Votre choix : ");

    COULEUR(GREEN);
    fflush(stdin);
    int choix = fgetc(stdin);
    COLOR_RESET;
    switch (choix) {
        case '1':
            COULEUR(RED);
            system("clear");
            printf("------ Création de la partie ------\n\n Veuillez entrer le nom de la partie : ");

            // TODO : CREER PARTIE
            break;
        case '2':
            COULEUR(RED);
            printf("------ Rejoindre une partie ------\n\n Veuillez entrer le numéro de la partie à rejoindre : ");
            COLOR_RESET;
            COULEUR(GREEN);
            clearBuffer(); 
            char numPartie = fgetc(stdin);
            printf("Num partie : %c\n", numPartie);
            // TODO : REJOINDRE PARTIE
            break;
        case '3':
            COULEUR(RED);
            printf("Ah ché genant\n");
            COLOR_RESET;
            exit(0);
            break;
        default:
            COULEUR(RED);
            printf("Ce choix n'est pas valide\n");
            COLOR_RESET;
            break;
    }
    

}


void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void loadingBar() {
    // Barre de chargement
    COULEUR(BLUE);
    printf("\nRécupération des données en cours...\n");
    for (int i = 0; i < 100; i++) {
        printf("\r");
        printf("[");
        for (int j = 0; j < i; j++) {
            printf("=");
        }
        for (int j = 0; j < 100 - i; j++) {
            printf(" ");
        }
        printf("] %d%%", i);
        fflush(stdout);
        usleep(10000);
    }
    printf("\n");
    COLOR_RESET;
}

void getPseudo(char *pseudo) {
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
    while (pseudo[i] != '\n') {
        i++;
    }
    pseudo[i] = '\0';
}

void afficherParties() {
    // afficher les parties de la liste

    // Affichage des parties en cours
    COULEUR(BLUE);
    printf("\n\nParties en cours :\n");
    // TODO : AFFICHER PARTIES EN COURS
    printf("1. \e[0mPartie 1\n");
    COULEUR(BLUE);
    printf("2. \e[0mPartie 2\n");
    COULEUR(BLUE);
    printf("3. \e[0mPartie 3\n");
    printf("\n\n");
    COLOR_RESET;
}
