#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define USEC_PER_SEC (1000 * 1000)
#define SLEEP_MAX (5 * USEC_PER_SEC)

typedef pthread_mutex_t lock_t;
#define linit(l) pthread_mutex_init(l, NULL)
#define lock(l) pthread_mutex_lock(l)
#define unlock(l) pthread_mutex_unlock(l)
#define sinit(s, count) sem_init(s, 0, count)
#define wait(s) sem_wait(s)
#define signal(s) sem_post(s)

int numPatients = 0;
int sick=0; //keeps count of the number of patients that need to be treated
int registered=0; //keeps count of the number of patients that have registered
int count =0;
sem_t turnstile;
sem_t turnstile2;
sem_t turnstile3;
sem_t treated_barrier;

lock_t door;
lock_t treated;
lock_t patients;

void *doctor(void *arg) {
    int id = *((int *)arg);
    printf("Doctor %d hired\n", id);
    count=0;

    while(1) {
        // Sleep for a random time
        usleep(random() % SLEEP_MAX);

        // Enter the clinic
        lock(&door); //stand in the doorway //new patients cant enter or leave while dr is in session
        printf("Doctor %d: entered\n", id);
        count++;

        // Treat the patients (after they all have registered)
        if(registered==sick && sick != 0){
          wait(&turnstile2);
          signal(&turnstile);
          printf("Doctor %d: treated patients\n", id);
        }

        // Exit the clinic
        //  patients must all be treated
        printf("Doctor %d: exited\n", id);
        unlock(&door); //release lock on doorway
    }
    return NULL;
}

void *patient(void *arg) {
    int id = *((int *)arg);
    printf("Patient %d born\n", id);

    while(1) {
        // Sleep for a random time
        usleep(random() % SLEEP_MAX);

        // (Get infected) enter the clinic
        pthread_mutex_lock(&door); //try to use doorway
        printf("Patient %d: entered\n", id);
        sick += 1;
        pthread_mutex_unlock(&door); //step out of doorway

        // Register -- all must complete before can be treated.
        lock(&patients);
        printf("Patient %d: registered\n", id);
        registered += 1;
        unlock(&patients);

        wait(&turnstile);
        signal(&turnstile);

        lock(&patients);
        registered--;
        if (registered==0){
          wait(&turnstile);
          signal(&turnstile2);
        }
        unlock(&patients);

        wait(&turnstile2);
        signal(&turnstile2);


        // Receive treatment
        //  BARRIER - dr has to treat all before can proceed
        //  block until nth thread
        //  3.7.5 in lil book of semaphor -- reusable barrier
        printf("Patient %d: treated\n", id);

        // Receive voucher
        printf("Patient %d: voucher\n", id);

        // Exit the clinic -- cant leave before receiving treatment
        lock(&door); //try to acquire doorway lock
        printf("Patient %d: exited\n", id);
        sick--;
        unlock(&door);//release doorway lock

    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // Check and read arguments
    if (argc != 3) {
        printf("Usage: clinic <num-doctors> <num-population>\n");
        return 1;
    }
    int numDoctors = atoi(argv[1]);
    int numPopulation = atoi(argv[2]);
    sinit(&turnstile, 0);
    sinit(&turnstile2,1);
    sinit(&turnstile3,0);
    sinit(&treated_barrier,0);
    linit(&patients);
    linit(&door);
    linit(&treated);

    srandom(time(NULL));
    //srandom(1); // Seed random number generator

    // Create threads
    pthread_t doctors[numDoctors];
    pthread_t patients[numPopulation];
    for (int i = 0; i < numDoctors; i++) {
        int *j = malloc(sizeof(int));
        *j = i;
        pthread_create(&doctors[i], NULL, &doctor, j);
    }
    for (int i = 0; i < numPopulation; i++) {
        int *j = malloc(sizeof(int));
        *j = i;
        pthread_create(&patients[i], NULL, &patient, j);
    }

    // Finish main thread, but do not end program
    pthread_exit(NULL);
}
