#include "session.h"

#define CHECK(sts,msg) if ((sts) == -1) {perror(msg);exit(-1);}
#ifdef SESSION_DEBUG
    #define SESSION_DEBUG_PRINT(...) if (SESSION_DEBUG) printf(__VA_ARGS__)
#else
    #define SESSION_DEBUG_PRINT(...)
#endif


/**
 * \fn      int createSocket(int mode)
 * \brief   Création d'une socket en mode TCP ou UDP
 * \param   mode : mode de la socket à créer (SOCK_STREAM/SOCK_DGRAM)
 * \return  int file descriptor de la socket créée
 */
int createSocket(int mode) {
    int sock;
    CHECK(sock=socket(PF_INET, mode, 0), "__SOCKET__");
    return sock;
}

/**
 * \fn      struct sockaddr_in createAddress(char *ip, int port)
 * \brief   Création d'une structure sockaddr_in à partir d'une adresse IP et d'un port
 * \param   ip : adresse ip
 * \param   port : port
 * \return  structure sockaddr_in 
*/
struct sockaddr_in createAddress(char *ip, int port) {
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons (port);
    addr.sin_addr.s_addr = inet_addr(ip);
    memset(&addr.sin_zero, 0, 8);
    return addr;
}

/**
 * \fn      int createBindedSocket(int mode, char *ip, int port)
 * \brief   Création d'une socket en mode TCP ou UDP et bindée à une adresse IP et un port
 * \param   mode : mode de la socket à créer (SOCK_STREAM/SOCK_DGRAM)
 * \param   ip : adresse ip
 * \param   port : port
 * \return  file descriptor de la socket créée
 */
int createBindedSocket(int mode, char *ip, int port) {
    int sock = createSocket(mode);
    struct sockaddr_in addr = createAddress(ip, port);
    CHECK(bind(sock, (struct sockaddr *)&addr, sizeof addr), "__BIND__");
    return sock;
}

/**
 * \fn      int createListeningSocket(int mode, char *ip, int port, short maxClients)
 * \brief   Création d'une socket en mode TCP ou UDP et bindée à une adresse IP et un port
 * \param   mode : mode de la socket à créer (SOCK_STREAM/SOCK_DGRAM)
 * \param   ip : adresse ip
 * \param   port : port
 * \param   maxClients : nombre maximum de clients en attente de connexion
 * \return  file descriptor de la socket créée
 */
int createListeningSocket(int mode, char *ip, int port, short maxClients) {
    int sock = createBindedSocket(mode, ip, port);
    CHECK(listen(sock, maxClients), "__LISTEN__");
    return sock;
}

/**
 * \fn     int connectToServer(char *ip, int port)
 * \brief  Demande de connexion à un serveur
 * \param  sock : file descriptor de la socket
 * \param  ip : adresse ip du serveur
 * \param  port : port du serveur
 */
int connectToServer(char *ip, int port) {
    int sock = createSocket(SOCK_STREAM);
    struct sockaddr_in addr = createAddress(ip, port);
    CHECK(connect(sock, (struct sockaddr *)&addr, sizeof addr), "__CONNECT__");
    return sock;
}

/**
 * \fn     char *readFromSocket(int sock, int size)
 * \brief  Lecture d'un message sur une socket
 * \param  sock : file descriptor de la socket
 * \param  size : taille du message à lire
 * \return message lu
 * \warning le message doit être libéré après utilisation
*/
char *readFromSocket(int sock, int size) {
    char *buff = malloc(size);
    CHECK(read(sock, buff, size), "__READ__");
    return buff;
}

/**
 * \fn     void writeToSocket(int sock, char *buff)
 * \brief  Ecriture d'un message sur une socket
 * \param  sock : file descriptor de la socket
 * \param  buff : message à envoyer
*/
void writeToSocket(int sock, char *buff) {
    int len = strlen(buff)+1;
    CHECK(write(sock, buff, len), "__WRITE__");
}

/**
 * \fn    int acceptClient(int sock)
 * \brief Acceptation d'un client
 * \param sock : file descriptor de la socket
*/
int acceptClient(int sock) {
    struct sockaddr_in addr;
    int len = sizeof(addr);
    int client = accept(sock, (struct sockaddr *)&addr, &len);
    SESSION_DEBUG_PRINT("Client connecté sur [%s]:[%d]\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    return client;
}
