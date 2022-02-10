// Implementation UDP de la gestion de comptes bancaires d'un client

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "structpartagees.h"
#define MAX 1000
#define PORT 8080
#define SA struct sockaddr

void chat(int sockfd, struct sockaddr_in cliaddr) {
	char buff[MAX];
	socklen_t cliaddrlen = sizeof(cliaddr);
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		printf("Veuillez entrer votre demande : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		sendto(sockfd, buff, sizeof(buff), 0, (SA*)&cliaddr, sizeof(cliaddr));
		bzero(buff, sizeof(buff));
		recvfrom(sockfd, buff, sizeof(buff), 0, (SA*)&cliaddr, &cliaddrlen);
		printf("Reçu du serveur : %s\n\n", buff);

		if ( (strcmp("exit", buff) == 0) || (strcmp("Exit", buff) == 0) || (strcmp("EXIT", buff) == 0) ) {
			printf("Le client est parti...\n\n");
			break;
		}
	}
}

int main() {
	int sockfd;
	struct sockaddr_in cliaddr;

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sockfd == -1) {
		printf("Échec de la création du socket...\n");
		return 0;
	}

	printf("\nSocket créé avec succès !\n\n");

	bzero(&cliaddr, sizeof(cliaddr));

	// assign IP, PORT
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	cliaddr.sin_port = htons(PORT);

	printf("Connexion au serveur !\n\n");

	// function for chat
	chat(sockfd, cliaddr);

	// close the socket
	close(sockfd);
	return 0;
}
