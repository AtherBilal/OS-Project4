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


// log files
char* log_filename;
FILE* logFilePointer = NULL;

// shared memory
int shmClockId;
int shmMsgId;
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
void timeOutExit();
void cleanUpExit();
void checkForErrors(char programName[], int errnoValue);
void checkTotalMaxProcessesLimitReached(int totalProcessCount, char programName[]);
void printUsageMsg(char programName[], int errorOut);
int msgQueueId;


int main(int argc, char *argv[]){
    signal(SIGALRM, timeOutExit);
    signal(SIGINT, handleCtrlC);
    key_t msgQueueKey = ftok("./CreateKeyFile", 4);
    msgQueueId = msgget(msgQueueKey, 0644);

    struct msgBuf message;
    message.mtype = 1;
    memset(&(message.mtext), 0, 200 * sizeof(char));
    (void)strcpy(message.mtext, "SORRY FOR LETTING YOU DOWN MARK BUT I CAN DO MSG QUEUES NOW");
    msgsnd(msgQueueId, &message, sizeof(long) + (strlen(message.mtext) * sizeof(char)) + 1, 0);
    return 0;
}

void handleCtrlC() {
  fprintf(stderr, "PID: %ld interrupted by signal. Exitting!\n", (long)getpid());
  exit(1);
}

void timeOutExit() {
  fprintf(stderr, "PID: %ld timing out because of signal from master\n", (long)getpid());
  exit(1);
}
