
//
// Created by giacomo on 02/12/18.
//

/*
 * protoc-c --c_out=. message.proto
 * gcc -c message.pb-c.c
 * gcc -o server Server.c message.pb-c.c -L/usr/lib -lprotobuf-c
 */



#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/stat.h>
//************INCLUDE PROTOBUF******************
#include "message.pb-c.h"
//**********************************************

#define DIM 4096
#define numero_argomenti 2

/* Gestore del segnale SIGCHLD */


int main(int argc, char **argv) {
    /******************************************VARIBILI CREAZIONE CONNESSIONE***************************************************************/
    struct addrinfo hints, *res;//Servono sempre
    int err, sd, ns, pid;
    int on = 1;//Servono sempre
    char buff[DIM];
    int nread;
    /********************************************FINE VARIABILI CREAZIONE CONNESSIONE*************************************************************/
    /*
     *
     * DICHIARAZIONE VARIABILI UTILI
     */


    RichiestaClient *richiesta;

    RispostaServer risposta = RISPOSTA_SERVER__INIT;
    void *buffer;
    unsigned length;
    struct stat st;

    char filename[DIM];
    int stop = 0;

    char stato[DIM];


    /*
    * FINE DICHIARAZIONE VARIABILI UTILI
    */
    /***********************************************GENERAZIONE SERVER*************************************************************/
    /* Controllo argomenti */
    if (argc < numero_argomenti) {
        printf("Uso: ./server <porta> \n");
        exit(1);
    }
    /*Fine controllo argomenti*/
    //Segnale per non dover aspettare i figli
    signal(SIGCHLD, handler);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((err = getaddrinfo(NULL, argv[1], &hints, &res)) != 0) {
        fprintf(stderr, "Errore setup indirizzo bind: %s\n", gai_strerror(err));
        exit(2);
    }

    if ((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("Errore in socket");
        exit(3);
    }
//Dichiaro on=1 ad inizio codice
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("setsockopt");
        exit(4);
    }

    if (bind(sd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Errore in bind");
        exit(5);
    }

    freeaddrinfo(res);

    if (listen(sd, SOMAXCONN) < 0) {
        perror("listen");
        exit(6);
    }


/*
*sd filedescriptor di ascolto per le connessioni in entrata
*ns filedescriptor specifico per una singola connessione, uso sempre ns perchè viene passato al figlio figlio tramite fork
*Il figlio chiude subito sd e alla fine chiude ns
*Il padre chiude sempre ns come prima cosa e non chiude mai sd
*/
    /*******************************************FINE GENERAZIONE SERVER**************************************************************/
//######################################CICLO INFINITO DI ASCOLTO CLIENT##############################################################

    /* Attendo i client... */
    for (;;) {
        printf("Server in ascolto...\n");
        //Accetto le connessioni
        if ((ns = accept(sd, NULL, NULL)) < 0) {
            /* Ignoro gli errori di tipo interruzione da segnale. */
            if (errno == EINTR)
                continue;
            /* Gestisco tutte le altre tipologie di errore. */
            perror("accept");
            exit(7);
        }

        /* Generazione di un figlio */
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(8);
        } else if (pid == 0) {
            /* figlio */
            close(sd);
/*******************************************************MODIFICARE PER FARE COSE*********************************************************************************************/
/*
 *
 * FIGLIO E CODICE DA ESEGUIRE
 *
 *
 */

            do {
                nread = read(ns, buff, sizeof(buff));//RICEVO
                if (nread < 0) {
                    perror("PROTOBUF");
                    exit(5);
                }

                richiesta = richiesta_client__unpack(NULL, nread, buff);//DESERIALIZZO/ESTRAGGO
                if (richiesta == NULL) {
                    perror("ERRORE DESERIALIZZAZIONE");
                    exit(6);
                }

                printf("%s", richiesta->nomefile);

                if (stat(richiesta->nomefile, &st) == 0) {
                    risposta.dim = st.st_size;
                    stop = 1;
                } else
                    risposta.dim = -1;

                proto_send_nodim_server(ns, &risposta);

            } while (stop == 0);


            fprintf(stderr, "\n\nESCO DAL LOOP\n\n");

            read_stringa_ben_formata(ns, stato);


            if (strcmp(stato, "ERROR") == 0) {
                fprintf(stderr, "OUCH");
                close(ns);
                exit(1);
            } else if (strcmp(stato, "OK") == 0) {
                fprintf(stderr, "YAY");

                write_on_socket(ns);
                execlp("cat", "cat", richiesta->nomefile, (char *) 0);
            }



/*******************************************************FINE MODIFICA PER FARE COSE*********************************************************************************************/
            close(ns);
            exit(0);
        } else {
            /* padre */
            close(ns);
        }
    }
    return 0;
}
