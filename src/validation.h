#ifndef validation_h__
#define validation_h__
// union semun {
//     int val;
// };
struct msgBuf {
   long mtype;
   char mtext[200];
};
extern int isNumber(char* str);
#endif
