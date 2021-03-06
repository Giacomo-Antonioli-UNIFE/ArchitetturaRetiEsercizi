#include "message.pb-c.h"
#include "comunication_tools.h"

int ricevi(int sd, char *buf, int n) {
    int i, j;
    i = recv(sd, buf, n, 0);
    if (i != n && i != 0) {
        if (i == -1) {
            fprintf(stderr, "errore in lettura\n");
            exit(1);
        }
        while (i < n) {
            j = recv(sd, &buf[i], n - i, 0);
            if (j == -1) {
                fprintf(stderr, "errore in lettura\n");
                exit(1);
            }
            i += j;
            if (j == 0)
                break;
        }
    } /* si assume che tutti i byte arrivino… se si verifica il fine file si esce */
    return i;
}
//------------------------------------------------------
void pipe_to_upper_level(int pipedesc[], int sd) {
/*
Input
Pipe
Socket Descriptor
*/
  //Nipote-->Figlio
  close(pipedesc[0]);
  close(sd);
  close(1);
  close(2);
  dup(pipedesc[1]);
  dup(pipedesc[1]);

  close(pipedesc[1]);
}
void pipe_from_lower_level(int pipedesc[]) {
  /*
  Input
  Socket Descriptor
  */

  close(pipedesc[1]);

  close(0);
  dup(pipedesc[0]);
  close(pipedesc[0]);

}
//------------------------------------------------------
void proto_send_nodim(int sd, Com1 *risposta) {
/*
Input
Socket descriptor
Protobuf struct da inviare
*/

    void *buffer;
    unsigned length;
    length = com1__get_packed_size(risposta);
    buffer = malloc(length);
    com1__pack(risposta, buffer);


    fprintf(stderr, "ESEGUO\n");

    if ((write(sd, buffer, length)) < 0) {
        perror("WRITE ERROR");
        exit(6);
    }

    free(buffer);
    fflush(stdout);

}
Com1 *proto_receive_nodim(int sd) {
  /*
  Input
  Socket Descriptor
  Output
  Protobuf struct da ricevere
*/
    char buff[DIM];
    int nread;
    Com1 *risposta;
    fprintf(stderr, "ENTRO NELLA FUNZIONE\n");
    nread = read(sd, buff, sizeof(buff));//RICEVO
    if (nread < 0) {
        perror("PROTOBUF");
        exit(5);
    }
    fprintf(stderr, "ENTRO NELLA FUNZIONE\n");

    risposta = com1__unpack(NULL, nread, buff);//DESERIALIZZO/ESTRAGGO
    fprintf(stderr, "ENTRO NELLA FUNZIONE\n");
    if (risposta == NULL) {
        perror("ERRORE DESERIALIZZAZIONE");
        exit(6);
    }

    return risposta;

}
//------------------------------------------------------
void proto_send_dim(int sd, Com1 *risposta) {
  /*
  Input
  Socket Descriptor
  Protobuf struct da inviare
  */
    void *buffer;
    unsigned length;

    char dim[1];

// Serializzo il messaggio
    length = com1__get_packed_size(risposta);
    printf("length: %d\n", length);
    buffer = malloc(length);
    com1__pack(risposta, buffer);

// Mando un singolo byte con la dimensione del messaggio
//	(per ipotesi dimensione del messaggio inferiore a 2^8-1)
//	In generale, con interi a più byte, problemi di
//	1) dimensione in byte (16 bit, 32 bit, 64 bit),
//	2) ordine dei byte (endianness),
//	3) rappresentazione (ad esempio, complemento a 1 o a 2)

    dim[0] = length;
    if (write(sd, dim, 1) < 0) {
        perror("write dim[0]");
        exit(EXIT_FAILURE);
    }

// Mando i byte contenenti il messaggio serializzato
    if (write(sd, buffer, length) < 0) {
        perror("write request");
        exit(5);
    }

// Rilascio della memoria
    free(buffer);
}
Com1 *proto_receive_dim(int sd) {
  /*
  Input
  Socket Descriptor
  Output
  Protobuf struct da ricevere
*/
    int nread;
    uint8_t buffer[DIM];
    Com1 *pack;


    char dim[1];


/* Leggo un singolo byte con la dimensione del messaggio */
    if ((nread = read(sd, dim, 1)) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

/* Leggo esattamente buff[0] byte. */
    if (ricevi(sd, buffer, dim[0]) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

// Deserializzazione
    pack = com1__unpack(NULL, dim[0], buffer);
    if (pack == NULL) {
        perror("errore deserializzazione\n");
        exit(1);
    }

    return pack;
}
//------------------------------------------------------
void read_stringa_dim_fissa(int sd, char *string) {
/*
Input
Socket Descriptor
Stringa da Inviare
*/
    int nread;
    nread = read(sd, string, sizeof(string));//RICEVO
    if (nread < 0) {
        perror("READ STATO");
        exit(5);
    }
}
void send_stringa_dim_fissa(int sd, char *string) {
  /*
  Input
  Socket Descriptor
  Stringa da inviare
*/
    if ((write(sd, string, sizeof(string))) < 0) {
        perror("WRITE ERROR");
        exit(6);
    }
}
//------------------------------------------------------
void read_stringa_senza_terminatore(int sd, char *string) {
  int nread;
  nread = read(sd, string, DIM-1);//RICEVO
  if (nread < 0) {
      perror("READ STATO");
      exit(5);
  }
  }
void send_stringa_senza_terminatore(int sd, char *string) {
  if ((write(sd, string, strlen(string))) < 0) {
      perror("WRITE ERROR");
      exit(6);
  }
}
//------------------------------------------------------
void send_stringa_con_terminatore(int sd, char *string) {
  if ((write(sd, string, strlen(string)+1)) < 0) {
      perror("WRITE ERROR");
      exit(6);
  }
}
void read_stringa_con_terminatore(int sd, char *string) {
  int nread;
  nread = read(sd, string, DIM-1);//RICEVO
  if (nread < 0) {
      perror("READ STATO");
      exit(5);
  }
  }
//------------------------------------------------------
void send_with_ack(int sd, char *string1, char *string2) {
  /*
  Input
  Socket Descriptor
  Stringa da inviare numero 1
  Stringa da inviare numero 2
*/
    char buf[DIM];
    char *ackVer = "ack\n";
    if (write(sd, string1, strlen(string1)) < 0) {
        perror("write str_anno");
        exit(6);
    }
// Leggo e verifico l'ack
    memset(buf, 0, sizeof(buf));
    if (read(sd, buf, DIM) < 0) {
        perror("read ack");
        exit(6);
    }
    if (strcmp(buf, ackVer) != 0) {
        close(sd);
        printf("errore nell'ack\n");
        exit(1);
    }
// Mando il mese al server
    if (write(sd, string2, strlen(string2)) < 0) {
        perror("write mese");
        exit(6);
    }

}
void receive_with_ack(int sd, char *string1, char *string2) {
  /*
  Input
  Socket Descriptor
  Stringa da inviare numero 1
  Stringa da inviare numero 2
*/
    char *ack = "ack\n";
    char localfirst[DIM], localsecond[DIM];
    memset(localfirst, 0, sizeof(localfirst));
    if (read(sd, localfirst, DIM - 1) < 0) {
        perror("read anno");
        exit(9);
    }
// invio ack
    if (write(sd, ack, strlen(ack)) < 0) {
        perror("write ack");
        exit(10);
    }
    memset(localsecond, 0, sizeof(localsecond));
    if (read(sd, localsecond, DIM - 1) < 0) {
        perror("read mese");
        exit(9);
    }
    sprintf(string1, "%s", localfirst);
    sprintf(string2, "%s", localsecond);
}
//------------------------------------------------------
void write_on_socket(int sd) {
  /*
  Input
  Socket Descriptor
  */
    close(1);
    close(2);
    dup(sd);
    dup(sd);
    close(sd);
}
void read_from_stream(int sd) {
  /*
  Input
  Socket Descriptor
*/
    char buff[DIM];
    int nread;
    printf("ENTRO READ");
    printf("ENTRO READ");
    while ((nread = read(sd, buff, DIM)) > 0) {//LEGGO DALLO STREAM
        if (write(1, buff, nread) < 0) {//SCRIVO SU STDOUTPUT
            perror("ERRORE SCRITTURA SU STODUT");
            exit(9);
        }
    }
    write(1, "CRIVO", strlen("CRIVO"));
    printf("\n\nFINE\n\n");
}
//------------------------------------------------------
void send_struct(int sd, Struttura *argstruct) {
  /*
  Input
  Socket Descriptor
  Struttura da inviare
  */
    if ((write(sd, argstruct, sizeof(argstruct))) < 0) {
        perror("WRITE ERROR");
        exit(6);
    }


}
Struttura recive_struct(int sd) {
  /*
  Input
  Socket Descriptor
  Output
  Protobuf struct da ricevere
*/
int nread;
Struttura localstruct;

    nread = read(sd, &localstruct, sizeof(localstruct));//RICEVO
if (nread < 0) {
    perror("READ STRUCT");
    exit(5);
}



    return localstruct;
}
