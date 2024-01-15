#include "data.h"


/**
 * @fn void send_data_DGRAM(socket_t *socket, generic_t data, serialize_t serializeFunc, char *ip, int port)
 * @brief Fonction privé permettant d'envoyer des données sérialisées en datagramme.
 * @param socket Socket sur lequel envoyer les données.
 * @param data Donnée générique à envoyer.
 * @param serializeFunc Fonction de sérialisation à utiliser pour envoyer la donnée générique.
 * @param ip Adresse IP du destinataire.
 * @param port Port du destinataire.
 * @see send_data
*/
void send_data_DGRAM(socket_t *socket, generic_t data, serialize_t serializeFunc, char *ip, int port) {
    buffer_t buff;
    if(serializeFunc == NULL) {
        DATA_DEBUG_PRINT("[SEND_DATA] buffer: \"%s\"\n", (char *)data);
        sendToSocket(socket, (char *)data, ip, port);
        return;
    } 
    serializeFunc(data, buff);
    DATA_DEBUG_PRINT("[SEND_DATA] buffer: \"%s\"\n", buff);
    sendToSocket(socket, buff, ip, port);

}

/**
 * @fn void send_data_stream(socket_t *socket , generic_t data, serialize_t serializeFunc)
 * @brief Fonction privé permettant d'envoyer des données sérialisées en mode stream.
 * @param socket Socket sur lequel envoyer les données.
 * @param data Donnée générique à envoyer.
 * @param serializeFunc Fonction de sérialisation à utiliser pour envoyer la donnée générique.
 * @see send_data
*/
void send_data_stream(socket_t *socket , generic_t data, serialize_t serializeFunc) {
    buffer_t buff;
    if(serializeFunc == NULL) {
        writeToSocket(socket, (char *)data);
        DATA_DEBUG_PRINT("[SEND_DATA] buffer: \"%s\"\n", (char *)data);
        return;
    }

    serializeFunc(data, buff);
    DATA_DEBUG_PRINT("[SEND_DATA] buffer: \"%s\"\n", buff);
    writeToSocket(socket, buff);
}

/**
 * @fn void send_data(socket_t *socket, generic_t data, serialize_t serializeFunc, ...)
 * @brief Fonction permettant d'envoyer des données sérialisées.
 * @param socket Socket sur lequel envoyer les données.
 * @param data Donnée générique à envoyer.
 * @param serializeFunc Fonction de sérialisation à utiliser pour envoyer la donnée générique.
 * @see send_data_stream, send_data_DGRAM
 * @note Si serializeFunc est NULL, cela considére que data est un buffer_t
 * @note Si socket est en mode stream, les arguments supplémentaires correspondront à d'autres données génériques à envoyer.
*/
void send_data(socket_t *socket, generic_t data, serialize_t serializeFunc, ...) {
    if(socket->mode == SOCK_STREAM) {
        send_data_stream(socket, data, serializeFunc);

    } else if(socket->mode == SOCK_DGRAM) {
        // TODO : Envoyer des données sérialisées en datagramme
        // Récupération des arguments
        va_list args;
        va_start(args, serializeFunc);
        char *ip = va_arg(args, char*);
        int port = va_arg(args, int);
        va_end(args);
        send_data_DGRAM(socket, data, serializeFunc, ip, port);
    }

}

/**
 * @fn void recv_data_stream(socket_t *socket, generic_t data, serialize_t dserializeFunc)
 * @brief Fonction privé permettant de recevoir des données sérialisées en mode stream.
 * @param socket Socket sur lequel recevoir les données.
 * @param data pointeur vers la donnée générique à remplir
 * @param dserializeFunc Fonction de désérialisation à utiliser pour remplir la donnée générique.
 * @see recv_data
*/
void recv_data_stream(socket_t *socket, generic_t data, serialize_t dserializeFunc) {
    buffer_t buff;
    readFromSocket(socket, MAX_BUFF, buff);
    DATA_DEBUG_PRINT("[RECV_DATA] buffer: \"%s\"\n", buff);
    if(dserializeFunc == NULL) {
        strcpy((char *)data, buff);
    } else {
        dserializeFunc(buff, data);
    }
}

/**
 * @fn void recv_data_DGRAM(socket_t *socket, generic_t data, serialize_t dserializeFunc)
 * @brief Fonction privé permettant de recevoir des données sérialisées en datagramme.
 * @param socket Socket sur lequel recevoir les données.
 * @param data pointeur vers la donnée générique à remplir
 * @param dserializeFunc Fonction de désérialisation à utiliser pour remplir la donnée générique.
 * @see recv_data
 * @note A n'utiliser que pour les sockets UDP
*/
void recv_data_DGRAM(socket_t *socket, generic_t data, serialize_t dserializeFunc) {
    buffer_t buff;
    recvFromSocket(socket, MAX_BUFF, buff);
    DATA_DEBUG_PRINT("[RECV_DATA] buffer: \"%s\"\n", buff);
    if(dserializeFunc == NULL) {
        strcpy((char *)data, buff);
    } else {
        dserializeFunc(buff, data);
    }
}

/**
 * @fn void recv_data(int socket, generic_t data, serialize_t dserializeFunc)
 * @brief Fonction permettant de recevoir des données sérialisées.
 * @param socket Socket sur lequel recevoir les données.
 * @param data pointeur vers la donnée générique à remplir
 * @param dserializeFunc  Fonction de désérialisation à utiliser pour remplir la donnée générique.
 * @note Si dsérializeFunc est NULL, cela considére que data est un buffer_t
*/
void recv_data(socket_t *socket, generic_t data, serialize_t dserializeFunc, ...) {
    if(socket->mode == SOCK_STREAM) {
        recv_data_stream(socket, data, dserializeFunc);
    } else if(socket->mode == SOCK_DGRAM) {
        // TODO : Recevoir des données sérialisées en datagramme
        // Récupération des arguments
        va_list args;
        va_start(args, dserializeFunc);
        va_end(args);
        recv_data_DGRAM(socket, data, dserializeFunc);
    }
    
}
