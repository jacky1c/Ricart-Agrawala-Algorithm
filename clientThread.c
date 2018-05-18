/***************
 This file requires changes!!
 
 This file is responsible for sending request messages and deferred reply messages to remote servers in the distributed system. You must add code in the three areas indicated:
 the send
 request messages part of the pre-protocol,
 the await for reply messages part of the pre-protocol,
 and the send deferred reply messages of the post-protocol.
 ***************/

#include "clientServerThreads.h"

void *client_thread (void *args){
    int socket_fd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char host_name [MAX_LINE_SIZE];
    int host_port_no;
    struct server_address server_table [MAX_SERVERS];
    int server_count;
    char request_message [MAX_LINE_SIZE];
    char work_c_string [MAX_LINE_SIZE];
    char caller [MAX_LINE_SIZE];
    int i;
    
    
    gethostname (host_name, sizeof (host_name));
    sprintf (caller, "    Client [%s]", host_name);
    printf ("    Client [HOST=%s PID=%d]: ready\n", host_name, getpid ());
    
    /* Don't start until server is ready */
    /*
     When a client thread is run, it blocks until it is notified by the corresponding server that the start message has been received.
     */
    while (SERVER_NOT_READY){
        mutex_lock (caller);
        if (*my_server_ready){
            break;
        }
        mutex_unlock (caller);
    }
    
    /* Don't start until client is ready */
    /*
     Once it is notified, it unblocks and reads the server names file so that it can know the host names and port numbers of all servers in the distributed system.
     */
    server_count = get_server_addresses (server_table);
    for (i = 0; i < server_count; i ++){
        if (strcmp (host_name, server_table [i].host_name) == 0){
            host_port_no = server_table [i].port_no;
            break;
        }
    }
    mutex_unlock (caller);
    
    srand (host_port_no);
    
    
    /****** Ricart-Agrawala Variables Initialization ******/
//    mutex_unlock(caller); // sometimes just need an extra unlock to make things work!
//    mutex_lock(caller);
//    *my_ticket_no = 0;
//    *my_highest_ticket_no = 0;
//    *my_replies = 0;
//    my_deferred_table = NULL;
//    *my_request = 1;
//    mutex_unlock(caller);
    /******************************************************/
    
    /*
     Client thread loops forever running the Ricart-Agrawala algorithm. To simulate doing useful work in the non-critical section, a client simply sleeps for some random amount of time. When it wakes up, it enters the send request messages part of the pre-protocol.
     */
    //*** Loop forever
    while (LOOP_FOREVER){
        //** Non-critical section
        printf ("    Client [HOST=%s PID=%d]: ENTERING NON-CRITICAL SECTION\n", host_name, getpid ());
        usleep (2000 * (1 + rand () % 1000));
        printf ("    Client [HOST=%s PID=%d]: LEAVING NON-CRITICAL SECTION\n", host_name, getpid ());
        
        
        /* THE REQUEST PART OF YOUR PRE-PROTOCOL CODE GOES HERE! */
        //mutex_unlock(caller); // new added
        mutex_lock(caller);
        //printf("milestone1\n");
        *my_request = 1; // my_request = true
        //printf("milestone2\n");
        *my_highest_ticket_no = *my_highest_ticket_no + 1;
        //printf("milestone3\n");
        *my_ticket_no = *my_highest_ticket_no;
        //printf("milestone4\n");
        *my_replies = 0;
        //printf("milestone5\n");
        *my_deferred_count = 0;
        //mutex_unlock(caller);
        
        //************ For all the other nodes, send request message
        for(i=0; i<server_count; i++){
            // If the server name is different than my host name, then send request message
            if(strcmp(server_table[i].host_name, host_name) != 0){
                /********* Send request message TEST **************/
                //*** Set up socket connection
                int socket_fd;
                struct sockaddr_in server_addr;
                struct hostent *hp;
                // Open up the socket */
                if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
                    printf ("    ERROR socket ERROR in Client\n");
                    exit (1);
                }
                // Clear the data structure (server_addr) to 0's. */
                memset (&server_addr, 0, sizeof (server_addr));
                // Tell our socket to be of the IPv4 internet family (AF_INET).
                server_addr.sin_family = AF_INET;
                // Acquire the ip address of receiver node */
                hp = gethostbyname (server_table[i].host_name);
                if (hp == (struct hostent *) NULL){
                    printf ("    ERROR gethostbyname ERROR in Client: %s does not exist\n", server_table[i].host_name);
                    exit (1);
                }
                // Copy the receiver node address to the socket data structure. */
                memcpy (&server_addr.sin_addr, hp -> h_addr, hp -> h_length);
                // Convert the integer Port Number to the network-short standardbrequired for networking stuff. This accounts for byte order differences.*/
                server_addr.sin_port = htons (server_table[i].port_no);
                // Establish a connection to receiver node. */
                if (connect (socket_fd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0){
                    printf ("    ERROR connect ERROR in Client\n");
                    exit (1);
                }
                //*** End of setting up socket connection
                
                // Construct request message
                sprintf(request_message, "request %s %d %d", host_name, host_port_no, *my_ticket_no);
//                printf("    ***Send request message| To: %s; my_ticket_no: %d***\n", server_table[i].host_name, *my_ticket_no);
                send_message (socket_fd, request_message, "client");
                
                close (socket_fd);
                /************** End of Send request message TEST **************/
                
            } // end of "If the server name is different than my host name, then send request message"
        } // end of "For all the other nodes, send request message"
        
        mutex_unlock(caller);
        
        
        printf ("    Client [HOST=%s PID=%d]: WAITING TO ENTER CRITICAL SECTION\n", host_name, getpid ());
        while (AWAIT_REPLIES){
            /* THE AWAIT REPLIES PART OF YOUR PRE-PROTOCOL CODE GOES HERE! */
            mutex_lock(caller);
            if(*my_replies == (server_count-1)){
                *my_replies = 0;
                mutex_unlock(caller);
                break;
            }
            mutex_unlock(caller);
        }
        
        printf ("    Client [HOST=%s PID=%d]: ENTERING CRITICAL SECTION\n", host_name, getpid ());
        //** Critial section
        write_to_history_file (host_name, host_port_no, my_ticket_no);
        printf ("    Client [HOST=%s PID=%d]: LEAVING CRITICAL SECTION\n", host_name, getpid ());
        
        //mutex_unlock(caller); // sometimes just need an extra unlock to make things work!
        
        mutex_lock(caller);
        /* THE DEFERRED REPLIES OF YOUR POST-PROTOCOL CODE GOES HERE! */
        //*my_request = 0;
        
        // For each node in my_deferred_table
        for(i=0; i < *my_deferred_count; i++){
            /********* Send request message TEST **************/
            //*** Set up socket connection
            int socket_fd_out;
            struct sockaddr_in server_addr_out;
            struct hostent *hp_out;
            // Open up the socket */
            if ((socket_fd_out = socket (AF_INET, SOCK_STREAM, 0)) < 0){
                printf ("    ERROR socket ERROR in Client\n");
                exit (1);
            }
            // Clear the data structure (server_addr_out) to 0's. */
            memset (&server_addr_out, 0, sizeof (server_addr_out));
            // Tell our socket to be of the IPv4 internet family (AF_INET).
            server_addr_out.sin_family = AF_INET;
            // Acquire the ip address of receiver node */
            hp_out = gethostbyname (my_deferred_table[i].host_name);
            if (hp_out == (struct hostent *) NULL){
                printf ("    ERROR gethostbyname ERROR in Client: %s does not exist\n", my_deferred_table[i].host_name);
                exit (1);
            }
            // Copy the receiver node address to the socket data structure. */
            memcpy (&server_addr_out.sin_addr, hp_out -> h_addr, hp_out -> h_length);
            // Convert the integer Port Number to the network-short standardbrequired for networking stuff. This accounts for byte order differences.*/
            server_addr_out.sin_port = htons (my_deferred_table[i].port_no);
            // Establish a connection to receiver node. */
            if (connect (socket_fd_out, (struct sockaddr *) &server_addr_out, sizeof (server_addr_out)) < 0){
                printf ("    ERROR connect ERROR in Client\n");
                exit (1);
            }
            //*** End of setting up socket connection
            
            // Construct request message
            sprintf(request_message, "reply %s %d %d", my_deferred_table[i].host_name, my_deferred_table[i].port_no, 666);
            printf("    ***Send deferred reply message| To: %s; deferred_port_no: %d***\n", my_deferred_table[i].host_name, my_deferred_table[i].port_no);
            send_message (socket_fd_out, request_message, "client");
            
            // TESTING
            strcpy(my_deferred_table[i].host_name, "");
            my_deferred_table[i].port_no = 0;
            
            close (socket_fd_out);
            /************** End of Send request message TEST **************/
        } // End of "for each node in my_deferred_table"
        
        *my_request = 0;
        
        mutex_unlock(caller);
        
    } // End of LOOP_FOREVER
    
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
        printf ("    ERROR Client: fopen failed for serverAddressFile read");
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

void write_to_history_file (char host_name [], int host_port_no, int *my_ticket_no){
    FILE *history_file;
    
    history_file = fopen ("historyFile", "a");
    if (history_file == (FILE *) NULL){
        printf ("    ERROR Client: fopen failed for historyFile append");
        exit (1);
    }
    
    fprintf (history_file, "%s\n", host_name);
    fprintf (history_file, "%d\n", host_port_no);
    fprintf (history_file, "%d\n\n", *my_ticket_no);
    
    fclose (history_file);
    
    return;
}

