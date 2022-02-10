// Implementation UDP de la gestion de comptes bancaires d'un client

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include "structpartagees.h"
#define MAX 1000
#define PORT 8080
#define SA struct sockaddr
#define _NONSTD_SOURCE


//UDP IMPLEMENTATION
//un client


//initialiser un seul client qui a plusieurs comptes
Client init_client(void) {
	Client Marcel;
	Marcel.ID_client = "1";
	Marcel.password = "Michel"; //même mdp pour tous les comptes
	Marcel.nb_compte = 2;
	Marcel.comptes = malloc(Marcel.nb_compte * sizeof(Compte)); //on free quand le client se déconnecte

	Marcel.comptes[0].ID_compte = "22";
	Marcel.comptes[0].Solde = 1000;
	Marcel.comptes[0].Op[0].op = AJOUT; //correspond a la creation du compte
	Marcel.comptes[0].Op[0].montant = 1000;
	strcpy(Marcel.comptes[0].Op[0].date, "01/01/2001");

	for (int i = 1; i < 10; i++) {
		Marcel.comptes[0].Op[i].op = NONE;
	}

	Marcel.comptes[1].ID_compte = "33";
	Marcel.comptes[1].Solde = 2000;
	Marcel.comptes[1].Op[0].op = AJOUT; //correspond a la creation du compte
	Marcel.comptes[1].Op[0].montant = 2000;
	strcpy(Marcel.comptes[1].Op[0].date, "01/01/2002");

	for (int i = 1; i < 10; i++) {
		Marcel.comptes[1].Op[i].op = NONE;
	}

	return Marcel;
}

//ajouter l'ope en cours dans l'historique d'ope : tous les AJOUT ou RETRAIT sur un compte d'un client
//attention : ici on ne sauvegarde pas les operations une fois que le client et le serveur ont quitte
void ajout_ope_historique(char* id_compte, int commande, signed int sum, Client* Marcel) {

	for (int j = 0; j < Marcel->nb_compte; j++) {

		if (strcmp(id_compte, Marcel->comptes[j].ID_compte) == 0) {
			Operation new_op[10];

			for (int i = 0; i < 9; i++) {
				new_op[i+1].op = Marcel->comptes[j].Op[i].op;
				strcpy(new_op[i+1].date, Marcel->comptes[j].Op[i].date);
				new_op[i+1].montant = Marcel->comptes[j].Op[i].montant;
			}

			new_op[0].op = commande;
			new_op[0].montant = sum;

			time_t t = time(NULL);
			struct tm tm = *localtime(&t);
			char* jour = (char*) calloc(3, sizeof(char));
			char* mois = (char*) calloc(3, sizeof(char));
			char* an = (char*) calloc(3, sizeof(char));
			char* date = (char*) calloc(20, sizeof(char));
			sprintf(jour, "%02d", tm.tm_mday);
			sprintf(mois, "%02d", tm.tm_mon + 1);
			sprintf(an, "%02d", tm.tm_year + 1900);
			strcat(date, jour);
			strcat(date, "/");
			strcat(date, mois);
			strcat(date, "/");
			strcat(date, an);
			strcpy(new_op[0].date, date);
			free(date);
			free(jour);
			free(mois);
			free(an);

			for (int i = 0; i < 10; i++) {
				Marcel->comptes[j].Op[i].op = new_op[i].op;
				Marcel->comptes[j].Op[i].montant = new_op[i].montant;
				char* st = new_op[i].date;
				strcpy(Marcel->comptes[j].Op[i].date, st);
			}
		}
	}
}

//OPERATIONS <id_client id_compte password>
char* fctOPERATIONS(char* id_client, char* id_compte, char* password, Client* Marcel) {
	if (strcmp(id_client, Marcel->ID_client) == 0) {

		for (int j = 0; j < Marcel->nb_compte; j++) {

			if (strcmp(id_compte, Marcel->comptes[j].ID_compte) == 0) {

				if (strcmp(password, Marcel->password) == 0) {

					char* phrase = "RES-OPERATIONS => 10 dernières opérations du compte : \n";
					char* resultat = (char*) calloc (500, sizeof(char));
					strcat(resultat, phrase);
					char* date;

					for (unsigned int i = 0; i < 10; i++) {
						char* montant = (char *) calloc (50, sizeof(char));
						char* op = (char *) calloc (50, sizeof(char));
						switch (Marcel->comptes[j].Op[i].op) {
							case AJOUT:
								strcat(op, "AJOUT");
								break;
							case RETRAIT:
								strcat(op, "RETRAIT");
								break;
							default:
								strcat(op, "_________");
								break;
						}

						if (strcmp(op, "_________") != 0) {
							date = (char *) calloc(11, sizeof(char));
							strcat(date, Marcel->comptes[j].Op[i].date);
							sprintf(montant, "%ld", Marcel->comptes[j].Op[i].montant);
						}
						else {
							date = (char *) calloc(11, sizeof(char));
							strcat(date,"__________");
							sprintf(montant, "%s", "___________");
						}
						strcat(resultat, date);
						strcat(resultat, " : ");
						strcat(resultat, op);
						strcat(resultat, " ");
						strcat(resultat, montant);
						strcat(resultat, "€\n");
						free(date);
						free(montant);
						free(op);
					}
					return resultat;
				}
				else {return "KO";}
			}
			else if (j == Marcel->nb_compte-1) {return "KO";}
		}
		return "KO";
	}
	else {return "KO";}
}

//SOLDE <id_client id_compte password>
char* fctSOLDE(char* id_client, char* id_compte, char* password, Client* Marcel) {
	if (strcmp(id_client, Marcel->ID_client) == 0) {

		for (int i = 0; i < Marcel->nb_compte; i++) {

			if (strcmp(id_compte, Marcel->comptes[i].ID_compte) == 0) {

				if (strcmp(password, Marcel->password) == 0) {

					char* solde = "RES_SOLDE => ";
					char* charsolde = (char *) calloc(100, sizeof(char));
					char* resultat = (char *) calloc(200, sizeof(char));
					sprintf(charsolde, "%ld", Marcel->comptes[i].Solde);
					strcat(resultat, solde);
					strcat(resultat, charsolde);
					strcat(resultat, "€, en date du ");
					char* date = Marcel->comptes[i].Op[0].date;
					strcat(resultat, date);
					free(charsolde);
					return resultat;
				}
				else {return "KO";}
			}
			else if (i == Marcel->nb_compte-1) {return "KO";}
		}
		return "KO";
	}
	else {return "KO";}
}

//RETRAIT <id_client id_compte password montant>
char* fctRETRAIT(char* id_client, char* id_compte, char* password, signed int sum, Client* Marcel){
	if (strcmp(id_client, Marcel->ID_client) == 0){

		for (int i = 0; i < Marcel->nb_compte; i++) {

			if (strcmp(id_compte, Marcel->comptes[i].ID_compte) == 0){

				if (strcmp(password, Marcel->password) == 0){
					Marcel->comptes[i].Solde -= sum;
					return "OK";
				}
				else {return "KO";}
			}
			else if (i == Marcel->nb_compte-1) {return "KO";}
		}
		return "KO";
	}
	else {return "KO";}
}

//AJOUT <id_client id_compte password montant>
char* fctAJOUT(char* id_client, char* id_compte, char* password, signed int sum, Client* Marcel) {
	if (strcmp(id_client, Marcel->ID_client) == 0) {

		for (int i = 0; i < Marcel->nb_compte; i++) {

			if (strcmp(id_compte, Marcel->comptes[i].ID_compte) == 0) {

				if (strcmp(password, Marcel->password) == 0) {
					Marcel->comptes[i].Solde += sum;
					return "OK";
				}
				else {return "KO";}
			}
			else if (i == Marcel->nb_compte-1) {return "KO";}
		}
		return "KO";
	}
	else {return "KO";}
}

char* Parameters(int i, char* buff, Client* Marcel, char* commande) {
	i += 2;
	char id_cmpt[15];
	char id_clnt[15];
	char psswrd[25];
	char somme[11];
	signed int sum;
	int j = 0;

	while (buff[i] != ' ') {
		id_clnt[j] = buff[i];
		i++;
		j++;
	}

	id_clnt[j] = '\0';
	j = 0;
	i++;

    while (buff[i] != ' ') {
		id_cmpt[j] = buff[i];
		i++;
		j++;
	}

	id_cmpt[j] = '\0';
	j = 0;
	i++;

	while (buff[i] != ' ' && buff[i] != '>' && buff[i] != '\n') {
		psswrd[j] = buff[i];
		i++;
		j++;
	}

	psswrd[j] = '\0';
	j = 0;
	i++;

	if (strcmp(commande, "AJOUT") == 0 || (strcmp(commande,"ajout") == 0) || strcmp(commande, "RETRAIT") == 0 || (strcmp(commande,"retrait") == 0) ) {

		if (buff[i] == '\n') {return "KO";} //echec s'il manque le montant

		while (buff[i] != '>') {
			somme[j] = buff[i];
			i++;
			j++;
		}

		somme[j] = '\0';
		sum = atoi(somme); //convertir somme en entier

		if ( strcmp(commande,"AJOUT") == 0 || (strcmp(commande,"ajout") == 0) ) {
			char* success = fctAJOUT(id_clnt, id_cmpt, psswrd, sum, Marcel);
			if (strcmp(success, "OK") == 0) {
				ajout_ope_historique(id_cmpt, 0, sum, Marcel);
			}
			return success;
		}

		else {
			char* success = fctRETRAIT(id_clnt, id_cmpt, psswrd, sum, Marcel);
			if (strcmp(success, "OK") == 0) {
				ajout_ope_historique(id_cmpt, 1, sum, Marcel);
			}
			return success;
		}
	}
	else if ( (strcmp(commande,"SOLDE") == 0) || (strcmp(commande,"solde") == 0) ) {
		return fctSOLDE(id_clnt, id_cmpt, psswrd, Marcel);
	}

	else if ( (strcmp(commande,"OPERATIONS") == 0) || (strcmp(commande,"operations") == 0) ) {
       return fctOPERATIONS(id_clnt,id_cmpt,psswrd,Marcel);
	}
	return "KO";
}


// Function designed for chat between client and server.
void chat(int sockfd, Client* Marcel, struct sockaddr_in servaddr) {
	char buff[MAX];
	int n;
	char commande[12]; //variable de stockage de la commande
	int i = 0;
	char* coderetour;
	int operation_true = 0;

	socklen_t servaddrlen = sizeof(servaddr);

	// infinite loop for chat
	for (;;) {

		printf("En attente d'une requête du client....\n");

		memset(buff, 0, MAX*sizeof(char));

		// read the message from client and copy it in buffer
		recvfrom(sockfd, buff, sizeof(buff), 0, (SA*)&servaddr, &servaddrlen);

		printf("Reçu du Client : %s\n", buff);

		if (buff[0] != '\n') {
			while (buff[i] != ' ' && buff[i] != '\n') {
				commande[i] = buff[i];
				i++;
			}
			commande[i] = '\0';

			if ( (strcmp(commande,"AJOUT") == 0) || (strcmp(commande,"ajout") == 0) || (strcmp(commande,"RETRAIT") == 0) || (strcmp(commande,"retrait") == 0) ) {
				coderetour = Parameters(i, buff, Marcel, commande);
			}

			else if (strcmp(commande, "OPERATIONS") == 0 || (strcmp(commande,"operations") == 0) ||  (strcmp(commande,"SOLDE") == 0) || (strcmp(commande,"solde") == 0) ) {
				coderetour = Parameters(i, buff, Marcel, commande);
				operation_true = 1;
			}

			else if ( (strcmp(commande, "exit") == 0) || (strcmp("Exit", commande) == 0) || (strcmp("EXIT", commande) == 0)) {
				coderetour = "exit";
			}

			else {coderetour = "KO";}
		}

		else {coderetour = "KO";}

		memset(buff, 0, MAX*sizeof(char));
		n = 0;
		i = 0;

		while (coderetour[n] != '\0') {
			buff[n] = coderetour[n];
			n++;
		}

		if (operation_true == 1 && (strcmp(coderetour,"\0") != 0) && (strcmp(coderetour,"KO") != 0)) {
			free(coderetour);
			//operation_true = 0;
		}
		operation_true = 0;

		// and send that buffer to client
		printf("Envoi de la réponse \"%s\" au client\n\n", buff);
		sendto(sockfd, buff, sizeof(buff), 0, (SA*)&servaddr, sizeof(servaddr));

		//server exits and chat ends.
		if ( (strcmp("exit", buff) == 0) ) {
			printf("Le serveur a quitté\n\n");
			free(Marcel->comptes);
			break;
		}
	}
}

int main() {
	int sockfd;
	unsigned int len;
	struct sockaddr_in servaddr, cli;

	//on n'initialise qu'un seul client qui a plusieurs comptes
	Client Marcel = init_client();

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sockfd == -1) {
		printf("Échec de la création du socket...\n");
		return 0;
	}

	printf("\nSocket créé avec succès !\n\n");

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ( (bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Échec du bind de socket...\n");
		return 0;
	}

	printf("Réussite du bind de socket !\n\n");

	printf("Le serveur est en attente...\n\n");
	len = sizeof(cli);

	// Function for chatting between client and server
	chat(sockfd, &Marcel, servaddr);

	// After chatting close the socket
	close(sockfd);

	return 0;
}
