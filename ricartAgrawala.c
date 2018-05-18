#include "ricartAgrawala.h"

int main (int argc, char *argv []){
    // Declare [segment_ids] array. Each element of [segment_ids] will contain a pointer to an area of memory, called a segment, which will be associated with a variable name and provide storage for the data type and size of that variable.
    int segment_ids [MAX_SHARED_MEMORY_SEGMENTS];
    int segment_counter = 0;

    // Allocates and initializes space for the eight variables that will be shared by the client and server on a node
    allocate_shared_memory (segment_ids, &segment_counter);
    
    // Initialize semaphore
    initialize_semaphore ();
    
    // Creation of one client and one server threads
    create_client_and_server_threads ();
    
    //At this point, main just waits for the client and server threads to finish (occurs after the server receives a shutdown message from environment).
    wait_for_threads_to_finish ();
    
    // After the client and server threads have finished, main destroys the semaphore, deallocates the variables in shared memory, and terminates.
    destroy_semaphore ();
    deallocate_shared_memory (segment_ids, &segment_counter);
    remove ("serverAddressFile");

    return 0;
}

void allocate_shared_memory (int segment_ids [], int *segment_counter){
    char parameter [MAX_LINE_SIZE];
    char file_name [MAX_LINE_SIZE];

    /* sem */
    generate_ftok_parameter (parameter);
    generate_ftok_file_name (file_name, parameter);
    segment_key = ftok (file_name, atoi (parameter));
    remove (file_name);
    segment_size = sizeof (sem_t);
    if ((segment_id = shmget (segment_key, segment_size, segment_permissions)) == -1){
        printf ("ERROR RicartAgrawala: shmget failed for sem\n");
        exit (1);
    }
    if ((sem = (sem_t *) shmat (segment_id, (const void *) NULL, 0)) == (sem_t *) -1){
        printf ("ERROR RicartAgrawala: shmat failed for sem\n");
        exit (1);
    }
    segment_ids [0] = segment_id;

    /* my_server_ready */
    generate_ftok_parameter (parameter);
    generate_ftok_file_name (file_name, parameter);
    segment_key = ftok (file_name, atoi (parameter));
    remove (file_name);
    segment_size = sizeof (int);
    if ((segment_id = shmget (segment_key, segment_size, segment_permissions)) == -1){
        printf ("ERROR RicartAgrawala: shmget failed for my_server_ready\n");
        exit (1);
    }
    if ((my_server_ready = (int *) shmat (segment_id, (const void *) NULL, 0)) == (int *) -1){
        printf ("ERROR RicartAgrawala: shmat failed for my_server_ready\n");
        exit (1);
    }
    segment_ids [1] = segment_id;

    /* my_ticket_no */
    generate_ftok_parameter (parameter);
    generate_ftok_file_name (file_name, parameter);
    segment_key = ftok (file_name, atoi (parameter));
    remove (file_name);
    segment_size = sizeof (int);
    if ((segment_id = shmget (segment_key, segment_size, segment_permissions)) == -1){
        printf ("ERROR RicartAgrawala: shmget failed for my_ticket_no\n");
        exit (1);
    }
    if ((my_ticket_no = (int *) shmat (segment_id, (const void *) NULL, 0)) == (int *) -1){
        printf ("ERROR RicartAgrawala: shmat failed for my_ticket_no\n");
        exit (1);
    }
    segment_ids [2] = segment_id;

    /* my_highest_ticket_no */
    generate_ftok_parameter (parameter);
    generate_ftok_file_name (file_name, parameter);
    segment_key = ftok (file_name, atoi (parameter));
    remove (file_name);
    segment_size = sizeof (int);
    if ((segment_id = shmget (segment_key, segment_size, segment_permissions)) == -1){
        printf ("ERROR RicartAgrawala: shmget failed for my_highest_ticket_no\n");
        exit (1);
    }
    if ((my_highest_ticket_no = (int *) shmat (segment_id, (const void *) NULL, 0)) == (int *) -1){
        printf ("ERROR RicartAgrawala: shmat failed for my_highest_ticket_no\n");
        exit (1);
    }
    segment_ids [3] = segment_id;

    /* my_request */
    generate_ftok_parameter (parameter);
    generate_ftok_file_name (file_name, parameter);
    segment_key = ftok (file_name, atoi (parameter));
    remove (file_name);
    segment_size = sizeof (int);
    if ((segment_id = shmget (segment_key, segment_size, segment_permissions)) == -1){
        printf ("ERROR RicartAgrawala: shmget failed for my_request\n");
        exit (1);
    }
    if ((my_request = (int *) shmat (segment_id, (const void *) NULL, 0)) == (int *) -1){
        printf ("ERROR RicartAgrawala: shmat failed for my_request\n");
        exit (1);
    }
    segment_ids [4] = segment_id;

    /* my_replies */
    generate_ftok_parameter (parameter);
    generate_ftok_file_name (file_name, parameter);
    segment_key = ftok (file_name, atoi (parameter));
    remove (file_name);
    segment_size = sizeof (int);
    if ((segment_id = shmget (segment_key, segment_size, segment_permissions)) == -1){
        printf ("ERROR RicartAgrawala: shmget failed for my_replies\n");
        exit (1);
    }
    if ((my_replies = (int *) shmat (segment_id, (const void *) NULL, 0)) == (int *) -1){
        printf ("ERROR RicartAgrawala: shmat failed for my_replies\n");
        exit (1);
    }
    segment_ids [5] = segment_id;

    /* my_deferred_table */
    generate_ftok_parameter (parameter);
    generate_ftok_file_name (file_name, parameter);
    segment_key = ftok (file_name, atoi (parameter));
    remove (file_name);
    segment_size = (sizeof (char) * MAX_LINE_SIZE + sizeof (int)) * MAX_SERVERS;
    if ((segment_id = shmget (segment_key, segment_size, segment_permissions)) == -1){
        printf ("ERROR RicartAgrawala: shmget failed for my_deferred_table\n");
        exit (1);
    }
    if ((my_deferred_table_address = (char *) shmat (segment_id, (const void *) NULL, 0)) == (char *) -1){
        printf ("ERROR RicartAgrawala: shmat failed for my_deferred_table\n");
        exit (1);
    }
    my_deferred_table = (struct server_address *) (void *) my_deferred_table_address;
    segment_ids [6] = segment_id;

    /* my_deferred_count */
    generate_ftok_parameter (parameter);
    generate_ftok_file_name (file_name, parameter);
    segment_key = ftok (file_name, atoi (parameter));
    remove (file_name);
    segment_size = sizeof (int);
    if ((segment_id = shmget (segment_key, segment_size, segment_permissions)) == -1){
        printf ("ERROR RicartAgrawala: shmget failed for my_deferred_count\n");
        exit (1);
    }
    if ((my_deferred_count = (int *) shmat (segment_id, (const void *) NULL, 0)) == (int *) -1){
        printf ("ERROR RicartAgrawala: shmat failed for my_deferred_count\n");
        exit (1);
    }
    segment_ids [7] = segment_id;

    *segment_counter = 8;

    return;
}

void generate_ftok_parameter (char parameter []){
    FILE *ftok_parameter_file;
    int next_parameter;
    int lock;
    struct flock key;

    key.l_type = F_WRLCK;
    key.l_whence = SEEK_SET;
    key.l_start = 0;
    key.l_len = 0;
    key.l_pid = getpid ();
    lock = open ("ftok_parameter_file_lock", O_WRONLY);
    fcntl (lock, F_SETLKW, &key);

    ftok_parameter_file = fopen ("ftokParameterFile", "r");
    if (ftok_parameter_file == (FILE *) NULL){
        printf ("ERROR RicartAgrawala: fopen failed for ftokParameterFile read");
        exit (1);
    }
    fscanf (ftok_parameter_file, "%s", parameter);
    fclose (ftok_parameter_file);
    remove ("ftokParameterFile");

    ftok_parameter_file = fopen ("ftokParameterFile", "w");
    if (ftok_parameter_file == (FILE *) NULL){
        printf ("ERROR RicartAgrawala: fopen failed for ftokParameterFile write");
        exit (1);
    }
    next_parameter = atoi (parameter);
    if (next_parameter == INT_MAX){
        next_parameter = 0;
    }
    next_parameter ++;
    fprintf (ftok_parameter_file, "%d\n", next_parameter);
    fclose (ftok_parameter_file);

    key.l_type = F_UNLCK;
    fcntl (lock, F_SETLK, &key);
    close (lock);

    return;
}

void generate_ftok_file_name (char file_name [], char parameter []){
    FILE *ftok_file;

    strcpy (file_name, getenv ("USER"));
    strcat (file_name, parameter);

    ftok_file = fopen (file_name, "w");
    if (ftok_file == (FILE *) NULL){
        printf ("ERROR RicartAgrawala: fopen failed for %s write", file_name);
        exit (1);
    }
    fprintf (ftok_file, "%s\n", file_name);
    fclose (ftok_file);

    return;
}

void initialize_semaphore (){
    if (sem_init (sem, 1, 1) == -1){
        perror ("ERROR RicartAgrawala: sem_init failed for sem");
        exit (1);
    }

    return;
}

void create_client_and_server_threads (){
    int error;

    tids = (pthread_t *) calloc (2, sizeof (pthread_t));
    if (tids == (pthread_t *) NULL){
        perror ("ERROR RicartAgrawala: calloc failed to allocate memory for thread IDs");
        exit (1);
    }

    if (error = pthread_create (tids, (const pthread_attr_t *) NULL, server_thread, (void *) NULL)){
        printf ("ERROR RicartAgrawala: pthread_create failed to create server_thread (%s)\n", strerror (error));
        exit (1);
    }

    if (error = pthread_create (tids + 1, (const pthread_attr_t *) NULL, client_thread, (void *) NULL)){
        printf ("ERROR RicartAgrawala: pthread_create failed to create client_thread (%s)\n", strerror (error));
        exit (1);
    }

    return;
}

void wait_for_threads_to_finish (){
    int i;
    int error;

    if (error = pthread_join (tids [0], (void **) NULL)){
        printf ("ERROR RicartAgrawala: pthread_join failed to join server_thread (%s)\n", strerror (error));
        exit (1);
    }

    return;
}

void destroy_semaphore (){
    if (sem_destroy (sem) == -1){
        perror ("ERROR RicartAgrawala: sem_destroy failed to destroy sem");
        exit (1);
    }

    return;
}

void deallocate_shared_memory (int segment_ids [], int *segment_counter){
    int i;

    for (i = 0; i < *segment_counter; i ++){
        if (shmctl (segment_ids [i], IPC_RMID, (struct shmid_ds *) NULL) == -1){
            printf ("ERROR RicartAgrawala: shmctl failed to deallocate shared memory for segment %d\n", i);
        }
    }

    return;
}
