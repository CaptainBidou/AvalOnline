#include "aotp.h"
#include "mysyscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define COULEUR(i) printf("\e[%dm", i)
#define COLOR_RESET printf("\e[0m")
#define RED 31
#define GREEN 32
#define BLUE 34

#define CHECK_SERVER 1
#define HOST_SERVER 2

list_party_t *parties; // Liste des parties en cours
client_t *client;      // Client courant

void clearBuffer();
void loadingBar();
void getPseudo(char *pseudo);
void afficherParties();
void connectClientToServer(int request, party_t *party, char *pseudo);
void sendRequest(AOTP_REQUEST action, short client_id, char *pseudo, party_id_t party_id, party_state_t party_state, coup_t *coup, evolution_t *evolution);
void menu();
void afficherParties();

int main()
{
    char pseudo[20];
    parties = NULL;

    getPseudo(pseudo);
    system("clear");

    // Requete de connexion au serv d'enregistrement
    connectClientToServer(CHECK_SERVER, NULL, pseudo);

    // Récupération de la réponse avec la liste des parties en cours
    sendRequest(AOTP_LIST_PARTIES, client->id, pseudo, -1, 0, NULL, NULL);
    aotp_response_t *response = malloc(sizeof(aotp_response_t));

    // Barre de chargement
    loadingBar();
    

    while(1){
        system("clear");

        COULEUR(RED);
        printf("\n\nBienvenue \e[%dm%s, \e[%dmvoici la liste des parties en cours !\n", GREEN, pseudo, RED);

        // Affichage des parties en cours
        afficherParties();
        menu();
        
        COULEUR(GREEN);
        char choix = fgetc(stdin);
        clearBuffer();
        COLOR_RESET;

        switch (choix){
            case '1':
                COULEUR(RED);
                system("clear");
                printf("------ Création de la partie ------\n\n");
            
                // TODO : CREER PARTIE
                break;
            case '2':
                COULEUR(RED);
                printf("------ Rejoindre une partie ------\n\n Veuillez entrer le numéro de la partie à rejoindre : ");
                COLOR_RESET;
                COULEUR(GREEN);
                char numPartie = fgetc(stdin);
                clearBuffer();
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
                printf("\nCe choix n'est pas valide\n\n");
                COLOR_RESET;
                break;
        }
    }
}

void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void loadingBar()
{
    // Barre de chargement
    COULEUR(BLUE);
    printf("\nRécupération des données en cours...\n");
    for (int i = 0; i < 100; i++)
    {
        printf("\r");
        printf("[");
        for (int j = 0; j < i; j++)
        {
            printf("=");
        }
        for (int j = 0; j < 100 - i; j++)
        {
            printf(" ");
        }
        printf("] %d%%", i);
        fflush(stdout);
        usleep(10000);
    }
    printf("\n");
    COLOR_RESET;
}

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

void afficherParties()
{   
    list_party_t *parties = NULL; // TODO : Récupérer la liste des parties 
    COULEUR(BLUE);
    printf("\n\nParties en cours :\n");
    if (parties == NULL)
    {
        printf("Aucune partie en cours\n");
    } else {
        while(parties->next != NULL){
        printf("\e[%dm%d. \e[0mPartie %s\n",BLUE, parties->party->id, parties->party->pseudo);
        parties = parties->next;
        }
    }
    printf("\n\n");
    COLOR_RESET;
}

void connectClientToServer(int request, party_t *party, char *pseudo)
{
}

void sendRequest(AOTP_REQUEST action, short client_id, char *pseudo, party_id_t party_id, party_state_t party_state, coup_t *coup, evolution_t *evolution)
{
    initRequest(action, client_id, pseudo, party_id, party_state, coup, evolution);
}

/*typedef struct
AOTP_REQUEST action;       !< Code de la requete
short client_id;           !< Identifiant du client
char pseudo[20];           !< Pseudo du client
party_id_t party_id;       !< Identifiant de la partie
party_state_t party_state; !< Etat de la partie
coup_t coup;               !< Coup a jouer
evolution_t evolution;     !< Evolution a jouer
aotp_request_t;            !< Structure de requete du protocole
*/
