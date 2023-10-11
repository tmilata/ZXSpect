#ifndef _DEBUG_H
#define _DEBUG_H



void d_DispText(const char* text);
void d_debug(char *text, ...);
void d_fprintf(char *text, ...);
void d_fast_fclose();
void d_fast_fprintf(char *text, ...);

#endif // _DEBUG_H
