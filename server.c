#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "parse.h"

void process_client_message(int sock);

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

void process_client_message(int sock) {
    struct pollfd fds[1];
    fds[0].fd = sock;
    fds[0].events = POLLIN;
    int bytes, level = 0;
    char buffer[BUFF_SIZE];
    bzero(buffer, BUFF_SIZE);
    if ((bytes = read_line(sock, buffer)) < 0) {
        error("ERROR reading from socket");
    }
    char *initialLineParsed[INIT_LINE_ARGS];
    parse_initial(buffer, initialLineParsed);
    for (int i = 0; i < INIT_LINE_ARGS; i++) {
        printf("%s\n", initialLineParsed[i]);
    }
    char *headerLinesParsed[HEAD_LINES][HEAD_LINE_ARGS];
    int headerLinesCount = parse_headers(sock, buffer, headerLinesParsed);
    printf("%d\n", headerLinesCount);
    for (int i = 0; i < headerLinesCount; i++) {
        for (int j = 0; headerLinesParsed[i][j] != NULL; j++) {
            printf("%s ", headerLinesParsed[i][j]);
        } 
        printf("\n");
    }
    int totalAmountOfBytes = 0;
    while ((bytes = read_line_body(fds, sock, buffer)) > 0) {
        printf("%d %s\n", bytes, buffer);
        printf("%d\n", atoi(headerLinesParsed[headerLinesCount - 1][1]));
        totalAmountOfBytes += bytes;
        if (atoi(headerLinesParsed[headerLinesCount - 1][1]) == totalAmountOfBytes) {
            break;
        }
    }
    bytes = write(sock, "HTTP/1.1 200 OK\r\n\r\n", 30);
    if (bytes < 0) {
        error("ERROR writing to socket");
    }
}
