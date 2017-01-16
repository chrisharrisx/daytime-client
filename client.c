#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 4096 /* max text line length */

int main(int argc, char **argv) {
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;
	
	if (argc != 3) {
		printf("usage: client <IPaddress> <port number>\n");
		exit(1);
	}

	for (int i = 0; argv[1][i] != '\0'; i++) {
    if (!isdigit(argv[1][i])) {
      puts("Port number must be an integer");
    }
  }
  int port_num = atoi(argv[1]);

  if (port_num < 1024) {
    puts("Port number must be greater than 1024");
    exit(1);
  }

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error\n");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(DAYTIME_PORT); /* daytime server */
	
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", argv[1]);
		exit(1);
	}
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		printf("connect error\n");
		exit(1);
	}

	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0; /* null terminate */
		
		if (fputs(recvline, stdout) == EOF) {
			printf("fputs error\n");
			exit(1);
		}
	}
	if (n < 0) {
		printf("read error\n");
		exit(1);
	}
	exit(0);
}
