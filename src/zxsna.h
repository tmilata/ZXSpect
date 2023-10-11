#ifndef _ZXSNA_H_
#define _ZXSNA_H_

int LoadSna(char *strSnaFile);
int LoadSna(char *strSnaFile, bool preserveStack,bool ayLoad);
void SaveSna(char *strFileName);
void SaveSna(char *strFileName, bool preserveStack,bool aySave);

#endif //_ZXSNA_H_
