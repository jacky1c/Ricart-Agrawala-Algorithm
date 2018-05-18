#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/file.h>

#define MAX_LINE_SIZE 256
#define MAX_SERVERS 16

struct server_address{
    char host_name [MAX_LINE_SIZE];
    int port_no;
};

int get_server_addresses (struct server_address server_table []);
void send_message (int socket_fd, char send_line []);
int write_n (int fd, char *ptr, int n_bytes);
