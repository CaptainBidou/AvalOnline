/**
 * \file avalam.h
 * \brief Fichier d'en-tête de la bibliothèque avalam
 * 
 * La bibliothèque avalam contient les fonctions de manipulation des positions, des coups et des scores.
*/

#define FAUX 0
#define VRAI 1


// Vainqueurs des parties et joueurs au trait 
#define EGALITE 0
#define VIDE 0
#define JAU 1
#define ROU 2

#define NBCASES 48
#define UNKNOWN 255

#define COLNAME(c) ((c==ROU) ? "rouge" : "jaune")

// Pour les exports JSON ////////////////////////////////////////////
#define STR_NB "\"nb\""
#define STR_COULEUR "\"couleur\""
#define STR_TURN "\"trait\""
#define STR_COLS "\"cols\""

#define STR_FEN "\"fen\""
#define STR_BONUS_J "\"bonusJ\""
#define STR_BONUS_R "\"bonusR\""
#define STR_MALUS_J "\"malusJ\""
#define STR_MALUS_R "\"malusR\""
#define STR_SCORE_J "\"scoreJ\""
#define STR_SCORE_J5 "\"scoreJ5\""
#define STR_SCORE_R "\"scoreR\""
#define STR_SCORE_R5 "\"scoreR5\""

#define STR_NUMDIAG "\"numDiag\""
#define STR_NOTES "\"notes\""

#define STR_COUPS "\"coups\""
#define STR_ORIGINE "\"o\""
#define STR_DESTINATION "\"d\""
#define STR_J "\"j\""
#define STR_R "\"r\""
#define STR_JOUEURS "\"joueurs\""

#define STR_NOM "\"nom\""
#define STR_SCORE "\"score\""
#define STR_RONDES "\"rondes\""
#define STR_RONDE "\"ronde\""
#define STR_PARTIES "\"parties\""
#define STR_RESULTAT "\"resultat\""
#define STR_STATUT "\"statut\""

#ifdef __DEBUG__
	#define printf0(p) printf(p)
	#define printf1(p,q) printf(p,q)
	#define printf2(p,q,r) printf(p,q,r)
	#define printf3(p,q,r,s) printf(p,q,r,s)
	#define printf4(p,q,r,s,t) printf(p,q,r,s,t)
	#define whoamid(p) whoami(p)
	#define whopd(p) whop(p)
	#define whojd(p) whoj(p)
	#define whoamjd() whoamj()
#else
	#define printf0(p)
	#define printf1(p,q)
	#define printf2(p,q,r)
	#define printf3(p,q,r,s)
	#define printf4(p,q,r,s,t)
	#define whoamid(p)
	#define whoamjd()
	#define whopd(p)
	#define whojd(p)
#endif

//verif appels systèmes 

#define CHECK_IF(sts,val,msg) \
if ((sts) == (val)) {fprintf(stderr,"erreur appel systeme\n");perror(msg); exit(-1);}

#define CHECK_DIF(sts,val,msg) \
if ((sts) != (val)) {fprintf(stderr,"erreur appel systeme\n");perror(msg); exit(-1);}

/**
 * \struct score_t
 * \brief Structure représentant le score d'une partie
 * 
*/
typedef struct {
	char nbJ; /*!< Nombre de pions jaunes */
	char nbJ5; /*!< Nombre de piles de 5 pions jaunes */
	char nbR;	/*!< Nombre de pions rouges */
	char nbR5; /*!< Nombre de piles de 5 pions rouges */
} score_t;

/**
 * \struct voisins_t
 * \brief Structure représentant les voisins d'une case
 * 
 * La structure contient le nombre de voisins et les numéros des cases voisines
 * dans un tableau de 8 cases.
*/
typedef struct {
	char nb; /*!< Nombre de voisins */
	char cases[8]; /*!< Numéros des cases voisines */
} voisins_t; 

/**
 * \struct colonne_t
 * \brief Structure représentant une colonne du jeu
 * La colonne représente un empilement de pions.
*/
typedef struct {
	char nb; /*!< Nombre de pions dans la colonne */
	char couleur; /*!< Couleur de la colonne */
} colonne_t; 

/**
 * \struct evolution_t
 * \brief Structure représentant les pions évolution
 * 
*/
typedef struct { 
	char bonusJ; /*!< Numéro de la case du bonus jaune */
	char malusJ; /*!< Numéro de la case du malus jaune */
	char bonusR; /*!< Numéro de la case du bonus rouge */
	char malusR; /*!< Numéro de la case du malus rouge */
} evolution_t;

/**
 * \struct position_t
 * \brief Structure représentant une position du jeu
 * 
 * La structure contient le trait, le nombre de coups joués, les colonnes du jeu
 * et les pions évolution.
 * \see evolution_t
 * \see colonne_t
*/
typedef struct { 
	char trait; /*!< Couleur du joueur qui doit jouer */
	// char numCoup; // A ajouter
	colonne_t cols[NBCASES]; /*!< Colonnes du jeu */
	evolution_t evolution; /*!< Pions évolution */
} position_t;

/**
 * \struct coup_t
 * \brief Structure représentant un coup
 * 
 * La structure contient l'origine et la destination du coup.
*/
typedef struct {
	char origine; 
	char destination;
} coup_t;

/**
 * \struct listeCoups_t
 * \brief Structure représentant une liste de coups
 * 
 * La structure contient le nombre de coups et un tableau de coups.
 * \see coup_t
*/
typedef struct {
	int nb; 
	coup_t coups[8*NBCASES]; 
} listeCoups_t; 

/**
 * \fn     char nbVoisins(char numCase)
 * \brief  Retourne le nombre de voisins de la case numCase
 * \param  numCase numéro de la case
 * \return nombre de voisins de la case numCase
 */
char nbVoisins(char numCase);

/**
 * \fn     voisins_t getVoisins(char numCase)
 * \brief  Retourne les voisins de la case numCase
 * \param  numCase numéro de la case
 * \return voisins de la case numCase (voisins_t)
*/
voisins_t getVoisins(char numCase); 

/**
 * \fn    void listerVoisins(char numCase)
 * \brief Affiche les voisins de la case numCase
 * \param numCase numéro de la case
 * \see   voisins_t
*/
void listerVoisins(char numCase); 

/**
 * \fn     char estVoisin(char numCase1, char numCase2)
 * \brief  Retourne la position initiale du jeu
 * \return position initiale du jeu (position_t)
 * \see    position_t
*/
position_t getPositionInitiale();

/**
 * \fn	  void afficherPosition(position_t p)
 * \brief Affiche la position p
 * \param p position à afficher
 * \see   position_t
*/
void afficherPosition(position_t p); 

/**
 * \fn void addCoup(listeCoups_t * pL, char origine, char destination)
 * \brief Ajoute un coup à la liste de coups
 * \param pL liste de coups
 * \param origine case d'origine du coup
 * \param destination case de destination du coup
 * \see listeCoups_t
*/
void addCoup(listeCoups_t * pL, char origine, char destination);

/**
 * \fn void afficherListeCoups(listeCoups_t l)
 * \brief Affiche la liste de coups l
 * \param l liste de coups à afficher
 * \see listeCoups_t
*/
void afficherListeCoups(listeCoups_t l);

/**
 * \fn jouerCoup(position_t p, char origine, char destination)
 * \brief Joue un coup dans la position p
 * \param p position
 * \param origine case d'origine du coup
 * \param destination case de destination du coup
 * \return position après le coup
 * \see position_t
*/
position_t jouerCoup(position_t p, char origine, char destination) ;

/**
 * \fn listeCoups_t getCoupsLegaux(position_t p)
 * \brief Retourne la liste des coups légaux dans la position p
 * \param p position
 * \return liste des coups légaux
 * 
*/
listeCoups_t getCoupsLegaux(position_t p) ; 

/**
 * @fn void afficherScore(score_t s)
 * @brief Affiche le score s
 * @param s score à afficher
 * @see score_t
*/
score_t evaluerScore(position_t p);

/**
 * \fn void afficherScore(score_t s)
 * \brief Affiche le score s
 * \param s score à afficher
 * \see score_t
*/
void afficherScore(score_t s); 

