#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUFF_SIZE 256
#define INIT_LINE_ARGS 3
#define HEAD_LINES 10
#define HEAD_LINE_ARGS 10

char get_ch(int);
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

void parse_initial(char *initialLine, char *initialLineParsed[INIT_LINE_ARGS]) {
    char *saveptr;
    initialLineParsed[0] = strtok_r(initialLine, " ", &saveptr);
    initialLineParsed[1] = strtok_r(NULL, " ", &saveptr);
    initialLineParsed[2] = strtok_r(NULL, " ", &saveptr);
}

int parse_header(char *headerLine, char *headerLineParsed[HEAD_LINE_ARGS], int count) { 
    char *saveptr;
    for (char *token = strtok_r(headerLine, " ", &saveptr); token != NULL; token = strtok_r(NULL, " ", &saveptr)) {
        headerLineParsed[count++] = token;
        printf("%s\n", headerLineParsed[count - 1]);
    }
    return count;
}

int parse_header_lines(int sock, char *buffer, char *headerLinesParsed[HEAD_LINES][HEAD_LINE_ARGS]) {
    int headerCount = -1, count = 0, bytes;
    while ((bytes = read_line(sock, buffer)) > 0) {
        if (!strcmp(buffer, "\r\n")) {
            break;
        }
        if (buffer[0] == 'h' || buffer[0] == 'H') {
            count = 0;
            count = parse_header(buffer, headerLinesParsed[++headerCount], count);
            headerLinesParsed[headerCount][count] = NULL;
        }
        else {
            count = parse_header(buffer, headerLinesParsed[headerCount], count);
            headerLinesParsed[headerCount][count] = NULL;
        }
    }
    return headerCount;
}

void process_client_message(int sock) {
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
    int headerLinesCount = parse_header_lines(sock, buffer, headerLinesParsed);
    printf("%d\n", headerLinesCount);
    for (int i = 0; i < headerLinesCount; i++) {
        for (int j = 0; headerLinesParsed[i][j] != NULL; j++) {
            printf("%s ", headerLinesParsed[i][j]);
        } 
        printf("\n");
    }
    /*while ((bytes = read_line(sock, buffer)) > 0) {
        if (!strcmp(buffer, "\r\n")) {
            break;
        }
        printf("%d %s", bytes, buffer);
    }*/
    bytes = write(sock, "I got your message.\n", 20);
    if (bytes < 0) {
        error("ERROR writing to socket");
    }
}
