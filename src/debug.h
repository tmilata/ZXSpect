#ifndef _DEBUG_H
#define _DEBUG_H

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;		// on 8-bit system use "signed int"
typedef unsigned short u16;		// on 8-bit system use "unsigned int"
typedef signed long int s32;		// on 64-bit system use "signed int"
typedef unsigned long int u32;		// on 64-bit system use "unsigned int"
typedef signed long long int s64;
typedef unsigned long long int u64;


// open file/directory descriptor (39 bytes)
typedef struct {
	char	name[11];	// short file name (0=not open)
	u8	attr;		// attributes (ATTR_RO,..., ATTR_NONE=file not exist)
	void*	dir;		// pointer to directory entry in disk buffer
	u32	dirsect;	// sector with directory entry
	u16	wtime;		// last write time
	u16	wdate;		// last write date
	u32	size;		// file size
	u32	sclust;		// start cluster
	u32	off;		// current read/write offset
	u32	clust;		// current read/write cluster
	u32	sect;		// current read/write sector (0=end of directory)
} _sFile;


typedef struct debugstate {
  int DispX, DispY; // X and Y text coordinate
  int nOpened;
  int nNeedFlush;
  _sFile fst1;
} debugstate;


void d_DispText(const char* text);
void d_debug(char *text, ...);
void d_fprintf(char *text, ...);
void d_fast_fclose();
void d_fast_fprintf(char *text, ...);

#endif // _DEBUG_H
