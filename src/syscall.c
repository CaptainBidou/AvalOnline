#include "syscall.h"
/* SECTION 1 : Gestion des signaux */
/*
void install_signal_handler(int signalNumber, void (*handler)(int), int flags) {
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = flags;
    action.sa_handler = handler;
    CHECK(sigaction(signalNumber, &action, NULL), "Erreur lors de l'installation du gestionnaire de signaux.\n");
}
*/

/**
 * Bibliothèque pour la matière programmation système
 * Cette bibliothèque a pour but de faciliter l'écriture de programmes pour la matière programmation système
 * Date de création : 2023-11-07
 * Auteur : Grando Lukas
 */

/* SECTION : Signalisation
 * Changement/Ajout de fonctionnalités :
 * - [ ] Fonctions qui change le masque de signaux du processus
 * - [ ] Macros de debug activables à la compilation
 */

/**
 * @brief Fonction qui initialise un masque de signaux
 * @details La fonction initialise un masque de signaux ou tous les signaux sont bloqués
 * @return void
 */
void init_mask()
{
    sigset_t mask;
    // Initialiser le masque de signaux en bloquant tous les signaux
    CHECK(sigfillset(&mask), "Erreur lors de l'initialisation du masque de signaux.\n");
    // Appliquer le masque de signaux
    CHECK(sigprocmask(SIG_SETMASK, &mask, NULL), "Erreur lors de l'application du masque de signaux.\n");
}

/**
 * @brief Fonction qui ajoute un signal au masque de signaux
 * @details La fonction ajoute un signal au masque de signaux
 * @param signalNumber Le numéro du signal à ajouter
 * @return void
 */
void add_signal_to_mask(int signalNumber)
{
    sigset_t mask;
    // Récupérer le masque de signaux du processus
    CHECK(sigprocmask(SIG_SETMASK, NULL, &mask), "Erreur lors de la récupération du masque de signaux.\n");
    // Ajouter le signal au masque de signaux
    CHECK(sigdelset(&mask, signalNumber), "Erreur lors de l'ajout du signal au masque de signaux.\n");
    // Appliquer le masque de signaux
    CHECK(sigprocmask(SIG_SETMASK, &mask, NULL), "Erreur lors de l'application du masque de signaux.\n");
}

/**
 * @brief Fonction qui ajoute un signal au masque de signaux
 * @details La fonction ajoute un signal au masque de signaux
 * @param signalNumber Le numéro du signal à ajouter
 * @param handler Le handler à utiliser pour le signal
 * @param flags Les flags à utiliser pour le signal
 */
void install_signal_handler(int signalNumber, void (*handler)(int), int flags)
{
    add_signal_to_mask(signalNumber);
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = flags;
    action.sa_handler = handler;
    CHECK(sigaction(signalNumber, &action, NULL), "Erreur lors de l'installation du gestionnaire de signaux.\n");
}

/**
 * @brief Fonction qui fait une temporisation aléatoire entre 2 valeurs
 * @details Les valeurs sont en microsecondes
 * @param min La valeur minimale de la temporisation
 * @param max La valeur maximale de la temporisation
 * @return void
 */
void random_usleep(int min, int max)
{
    int random = rand() % (max - min + 1) + min;
    usleep(random);
}

/* SECTION : THREADS
 * Changement/Ajout de fonctionnalités :
 * - [ ] Ajout de la fonction display_pthread_attr() man init
 */

/**
 * @brief Fonction qui affiche les attributs d'un thread
 * @details La fonction provient du man init
 * @param attr Les attributs du thread
 */
void display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
    int s, i;
    size_t v;
    void *stkaddr;
    struct sched_param sp;

    CHECK_T(s = pthread_attr_getdetachstate(attr, &i), "pthread_attr_getdetachstate");
    printf("%sDetach state        = %s\n", prefix, (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" : (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE"
                                                                                                                                               : "???");

    CHECK_T(s = pthread_attr_getscope(attr, &i), "pthread_attr_getscope");
    printf("%sScope               = %s\n", prefix, (i == PTHREAD_SCOPE_SYSTEM) ? "PTHREAD_SCOPE_SYSTEM" : (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS"
                                                                                                                                       : "???");

    CHECK_T(s = pthread_attr_getinheritsched(attr, &i), "pthread_attr_getinheritsched");
    printf("%sInherit scheduler   = %s\n", prefix, (i == PTHREAD_INHERIT_SCHED) ? "PTHREAD_INHERIT_SCHED" : (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED"
                                                                                                                                          : "???");

    CHECK_T(s = pthread_attr_getschedpolicy(attr, &i), "pthread_attr_getschedpolicy");
    printf("%sScheduling policy   = %s\n", prefix, (i == SCHED_OTHER) ? "SCHED_OTHER" : (i == SCHED_FIFO) ? "SCHED_FIFO"
                                                                                    : (i == SCHED_RR)     ? "SCHED_RR"
                                                                                                          : "???");

    CHECK_T(s = pthread_attr_getschedparam(attr, &sp), "pthread_attr_getschedparam");
    printf("%sScheduling priority = %d\n", prefix, sp.sched_priority);

    CHECK_T(s = pthread_attr_getguardsize(attr, &v), "pthread_attr_getguardsize");
    printf("%sGuard size          = %zu bytes\n", prefix, v);

    CHECK_T(s = pthread_attr_getstack(attr, &stkaddr, &v), "pthread_attr_getstack");
    printf("%sStack address       = %p\n", prefix, stkaddr);
    printf("%sStack size          = %#zx bytes\n", prefix, v);
}

/**
 * @brief Fonction qui créer un thread avec un numéro
 * @details La fonction créer un thread
 */
pthread_t create_thread(pthread_t *thread, void *(*start_routine)(void *), long thread_number)
{
    // Creation de l'attribut du thread
    pthread_attr_t attr;
    pthread_t tid;

    // Initialisation de l'attribut du thread
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); // On rend le thread détachable

    // Création du thread
    CHECK_T(tid = pthread_create(thread, &attr, start_routine, (void *)thread_number), "pthread_create");
    pthread_attr_destroy(&attr); // Destruction de l'attribut du thread
}

/* SECTION : SEM
 * Changement/Ajout de fonctionnalités :
 */

/**
 * @brief Fonction qui créer une sémaphore nommé
 */
sem_t *create_named_sem(char *name, int value)
{
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, 0666, value);

    return sem;
}

/**
 * @brief Fonction qui ouvre une sémaphore nommé
 * @details la sémpahore doit être créé avant
 */
sem_t *open_named_sem(char *name)
{
    sem_t *sem = sem_open(name, 0);
    CHECK_SEM_OPEN(sem, "Erreur lors de l'ouverture de la sémaphore nommé.\n");
    return sem;
}

/**
 * @brief Fonction qui ferme une sémaphore nommé
 */
void close_named_sem(sem_t *sem)
{
    CHECK(sem_close(sem), "Erreur lors de la fermeture de la sémaphore nommé.\n");
}

/**
 * @brief Fonction qui supprime une sémaphore nommé
 */
void unlink_named_sem(char *name)
{
    CHECK(sem_unlink(name), "Erreur lors de la suppression de la sémaphore nommé.\n");
}
