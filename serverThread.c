#include "clientServerThreads.h"

void *server_thread (void *args){
    int socket_fd;
    int new_socket_fd;
    int addr_length;
    int child_pid;
    int child_status;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct hostent *hp;
    char host_name [MAX_LINE_SIZE];
    int source_socket_fd;
    struct sockaddr_in source_addr;
    char message_type [MAX_LINE_SIZE];
    char source_host_name [MAX_LINE_SIZE];
    char source_port [MAX_LINE_SIZE];
    char source_ticket [MAX_LINE_SIZE];
    char recv_line [MAX_LINE_SIZE];
    char send_line [MAX_LINE_SIZE];
    char work_c_string [MAX_LINE_SIZE];
    char caller [MAX_LINE_SIZE];
    int source_ticket_no;
    int source_port_no;
    
    /* Open up a socket. */
    if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
        printf ("ERROR Server: socket failed");
        exit (1);
    }
    
    /* Clear the data structure (server_addr) to 0's. */
    memset (&server_addr, 0, sizeof (server_addr));
    
    /* Tell our socket to be of the IPv4 internet family (AF_INET). */
    server_addr.sin_family = AF_INET;
    
    /* Aquire the name of the current host system (the local machine). */
    gethostname (host_name, sizeof (host_name));
    
    /* Return misc. host information.  Store the results in the
     * hp (hostent) data structure.  */
    hp = gethostbyname (host_name);
    if (hp == (struct hostent *) NULL){
        printf ("ERROR Server: gethostbyname failed (%s does not exist)", host_name);
        exit (1);
    }
    
    sprintf (caller, "Server [%s]", host_name);
    
    /* Copy the host address to the socket data structure. */
    memcpy (&server_addr.sin_addr, hp -> h_addr, hp -> h_length);
    
    /* Convert the integer 0 to the network-short standard
     * required for networking stuff. This accounts for byte order differences.*/
    server_addr.sin_port = htons (0);
    
    /* Register our address with the system. */
    if (bind (socket_fd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0){
        printf ("ERROR Server: bind failed");
        exit (1);
    }
    
    addr_length = sizeof (server_addr);
    if (getsockname (socket_fd, (struct sockaddr *) &server_addr, &addr_length) < 0){
        printf ("ERROR Server: getsockname failed");
        exit (1);
    }
    
    /* Tell socket to wait for input.  Queue length is 1. */
    if (listen (socket_fd, 128) < 0){
        printf ("ERROR Server: listen failed");
        exit (1);
    }
    publish_server_address (host_name, ntohs (server_addr.sin_port));
    printf ("Server [HOST=%s PID=%d]: ready and listening on [PORT=%d]\n", host_name, getpid (), ntohs (server_addr.sin_port));
    
    mutex_unlock(caller); // extra unlock
    
    while (LOOP_FOREVER){
        addr_length = sizeof (client_addr);
        new_socket_fd = accept (socket_fd, (struct sockaddr *) &client_addr, &addr_length);
        if (new_socket_fd < 0){
            printf ("ERROR Server: accept failed");
            exit (1);
        }
        
        
        mutex_lock (caller);
        get_message (new_socket_fd, recv_line, "Server");
        close (new_socket_fd);
        parse_message (recv_line, message_type, source_host_name, source_port, source_ticket);
        printf ("Server [HOST=%s PID=%d]: received [%s]\n", host_name, getpid (), recv_line);
        source_port_no = atoi(source_port);
        source_ticket_no = atoi(source_ticket);
        mutex_unlock (caller);
        
        
        if (strcmp (message_type, "start") == 0){
            mutex_lock (caller);
            *my_server_ready = 1;
            mutex_unlock (caller);
            continue;
        }
        else if (strcmp (message_type, "shutdown") == 0){
            close (socket_fd);
            return ((void *) NULL);
        }
        if ((child_pid = fork ()) < 0){
            printf ("ERROR Server: fork failed");
            exit (1);
        }
        else if (child_pid == 0){
            mutex_lock (caller);
            
            close(socket_fd);
            
            /* YOUR REQUEST, REPLY, AND DEFERRED REPLY CODE GOES HERE! */
            if (strcmp (message_type, "request") == 0){
                if(*my_highest_ticket_no < source_ticket_no){
                    *my_highest_ticket_no = source_ticket_no;
                }
                
                /*** If a reply message needs to be sent ***/
                if ((*my_request==0) || /*NEED CHANGE BEFORE HAND IN*/
                    source_ticket_no < *my_ticket_no ||
                    (source_ticket_no == *my_ticket_no && strcmp(source_host_name, host_name) < 0)
                    ){

                    // send reply message
                    /************** TEST **************/
                    int socket_fd_out;
                    struct sockaddr_in server_addr_out;
                    struct hostent *hp_out;
                    char ticket_no [MAX_LINE_SIZE];

                    //*** Set up socket connection
                    // Open up the socket
                    if ((socket_fd_out = socket (AF_INET, SOCK_STREAM, 0)) < 0){
                        printf ("ERROR socket ERROR in Server\n");
                        exit (1);
                    }
                    // Clear the data structure (server_addr_out) to 0's.
                    memset (&server_addr_out, 0, sizeof (server_addr_out));
                    // Tell our socket to be of the IPv4 internet family (AF_INET).
                    server_addr_out.sin_family = AF_INET;
                    // Acquire the ip address of receiver node
                    hp_out = gethostbyname (source_host_name);
                    if (hp_out == (struct hostent *) NULL){
                        printf ("ERROR gethostbyname ERROR in Server: %s does not exist\n", source_host_name);
                        exit (1);
                    }
                    // Copy the receiver node address to the socket data structure.
                    memcpy (&server_addr_out.sin_addr, hp_out -> h_addr, hp_out -> h_length);
                    // Convert the integer Port Number to the network-short standardbrequired for networking stuff. This accounts for byte order differences.*/
                    server_addr_out.sin_port = htons (source_port_no);
                    // Establish a connection to receiver node.
                    if (connect (socket_fd_out, (struct sockaddr *) &server_addr_out, sizeof (server_addr_out)) < 0){
                        printf ("ERROR connect ERROR in Server\n");
                        exit (1);
                    }

                    // construct reply message
                    sprintf(send_line, "reply %s %d %d", host_name, 0, *my_highest_ticket_no);
                    send_message (socket_fd_out, send_line, "server");
                    printf ("Response to reuqest message from [%s]: reply message\n", source_host_name);
                    close (socket_fd_out);
                    /****************************/
                }
                
                /*** If reply message won't be sent, update my deferred table ***/
                else{
                    // Add source_ID to my_deferred table
                    strcpy (my_deferred_table[*my_deferred_count].host_name, source_host_name);
                    my_deferred_table[*my_deferred_count].port_no = source_port_no;
                    *my_deferred_count = *my_deferred_count+1;
                    printf ("***Response to the reuqest message from [%s]: update my_deferred_count to %d\n", my_deferred_table[*my_deferred_count].host_name, *my_deferred_count);
                }
            } // End of "If a request messages is received..."
            
            //*** If a reply message is received, increment my_replies ***
            else if (strcmp (message_type, "reply") == 0){
                *my_replies = *my_replies + 1;
                printf("Server: my_replies = %d\n", *my_replies);
                
            } // End of "If a reply message is received..."
            mutex_unlock(caller);
            exit(0);
        }
        do{
            child_pid = waitpid (-1, &child_status, WNOHANG);
        } while (child_pid > 0);
    }
    
    exit (0);
}

void publish_server_address (char host_name [], int port_no){
    FILE *server_address_file;
    int lock;
    struct flock key;
    
    key.l_type = F_WRLCK;
    key.l_whence = SEEK_SET;
    key.l_start = 0;
    key.l_len = 0;
    key.l_pid = getpid ();
    lock = open ("server_address_file_lock", O_WRONLY);
    fcntl (lock, F_SETLKW, &key);
    
    server_address_file = fopen ("serverAddressFile", "a");
    if (server_address_file == (FILE *) NULL){
        printf ("ERROR Server: fopen failed for serverAddressFile append");
        exit (1);
    }
    
    fprintf (server_address_file, "%s\n", host_name);
    fprintf (server_address_file, "%d\n", port_no);
    
    fclose (server_address_file);
    
    key.l_type = F_UNLCK;
    fcntl (lock, F_SETLK, &key);
    close (lock);
    
    return;
}

void parse_message (char recv_line [], char message_type [], char source_host_name [], char source_port_no [], char source_ticket_no []){
    char message [MAX_LINE_SIZE];
    char tokens [MAX_TOKENS][MAX_LINE_SIZE];
    
    strcpy (message, recv_line);
    tokenize_message (tokens, message);
    
    strcpy (message_type, tokens [0]);
    strcpy (source_host_name, tokens [1]);
    strcpy (source_port_no, tokens [2]);
    strcpy (source_ticket_no, tokens [3]);
    
    return;
}

int tokenize_message (char tokens [][MAX_LINE_SIZE], char message []){
    char *token;
    int i;
    
    for (i = 0; i < MAX_TOKENS; i ++){
        strcpy (tokens [i], "");
    }
    
    if ((token = strtok (message, " ")) != NULL){
        strcpy (tokens [0], token);
        i = 1;
        while ((token = strtok (NULL, " ")) != NULL){
            strcpy (tokens [i], token);
            i ++;
        }
    }
    
    return i;
}

