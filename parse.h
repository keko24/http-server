#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>                                                                                              
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <poll.h>

#define BUFF_SIZE 1024
#define INIT_LINE_ARGS 3
#define HEAD_LINES 10
#define HEAD_LINE_ARGS 10

char get_ch(int fd);

int read_line(int sock, char *buffer);

int read_line_body(struct pollfd fds[1], int sock, char *buffer);

void parse_initial(char *initialLine, char *initialLineParsed[INIT_LINE_ARGS]);

int parse_header_line(char *headerLine, char *headerLineParsed[HEAD_LINE_ARGS]);

int parse_headers(int sock, char *buffer, char *headersParsed[HEAD_LINES][HEAD_LINE_ARGS]);

#endif
