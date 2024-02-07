#include <stdio.h>
#include "avalam.h"
#include "topologie.h"

#include "unistd.h"

extern voisins_t topologie[NBCASES]; 
position_t positionInitiale; 

/**
 * \fn     char nbVoisins(char numCase)
 * \brief  Retourne le nombre de voisins de la case numCase
 * \param  numCase numéro de la case
 * \return nombre de voisins de la case numCase
 */
char nbVoisins(char numCase) {
	return topologie[numCase].nb; 
}

/**
 * \fn     voisins_t getVoisins(char numCase)
 * \brief  Retourne les voisins de la case numCase
 * \param  numCase numéro de la case
 * \return voisins de la case numCase (voisins_t)
*/
voisins_t getVoisins(char numCase) {
	return topologie[numCase]; 
}

/**
 * \fn    void listerVoisins(char numCase)
 * \brief Affiche les voisins de la case numCase
 * \param numCase numéro de la case
 * \see   voisins_t
*/
void listerVoisins(char numCase) {
	int i; 
	voisins_t v = getVoisins(numCase);
	for(i=0;i<v.nb;i++) {
		printf("%d ",v.cases[i]);
	}
	printf("\n");
}

/**
 * \fn     char estVoisin(char numCase1, char numCase2)
 * \brief  Retourne la position initiale du jeu
 * \return position initiale du jeu (position_t)
 * \see    position_t
*/
position_t getPositionInitiale() {
	return positionInitiale ;
}

/**
 * \fn	  void afficherPosition(position_t p)
 * \brief Affiche la position p
 * \param p position à afficher
 * \see   position_t
*/
void afficherPosition(position_t p) {
	int i; int delta; 
	for(i=0;i<NBCASES;i++) {
		printf("%3d : %3d : %s", i, p.cols[i].nb, COLNAME(p.cols[i].couleur));
		delta = 0; 
		if (p.evolution.bonusR == i)  delta++; 
		if (p.evolution.bonusJ == i)  delta++;
		if (p.evolution.malusR == i)  delta--;
		if (p.evolution.malusJ == i)  delta--;
		if (delta>0) printf(" [Evolution:+%d]\n",delta);
		else if (delta<0) printf(" [Evolution:%d]\n",delta); 
		else printf("\n");
	}
	
}

/**
 * \fn listeCoups_t getCoupsLegaux(position_t p)
 * \brief Retourne la liste des coups légaux dans la position p
 * \param p position
 * \return liste des coups légaux
 * 
*/
listeCoups_t getCoupsLegaux(position_t p) {
	listeCoups_t l ={0};

	// Pour chaque case, 
	// Si cette contient une colonne non vide, 
	// Pour chaque voisin de cette colonne, 
	// Si la somme colO+colA<= 5, 
	// ajouter ce coup 

	int cO, cD, iV; // case Origine, case Destination, indice Voisin
	voisins_t v; 
	
	for(cO=0;cO<NBCASES;cO++) {
		printf1("case %d\n",cO); 
		if (p.cols[cO].nb == VIDE ) {printf0("vide!\n"); continue;}
		v = getVoisins(cO); 
		for(iV=0;iV<v.nb;iV++) {
			cD = v.cases[iV]; 
			printf2("voisin %d : case %d\n",iV, cD);
			if (p.cols[cD].nb == VIDE ) {printf0("vide!\n"); continue;} // ajout par tomnab 21/03/2018
			if (p.cols[cO].nb + p.cols[cD].nb <= 5) {
				printf0("possible !\n");
				addCoup(&l, cO,cD); 
			}
		}
	}

	return l; 
}

/**
 * \fn void addCoup(listeCoups_t * pL, char origine, char destination)
 * \brief Ajoute un coup à la liste de coups
 * \param pL liste de coups
 * \param origine case d'origine du coup
 * \param destination case de destination du coup
 * \see listeCoups_t
*/
void addCoup(listeCoups_t * pL, char origine, char destination) {
	pL->coups[pL->nb].origine = origine; 
	pL->coups[pL->nb].destination = destination; 
	pL->nb++; 
}

/**
 * \fn void afficherListeCoups(listeCoups_t l)
 * \brief Affiche la liste de coups l
 * \param l liste de coups à afficher
 * \see listeCoups_t
*/
void afficherListeCoups(listeCoups_t l) {
	int i; 
	for(i=0;i<l.nb;i++) {
		printf("%3d : %3d -> %3d\n", i, l.coups[i].origine, l.coups[i].destination); 
	}
}

/**
 * \fn char estValide(position_t p, char origine, char destination)
 * \brief Vérifie si un coup est valide
 * \param p position
 * \param origine case d'origine du coup
 * \param destination case de destination du coup
 * \return VRAI si le coup est valide, FAUX sinon
 * \see position_t
*/
char estValide(position_t p, char origine, char destination) {

	voisins_t v; int i;
	
	// vérifie la légalité d'un coup 
	if (p.cols[origine].nb == VIDE) { 
		printf("jouerCoup impossible : la colonne %d est vide ! \n", origine);
		return FAUX;  
	} 

	if (p.cols[destination].nb == VIDE) { 
		printf("jouerCoup impossible : la colonne %d est vide ! \n", destination);
		return FAUX;  
	} 

	if (p.cols[origine].nb + p.cols[destination].nb > 5) {
		printf("jouerCoup impossible : trop de jetons entre %d et %d ! \n", origine, destination);
		return FAUX;  		
	}

	// Il faut aussi vérifier accessibilité !!
	v= getVoisins(origine);
	for (i=0;i<v.nb;i++)
		if (v.cases[i]==destination) return VRAI;
	
	printf("jouerCoup impossible : cases %d et %d inaccessibles! \n", origine, destination);	
	return FAUX;
}

/**
 * \fn jouerCoup(position_t p, char origine, char destination)
 * \brief Joue un coup dans la position p
 * \param p position
 * \param origine case d'origine du coup
 * \param destination case de destination du coup
 * \return position après le coup
 * \see position_t
*/
position_t jouerCoup(position_t p, char origine, char destination) {
	if (!estValide(p,origine,destination)) return p;

	// Joue un coup 
	p.cols[destination].nb += p.cols[origine].nb; 
	p.cols[destination].couleur = p.cols[origine].couleur; 
	p.cols[origine].nb = VIDE; 
	p.cols[origine].couleur = VIDE; 
	p.numCoup++;
	
	// Il faut mettre à jour les pions évolution
	if (p.evolution.bonusR == origine) p.evolution.bonusR=destination;
	if (p.evolution.bonusJ == origine) p.evolution.bonusJ=destination;
	if (p.evolution.malusR == origine) p.evolution.malusR=destination;
	if (p.evolution.malusJ == origine) p.evolution.malusJ=destination;
	
	// On inverse le trait
	p.trait = (p.trait == JAU) ? ROU : JAU;

	// renvoie la nouvelle position 
	return p; 
}


/**
 * @fn void afficherScore(score_t s)
 * @brief Affiche le score s
 * @param s score à afficher
 * @see score_t
*/
score_t evaluerScore(position_t p) {
	score_t s ={0};
	int i;
 
	for(i=0;i<NBCASES;i++) {
		if (p.cols[i].nb != VIDE)  {
			if (p.cols[i].couleur == JAU) {
				s.nbJ++; 
				if (p.cols[i].nb == 5)  s.nbJ5++; 
			} else {
				s.nbR++; 
				if (p.cols[i].nb == 5)  s.nbR5++; 
			}
		}
	}
	
	// partie évolution 	
	if ( (p.cols[p.evolution.bonusR].nb != VIDE) && (p.cols[p.evolution.bonusR].couleur == JAU)) {s.nbJ++;}
	if ( (p.cols[p.evolution.bonusR].nb != VIDE) && (p.cols[p.evolution.bonusR].couleur == ROU)) {s.nbR++;} 
	if ( (p.cols[p.evolution.bonusJ].nb != VIDE) && (p.cols[p.evolution.bonusJ].couleur == JAU)) {s.nbJ++;}
	if ( (p.cols[p.evolution.bonusJ].nb != VIDE) && (p.cols[p.evolution.bonusJ].couleur == ROU)) {s.nbR++;}
	if ( (p.cols[p.evolution.malusR].nb != VIDE) && (p.cols[p.evolution.malusR].couleur == JAU)) {s.nbJ--;}
	if ( (p.cols[p.evolution.malusR].nb != VIDE) && (p.cols[p.evolution.malusR].couleur == ROU)) {s.nbR--;}
	if ( (p.cols[p.evolution.malusJ].nb != VIDE) && (p.cols[p.evolution.malusJ].couleur == JAU)) {s.nbJ--;}
	if ( (p.cols[p.evolution.malusJ].nb != VIDE) && (p.cols[p.evolution.malusJ].couleur == ROU)) {s.nbR--;}


	return s; 
}

/**
 * \fn void afficherScore(score_t s)
 * \brief Affiche le score s
 * \param s score à afficher
 * \see score_t
*/
void afficherScore(score_t s) {
	printf("J: %d (%d piles de 5) - R : %d (%d piles de 5)\n", s.nbJ, s.nbJ5, s.nbR, s.nbR5);
}

/**
 * \fn int writePosition(position_t p)
 * \brief Ecrit la position p dans un fichier web/avalonline.json
 * \param p position à écrire
 * \return 0 si l'écriture s'est bien passée, -1 sinon
*/
int writePosition(position_t p) {
	FILE *fic=NULL; // Pointeur de notre fichier
	char tmpname[100]; // Nom temporaire du fichier car les clients écrivent tous dans le même fichier en local
	sprintf(tmpname,"web/js/avalonline_%d.js",getpid());
	fic=fopen(tmpname,"w"); // Ouverture du fichier en écriture
	score_t s = evaluerScore(p);

	if (fic==NULL) return 0;
	
	fprintf(fic,"traiterJson({\n %s:%d,\n %s:%d,\n %s:%d,\n %s:%d,\n %s:%d,\n %s:%d,\n %s:%d,\n %s:%d,\n %s:%d,\n %s:[",
		STR_TURN,p.trait,
		STR_SCORE_J,s.nbJ,
		STR_SCORE_J5,s.nbJ5,
		STR_SCORE_R,s.nbR,
		STR_SCORE_R5,s.nbR5,
		STR_BONUS_J,p.evolution.bonusJ,
		STR_BONUS_R,p.evolution.bonusR,
		STR_MALUS_R,p.evolution.malusR,
		STR_MALUS_J,p.evolution.malusJ,
		STR_COLS
	);

	fprintf(fic,"{%s:%d, %s:%d,}",STR_NB,p.cols[0].nb,STR_COULEUR,p.cols[0].couleur);
	for (int i=1;i<NBCASES;i++) fprintf(fic,",\n\t{%s:%d, %s:%d,}",STR_NB,p.cols[i].nb,STR_COULEUR,p.cols[i].couleur);
	fprintf(fic,"]});");
	fclose(fic);
	return 1;
}

/**
 * \fn void jouerEvolution(position_t p, evolution_t evolution)
 * \brief joue les coups d'évolution
 * \param position position actuelle
 * \param evolution pions évolution
*/
position_t jouerEvolution(position_t position ,evolution_t evolution){
	if(position.trait == JAU && position.numCoup == 0) position.evolution.bonusJ = evolution.bonusJ;
    if(position.trait == ROU && position.numCoup == 1) position.evolution.bonusR = evolution.bonusR;
    if(position.trait == ROU && position.numCoup == 2) position.evolution.malusR = evolution.malusR;
    if(position.trait == JAU && position.numCoup == 3) position.evolution.malusJ = evolution.malusJ;
    position.numCoup++;
	if(position.numCoup != 2) position.trait = (position.trait == JAU) ? ROU : JAU;
	else position.trait = ROU; // Le rouge joue 2 fois d'affilée pour l'évolution 
	return position;
}

/**
 * \fn isEvolution(evolution_t e)
 * \brief Vérifie si une position est une position d'évol
 * \param e evolution
*/
int isEvolution(evolution_t e) {
	if (e.bonusJ == -1) return 0;
	if(e.bonusR == -1) return 0;
	if(e.malusJ == -1) return 0;
	if(e.malusR == -1) return 0;

	return 1;
}

/**
 * \fn int isCoup(coup_t c)
 * \brief Vérifie si une position est une position d'évol
 * \param 
*/
int isCoup(coup_t c) {
	if (c.origine == -1) return 0;
	if(c.destination == -1) return 0;
	return 1;
}
