#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "unix_socket_example"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s <file name>\n", argv[0]);
		exit(1);
	}

	char *file_name = argv[1];

    int sockfd, portno, n;

    struct sockaddr_un serv_addr;
    char buffer[256];

    /* create socket, get sockfd handle */
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in server address */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCK_PATH); 

    /* ask user for input */
	// print file to terminal
	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		printf("Error: file not found\n");
		exit(1);
	}
	// char line[256];
	// while (fgets(line, sizeof(line), file)) {
	// 	printf("%s", line);
	// }

    fseek(file, 0, SEEK_END);
    int file_size =ftell(file);
    fclose(file);

    sendto(sockfd, &file_size, sizeof(file_size), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    file = fopen(file_name, "r");
    while(fgets(buffer, 256, file) != NULL)
    {
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    }

	fclose(file);


    close(sockfd);
    return 0;
}
