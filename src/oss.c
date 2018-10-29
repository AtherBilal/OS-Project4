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
void timeoutKillAll();
void cleanUpExit();
void checkForErrors(char programName[], int errnoValue);
void checkTotalMaxProcessesLimitReached(int totalProcessCount, char programName[]);
void printUsageMsg(char programName[], int errorOut);


int main(int argc, char *argv[]){
    printf("hello world\n");
}