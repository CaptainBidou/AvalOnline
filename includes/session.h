/**
 * \file    session.h
 * \brief   Fichier d'en-tête de la bibliothèque session
*/

#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>



/**
 * \fn      int createSocket(int mode)
 * \brief   Création d'une socket en mode TCP ou UDP
 * \param   mode : mode de la socket à créer (SOCK_STREAM/SOCK_DGRAM)
 * \return  file descriptor de la socket créée
 */
int createSocket(int mode);

/**
 * \fn      struct sockaddr_in createAddress(char *ip, int port)
 * \brief   Création d'une structure sockaddr_in à partir d'une adresse IP et d'un port
 * \param   ip : adresse ip
 * \param   port : port
 * \return  structure sockaddr_in 
*/
struct sockaddr_in createAddress(char *ip, int port);


/**
 * \fn      int createBindedSocket(int mode, char *ip, int port)
 * \brief   Création d'une socket en mode TCP ou UDP et bindée à une adresse IP et un port
 * \param   mode : mode de la socket à créer (SOCK_STREAM/SOCK_DGRAM)
 * \param   ip : adresse ip
 * \param   port : port
 * \return  file descriptor de la socket créée
 */
int createBindedSocket(int mode, char *ip, int port);

/**
 * \fn     int connectToServer(char *ip, int port)
 * \brief  Demande de connexion à un serveur
 * \param  sock : file descriptor de la socket
 * \param  ip : adresse ip du serveur
 * \param  port : port du serveur
 */
int connectToServer(char *ip, int port);

/**
 * \fn     char *readFromSocket(int sock, int size)
 * \brief  Lecture d'un message sur une socket
 * \param  sock : file descriptor de la socket
 * \param  size : taille du message à lire
 * \return message lu
 * \warning le message doit être libéré après utilisation
*/
char *readFromSocket(int sock, int size);

/**
 * \fn     void writeToSocket(int sock, char *buff)
 * \brief  Ecriture d'un message sur une socket
 * \param  sock : file descriptor de la socket
 * \param  buff : message à envoyer
*/
void writeToSocket(int sock, char *buff);

/**
 * \fn    int acceptClient(int sock)
 * \brief Acceptation d'un client
 * \param sock : file descriptor de la socket
 * 
*/
int acceptClient(int sock);

#endif