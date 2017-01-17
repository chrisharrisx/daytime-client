#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 4096 /* max text line length */
#define HOSTMAX 255
#define IPMAX 128
#define SERVICEMAX 32

int parse_args(int argc, char **argv, int port_num);
void read_response(char *hostname, char *ip_address, int sockfd, char recvline[]);

int main(int argc, char **argv) {
	bool hostname_set = false;
	char recvline[MAXLINE + 1];
	char *hostname = malloc(HOSTMAX);
	char *ip_address = malloc(IPMAX);
	int sockfd;
	int port_num = -1;
	struct sockaddr_in servaddr;

	port_num = parse_args(argc, argv, port_num);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error\n");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port_num); /* daytime server */

	if (!isdigit(argv[1][0])) { // user specified a server name and port
		hostname = argv[1];
		hostname_set = true;

		// get ip address from hostname
		struct addrinfo info, *infoptr;
		memset(&info, 0, sizeof(info));

		int error = getaddrinfo(argv[1], NULL, &info, &infoptr);
		if (error) {
			fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(error));
			exit(1);
		}

		struct addrinfo *ptr;
  	char ip[IPMAX], service[SERVICEMAX];

  	for (ptr = infoptr; ptr != NULL; ptr = ptr->ai_next) {
			getnameinfo(ptr->ai_addr, ptr->ai_addrlen, ip, IPMAX, service, SERVICEMAX, NI_NUMERICHOST);
			ip_address = ip;
  	}

  	freeaddrinfo(infoptr);
	}
	else {	// user specified an ip address and port
		ip_address = argv[1];
	}

	// copy ip address to servaddr struct
	if (inet_pton(AF_INET, ip_address, &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", argv[1]);
		exit(1);
	}

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		printf("connect error\n");
		exit(1);
	}

	// get hostname from ip address
	if (!hostname_set) {
		char service_type[SERVICEMAX];
		getnameinfo((struct sockaddr *) &servaddr, sizeof(servaddr), hostname, HOSTMAX, service_type, SERVICEMAX, 0);
	}

	read_response(hostname, ip_address, sockfd, recvline);

	exit(0);
}

int parse_args(int argc, char **argv, int port_num) {
	if (argc != 3) {
		printf("usage: client <IPaddress> <port number>\n");
		exit(1);
	}

	for (int i = 0; argv[2][i] != '\0'; i++) {
    if (!isdigit(argv[2][i])) {
      puts("Port number must be an integer");
      exit(1);
    }
  }
  port_num = atoi(argv[2]);

  if (port_num < 1024) {
    puts("Port number must be greater than 1024");
    exit(1);
  }

  return port_num;
}

void read_response(char *hostname, char *ip_address, int sockfd, char recvline[]) {
	printf("Server Name: %s\n", hostname);
	printf("IP Address: %s\n", ip_address);
	printf("Time: ");

	int n;

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
}