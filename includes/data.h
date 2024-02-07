/**
 * @file data.h
 * @brief Fichier d'en-tête pour la bliothèque data.
 * @author Lukas Grando
 * @version 1.0
*/

#include "session.h"
#include <stdarg.h>

#ifndef DATA_H
#define DATA_H

#ifdef DATA_DEBUG
    #define DATA_DEBUG_PRINT(...) fprintf(stderr, "[DATA_DEBUG] "); fprintf(stderr, __VA_ARGS__)
#else
    #define DATA_DEBUG_PRINT(...)
#endif


// Creation des types de données
#define MAX_BUFF 1024

typedef char buffer_t[MAX_BUFF];
typedef void* generic_t;

typedef void (*serialize_t)(generic_t, generic_t); //!< Fonction de sérialisation prenant en paramètre un pointeur vers une donnée générique et un pointeur vers un buffer.

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
void send_data(socket_t *socket, generic_t data, serialize_t serializeFunc, ...);

/**
 * @fn void recv_data(int socket, generic_t data, serialize_t dserializeFunc)
 * @brief Fonction permettant de recevoir des données sérialisées.
 * @param socket Socket sur lequel recevoir les données.
 * @param data pointeur vers la donnée générique à remplir
 * @param dserializeFunc  Fonction de désérialisation à utiliser pour remplir la donnée générique.
 * @note Si dsérializeFunc est NULL, cela considére que data est un buffer_t
*/
void recv_data(socket_t *socket, generic_t data, serialize_t dserializeFunc, ...);

#endif // DATA_H