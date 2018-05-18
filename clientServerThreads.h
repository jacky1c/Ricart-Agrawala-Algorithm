// included by clientThread.c and serverThread.c

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
#include <unistd.h>

#include "globals.h"

#define MAX_TOKENS 8
#define LOOP_FOREVER 1
#define AWAIT_REPLIES 1
#define SERVER_NOT_READY 1

struct server_address{
    char host_name [MAX_LINE_SIZE];
    int port_no;
};

extern int *my_server_ready;
extern int *my_ticket_no;
extern int *my_highest_ticket_no;
extern int *my_replies;
extern int *my_request;
extern struct server_address *my_deferred_table;
extern int *my_deferred_count;

void mutex_lock (char caller []);
void mutex_unlock (char caller []);
void send_message (int socket_fd, char send_line [], char caller []);
void get_message (int socket_fd, char recv_line [], char caller []);
int read_line (int fd, char *ptr, int line_size);
int write_n (int fd, char *ptr, int n_bytes);
int read_n (int fd, char *ptr, int n_bytes);

void publish_server_address (char host_name [], int port_no);
int get_server_addresses (struct server_address server_table []);
void parse_message (char recv_line [], char message_type [], char source_host_name [], char source_port_no [], char source_ticket_no []);
int tokenize_message (char tokens [][MAX_LINE_SIZE], char message []);
void write_to_history_file (char host_name [], int host_port_no, int *my_ticket_no);
