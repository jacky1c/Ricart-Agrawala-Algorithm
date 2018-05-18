#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/file.h>
#include <limits.h>

#include "globals.h"

#define MAX_SHARED_MEMORY_SEGMENTS 16

struct server_address{
    char host_name [MAX_LINE_SIZE];
    int port_no;
};

int segment_id;
key_t segment_key;
int segment_permissions = SHM_R | SHM_W | IPC_CREAT | IPC_EXCL;
size_t segment_size;

pthread_t *tids;

void *server_thread (void *args); // defined in serverThread.c
void *client_thread (void *args); // defined in clientThread.c

// Used by the mutex_lock and mutex_unlock procedures in threadIO.c to synchronize access to variables shared by the client and server on the same node
sem_t *sem;

// Used when a node is starting to prevent the client from sending request messages to remote servers before the local server receives the start message from environment.
int *my_server_ready = 0;

// *** Variables used for RicartAgrawala algorithm ***
int *my_ticket_no = 0;
int *my_highest_ticket_no = 0;
int *my_request = 0; // the same as my_request_cs in Assignment6.doc?
int *my_replies = 0;
struct server_address *my_deferred_table; // the same as my_deferred_table in Assignment6.doc?
int *my_deferred_count = 0; // The number of elements in my_deferred_table
// *** End of Variables used for RicartAgrawala algorithm ***


char *my_deferred_table_address;


void allocate_shared_memory (int segment_ids [], int *segment_counter);
void generate_ftok_parameter (char parameter []);
void generate_ftok_file_name (char file_name [], char parameter []);
void initialize_semaphore ();
void create_client_and_server_threads ();
void wait_for_threads_to_finish ();
void destroy_semaphore ();
void deallocate_shared_memory (int segment_ids [], int *segment_counter);
