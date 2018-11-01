#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "validation.h"
typedef struct
{
  int semaphoreID; //Also used to determine which semaphore it connects to.
  int status; //-1 for empty. 0 for scheduled. 1 for dispatched/running. 2 for blocked(i.e. waiting for the event), 3 for terminated.
  long pid;
  int queue; 
} pcb;

// log files
char* log_filename;
FILE* logFilePointer = NULL;


// shared memory
int shmClockId;
int shmBlockId;
int shmQueueId;
int msgQueueId;
void* shmClockAddress;
void* shmMsgAddress;
void handleCtrlC();
void closeFilePointers();
void deallocateMemory();
void timeoutKillAll();
void cleanUpExit();
void checkForErrors(char programName[], int errnoValue);


int main(int argc, char *argv[]){
    signal(SIGALRM, timeoutKillAll);
    signal(SIGINT, handleCtrlC);

    //allocate shared memory
    logFilePointer = fopen("logfile", "w");

    struct msgBuf buf;
    key_t shmClockKey = ftok("./CreateKeyFile", 1);
    key_t shmBlockKey = ftok("./CreateKeyFile", 2);
    key_t shmQueueKey = ftok("./CreateKeyFile", 3);
    key_t msgQueueKey = ftok("./CreateKeyFile", 4);
    checkForErrors(argv[0], errno);

    shmClockId = shmget(shmClockKey, sizeof(unsigned int) * 2, IPC_CREAT | 0644);
    shmBlockId = shmget(shmBlockKey, sizeof(pcb) * 19, IPC_CREAT | 0644);
    shmQueueId = shmget(shmQueueKey, sizeof(long) * 19, IPC_CREAT | 0644);
    msgQueueId = msgget(msgQueueKey, IPC_CREAT | 0644);
    checkForErrors(argv[0], errno);
    int pid = fork();
    if (pid == 0) {
        execl("./proc", "proc", NULL);
    }


    //NOTE: Gave up around this point. I am going to try to implement message queues for the next project
    for(;;) { 
        if (msgrcv(msgQueueId, &buf, sizeof(buf.mtext), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        fprintf(logFilePointer,"HELLO POINTER");
        printf("recieved from child!: \"%s\"\n", buf.mtext);
        wait(NULL);
        if (msgctl(msgQueueId, IPC_RMID, NULL) == -1) {
            perror("msgctl");
            exit(1);
        }
        break;
    }
    deallocateMemory();
    return 0;
}

void timeoutKillAll()
{
  fprintf(stderr,"Master process exiting due to timeout. Killing children processes\n");
  kill(0, SIGTERM);
  deallocateMemory();
  closeFilePointers();
  exit(1);
}

void handleCtrlC() {
  fprintf(stderr,"Master process exiting due to exit signal. Killing children processes\n");
  kill(0, SIGINT);
  deallocateMemory();
  exit(1);
}

void deallocateMemory() {
  if (shmClockId)
    shmctl(shmClockId, IPC_RMID, NULL);
  if (shmQueueId)
    shmctl(shmQueueId, IPC_RMID, NULL);
  if (shmBlockId)
    shmctl(shmBlockId, IPC_RMID, NULL);
  if(msgQueueId)
    msgctl(msgQueueId, IPC_RMID, NULL);
}

void closeFilePointers() {
  if (logFilePointer) {
    printf("closing logFilePointer\n");
    fclose(logFilePointer);
  }
}

void checkForErrors(char programName[], int errnoValue){
  if (errnoValue) {
    fprintf(stderr, "%s: Error: %s\n", programName, strerror(errno));
    deallocateMemory();
    exit (1);
  }
}