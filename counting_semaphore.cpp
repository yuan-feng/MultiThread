#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_fnc(void * arg);

/* semaphores are declared global so they can be accessed
   in main() and in thread routine */
sem_t my_semaphore;
char my_string[100];  /* shared variable */

int main()
{
        int ret;
        int value;
        pthread_t my_thread;
        void *ret_join;

        /* initialize mutex to 1 - binary semaphore */
        /* second param = 0 - semaphore is local */
        ret = sem_init(&my_semaphore, 0, 0);

        sem_getvalue(&my_semaphore, &value);
        printf("The initial value of the semaphore is %d\n", value);

        if(ret != 0) {
                perror("semaphore init failed\n");
                exit(EXIT_FAILURE);
        }

        ret =  pthread_create(&my_thread, NULL, thread_fnc, NULL);
        if(ret != 0) {
                perror("pthread_create failed\n");
                exit(EXIT_FAILURE);
        }

        printf("Type in some characters. Enter 'quit' to finish\n");
        while(strncmp("quit", my_string, 4) != 0) {
                fgets(my_string, 100, stdin);
                sem_post(&my_semaphore);
                sem_getvalue(&my_semaphore, &value);
                printf("The value of the semaphore after sem_post() is %d\n", value);
        }
        printf("Waiting for thread to finish...\n");
        ret = pthread_join(my_thread, &ret_join);
        if(ret != 0) {
                perror("pthread_join failed");
                exit(EXIT_FAILURE);
        }
        printf("Thread joined, it returned %s\n", (char *) ret_join);
        sem_destroy(&my_semaphore);
        exit(EXIT_SUCCESS);
}

void *thread_fnc(void *arg)
{
        int val;

        printf("This is thread_fnc(), waiting for nonzero count...\n");
        sem_getvalue(&my_semaphore, &val);
        printf("The value of the semaphore in thread_fnc() is %d\n", val);
        sem_wait(&my_semaphore);
        sem_getvalue(&my_semaphore, &val);
        printf("The value of the semaphore after sem_wait() in thread_fnc() is %d\n", val);
        while(strncmp("quit", my_string, 4) != 0) {
                printf("You typed in %d characters\n",strlen(my_string)-1);
                sem_getvalue(&my_semaphore, &val);
                printf("The value of the semaphore before sem_wait() in thread_fnc() is %d\n", val);
                sem_wait(&my_semaphore);
                sem_getvalue(&my_semaphore, &val);
                printf("The value of the semaphore after sem_wait() in thread_fnc() is %d\n", val);
        }
        pthread_exit(NULL);
}
