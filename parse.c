#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <poll.h>

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

int read_line_body(struct pollfd fds[1], int sock, char *buffer) {
    char c;
    int count = 0, flag = 0;
    while (poll(fds, sock, 2000) > 0 && (c = get_ch(sock)) != EOF) {
        buffer[count++] = c;
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

int parse_header_line(char *headerLine, char *headerLineParsed[HEAD_LINE_ARGS]) { 
    char *saveptr;
    int count = 0;
    for (char *token = strtok_r(headerLine, " ", &saveptr); token != NULL; token = strtok_r(NULL, " ", &saveptr)) {
        headerLineParsed[count] = malloc(strlen(token));
        strcpy(headerLineParsed[count++], token);
    }
    return count;
}

int parse_headers(int sock, char *buffer, char *headersParsed[HEAD_LINES][HEAD_LINE_ARGS]) {
    int headerCount = -1, count = 0, bytes = 0, i = 0, curr = 0;
    char *headerLineParsed[HEAD_LINE_ARGS];
    while ((bytes = read_line(sock, buffer)) > 0) {
        if (!strcmp(buffer, "\r\n")) {
            break;
        }
        count = parse_header_line(buffer, headerLineParsed);
        if (headerLineParsed[0][strlen(headerLineParsed[0]) - 1] == ':') {
            curr = 0;
            headerCount++;
        }
        for (i = 0; i < count; i++) {
            headersParsed[headerCount][curr + i] = malloc(strlen(headerLineParsed[i]));
            strcpy(headersParsed[headerCount][curr + i], headerLineParsed[i]);
            free(headerLineParsed[i]);
        }
        curr += count;
        headersParsed[headerCount][curr] = NULL;
    }
    return headerCount + 1;
}
