#include "threadIO.h"

void mutex_lock (char caller []){
    while (sem_wait (sem) == -1){
        if (errno != EINTR){
            printf ("ERROR %s: sem_wait failed\n", caller);
            exit (1);
        }
    }
    
    return;
}

void mutex_unlock (char caller []){
    if (sem_post (sem) == -1){
        printf ("ERROR %s: sem_post failed\n", caller);
        exit (1);
    }
    
    return;
}

void send_message (int socket_fd, char send_line [], char caller []){
    int i;
    int n;
    
    n = strlen (send_line);
    if ((i = write_n (socket_fd, send_line, n)) != n){
        printf ("ERROR %s: write_n failed in send_message", caller);
        exit (1);
    }
    
    return;
}

void get_message (int socket_fd, char recv_line [], char caller []){
    int i;
    int n;
    
    for ( ; ; ){
        n = read_line (socket_fd, recv_line, MAX_LINE_SIZE);
        if (n == 0){
            break;
        }
        if (n < 0){
            printf ("ERROR %s: read_line failed in get_message", caller);
            exit (1);
        }
    }
    
    return;
}

int read_line (int fd, char *ptr, int line_size){
    int n;
    int rc;
    char c;
    
    for (n = 1; n < line_size; n++){
        if ((rc = read_n (fd, &c, 1)) == 1){
            *ptr++ = c;
            if (c == '\n'){
                break;
            }
        }
        else if (rc == 0){
            if (n == 1){
                return (0);
            }
            else{
                break;
            }
        }
        else{
            return (-1);
        }
    }
    *ptr = 0;
    
    return (n);
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

int read_n (int fd, char *ptr, int n_bytes){
    int n_left;
    int n_read;
    
    n_left = n_bytes;
    
    while (n_left > 0){
        n_read = read (fd, ptr, n_left);
        if (n_read < 0){
            return (n_read);
        }
        else if (n_read == 0){
            break;
        }
        n_left = n_left - n_read;
        ptr = ptr + n_read;
    }
    
    return (n_bytes - n_left);
}







