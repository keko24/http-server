#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUFF_SIZE 256

int read_line(int, char *);
void process_client_message(int);

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        error("usage: ./server <port-number>");
    }
    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (1) {
       if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0) {
           error("ERROR on accept");
       }
       pid = fork();
       if (pid < 0) {
           error("ERROR on fork");
       }
       else if (!pid) {
           close(sockfd);
           process_client_message(newsockfd);
           exit(0);
       }
       else {
           close(newsockfd);
       }
    }
    return 0;
}

char get_ch(int fd) {
    char c;
    if (read(fd, &c, 1) == 1)
        return c;
    return EOF;
}

int read_line(int sock, char *buffer) {
    char c;
    int count = 0, flag = 0;
    while ((c = get_ch(sock)) != EOF) {
        buffer[count++] = c;
        if (c == '\n' && buffer[count - 2] == '\r') {
            break;
        } 
    }
    buffer[count] = '\0';
    return count;
}

void process_client_message(int sock) {
    int bytes, level = 0;
    char buffer[BUFF_SIZE];
    bzero(buffer, BUFF_SIZE);
    while ((bytes = read_line(sock, buffer)) > 0) {
        if (!strcmp(buffer, "\r\n")) {
            break;
        }
        printf("%d %s", bytes, buffer);
    }
    bytes = write(sock, "I got your message.\n", 20);
    if (bytes < 0) {
        error("ERROR writing to socket");
    }
}
