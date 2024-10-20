#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 4
#define MAX_PASSWORD_LEN 100 
#define MAX_COMMAND_LEN 150
#define GPG_FILE "secret1.gpg" // After finding the first, change to "mainsecret.gpg" 
#define listlength 60000 

// Structure for thread data
typedef struct {
    int thread_id;
    char **passwords;
    int start_index;
    int end_index;
} thread_data_t;

// Function declarations
void *decrypt_file(void *threadarg);
char **read_passwords(char *filename, int *num_passwords);
void free_passwords(char **passwords, int num_passwords);

int main(int argc, char *argv[]) {
    char *dict_file = argc > 1 ? argv[1] : "/usr/share/dict/cracklib-small"; 
    int num_passwords = 0;
    char **passwords = read_passwords(dict_file, &num_passwords);
    if (passwords == NULL) {
        fprintf(stderr, "Failed to read passwords from dictionary.\n");
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    int rc, i;

    int passwords_per_thread = num_passwords / NUM_THREADS;

    for (i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].passwords = passwords;
        thread_data[i].start_index = i * passwords_per_thread;
        thread_data[i].end_index = (i == NUM_THREADS - 1) ? num_passwords : (i + 1) * passwords_per_thread;

        rc = pthread_create(&threads[i], NULL, decrypt_file, (void *)&thread_data[i]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    free_passwords(passwords, num_passwords);
    return 0;
}

void *decrypt_file(void *threadarg) {
    thread_data_t *my_data = (thread_data_t *) threadarg;
    my_data = (thread_data_t *) threadarg;
    int tid = my_data->thread_id;
    char **passwords = my_data->passwords;
    int start_index = my_data->start_index;
    int end_index = my_data->end_index;
    char command[MAX_COMMAND_LEN];
    int rc;

    for (int i = start_index; i < end_index; i++) {
        printf("Thread %d trying password: %s\n", tid, passwords[i]);
        
        snprintf(command, sizeof(command), "gpg --batch --quiet --passphrase \"%s\" -d %s > /dev/null 2>&1", passwords[i], GPG_FILE);
        rc = system(command);
        if (rc == 0) {
            printf("Thread %d found the password: %s\n", tid, passwords[i]);
            exit(0); // Found the password, exit program
        }
    }
    printf("Thread %d did not find the password.\n", tid);
    pthread_exit(NULL);
}

char **read_passwords(char *filename, int *num_passwords) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    char **passwords = malloc(listlength * sizeof(char *));
    if (!passwords) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    char line[MAX_PASSWORD_LEN + 1]; // +1 for the null terminator
    *num_passwords = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // To remove newline character
        if (strlen(line) > 0) {
            passwords[*num_passwords] = strdup(line);
            if (!passwords[*num_passwords]) {
                perror("Memory allocation failed for password string");
                for (int i = 0; i < *num_passwords; ++i) {
                    free(passwords[i]);
                }
                free(passwords);
                fclose(file);
                return NULL;
            }
            (*num_passwords)++;
        }
    }

    fclose(file);
    return passwords;
}

void free_passwords(char **passwords, int num_passwords) {
    if (passwords != NULL) {
        for (int i = 0; i < num_passwords; i++) {
            free(passwords[i]);
        }
    }
    free(passwords);
}

