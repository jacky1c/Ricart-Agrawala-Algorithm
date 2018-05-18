/*********************
 No changes are required to this file!!
 
 This file is responsible starting all the server threads in the distributed system, where each server thread in turn starts its corresponding client thread.
 When a client and server are run, the client blocks while the server performs some initialization.
 The server gets the host name and port number for the host on which it is running, publishes these to a server names file that is shared by all servers in the distributed system, and then blocks waiting for the start message from environment.
 The start message is sent when the user provides keyboard input to environment indicating that the distributed system should start.
 Once the server receives the start message, it notifies the corresponding client that the start message has been received.
 At this point, the client unblocks and reads the server names file so that it can know the host names and port numbers of all the servers in the distributed system.
 While the distributed system runs, environment just waits until keyboard input is provided indicating that the distributed system should stop.
 At this point, a shutdown message is sent to all servers in the distributed system and the distributed system stops.
 *********************/

#include "environment.h"

int main (){
    int socket_fd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char host_name [MAX_LINE_SIZE];
    struct server_address server_table [MAX_SERVERS];
    int server_count;
    char send_line [MAX_LINE_SIZE];
    int i;
    
    gethostname (host_name, sizeof (host_name));
    
    server_count = get_server_addresses (server_table);
    
    /* Start servers */
    printf ("Environment [HOST=%s PID=%d}: Press [ctrl/d] to start ", host_name, getpid ());
    scanf ("%s", send_line);
    printf ("\n");
    strcpy (send_line, "start");
    for (i = 0; i < server_count; i ++){
        if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
            printf ("ERROR socket ERROR in main");
            exit (1);
        }
        memset (&server_addr, 0, sizeof (server_addr));
        server_addr.sin_family = AF_INET;
        hp = gethostbyname (server_table [i].host_name);
        if (hp == (struct hostent *) NULL){
            printf ("ERROR gethostbyname ERROR in main: %s does not exist", server_table [i].host_name);
            exit (1);
        }
        memcpy (&server_addr.sin_addr, hp -> h_addr, hp -> h_length);
        server_addr.sin_port = htons (server_table [i].port_no);
        if (connect (socket_fd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0){
            printf ("ERROR connect ERROR in main");
            exit (1);
        }
        printf ("Environment [HOST=%s PID=%d]: [%s] sent to [HOST=%s PORT=%d]\n", host_name, getpid (), send_line, server_table [i].host_name, server_table [i].port_no);
        send_message (socket_fd, send_line);
        close (socket_fd);
    }
    
    /* Shutdown servers */
    printf ("Environment [HOST=%s PID=%d]: Press [ctrl/d] to shutdown ", host_name, getpid ());
    scanf ("%s", send_line);
    printf ("\n");
    strcpy (send_line, "shutdown");
    for (i = 0; i < server_count; i ++){
        if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
            printf ("ERROR socket ERROR in main");
            exit (1);
        }
        memset (&server_addr, 0, sizeof (server_addr));
        server_addr.sin_family = AF_INET;
        hp = gethostbyname (server_table [i].host_name);
        if (hp == (struct hostent *) NULL){
            printf ("ERROR gethostbyname ERROR in main: %s does not exist", server_table [i].host_name);
            exit (1);
        }
        memcpy (&server_addr.sin_addr, hp -> h_addr, hp -> h_length);
        server_addr.sin_port = htons (server_table [i].port_no);
        if (connect (socket_fd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0){
            close (socket_fd);
            continue;
        }
        printf ("Environment [HOST=%s PID=%d]: [%s] sent to [HOST=%s PORT=%d]\n", host_name, getpid (), send_line, server_table [i].host_name, server_table [i].port_no);
        send_message (socket_fd, send_line);
        close (socket_fd);
    }
    
    exit (0);
}

int get_server_addresses (struct server_address server_table []){
    FILE *server_address_file;
    char char_port_no [MAX_LINE_SIZE];
    int int_port_no;
    int server_count = 0;
    int lock;
    struct flock key;
    
    key.l_type = F_WRLCK;
    key.l_whence = SEEK_SET;
    key.l_start = 0;
    key.l_len = 0;
    key.l_pid = getpid ();
    lock = open ("server_address_file_lock", O_WRONLY);
    fcntl (lock, F_SETLKW, &key);
    
    server_address_file = fopen ("serverAddressFile", "r");
    if (server_address_file == (FILE *) NULL){
        printf ("ERROR clientThread: fopen ERROR");
        exit (1);
    }
    
    while (fscanf (server_address_file, "%s", server_table [server_count].host_name) == 1){
        fscanf (server_address_file, "%s", char_port_no);
        int_port_no = atoi (char_port_no);
        server_table [server_count].port_no = int_port_no;
        server_count ++;
    }
    
    fclose (server_address_file);
    
    key.l_type = F_UNLCK;
    fcntl (lock, F_SETLK, &key);
    close (lock);
    
    return (server_count);
}

void send_message (int socket_fd, char send_line []){
    int i;
    int n;
    
    n = strlen (send_line);
    if ((i = write_n (socket_fd, send_line, n)) != n){
        printf ("ERROR Environment: write_n failed in send_message");
        exit (1);
    }
    
    return;
}

int write_n (int fd, char *ptr, int n_bytes){
    int n_left;
    int n_written;
    
    n_left = n_bytes;
    
    while (n_left > 0){
        n_written = write (fd, ptr, n_left);
        if (n_written <= 0){
            return (n_written);
        }
        n_left = n_left - n_written;
        ptr = ptr + n_written;
    }
    
    return (n_bytes - n_left);
}
