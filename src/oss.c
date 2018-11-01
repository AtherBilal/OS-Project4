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
int maxRunTime = -1;
//Semaphore
int semID;
// union semun {
//     int val;
// };
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


    //NOTE: Gave up around this point. I am going to try to implement message queues for the next project
    // printf("creating semaphore\n");
    // semID = semget(semKey, 1, IPC_CREAT | 0666);
    // checkForErrors(argv[0], errno);
    // union semun semArg;
    // semArg.val = 1;
    // semctl(semID, 0, SETVAL, semArg);

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
//   if (shmClockAddress){
//     shmdt(shmClockAddress);
//   }
//   semctl(semID, 0, IPC_RMID);
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