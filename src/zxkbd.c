#include "osdep.h"
#include "zxkbd.h"
#include "debug.h"

typedef struct {
	u8	rownum; //row position
	u8	bitmask;  //bit number
	char	name[13];	//key name
} sKeyInfo;

#define KEY_BIT0  0xfe;
#define KEY_BIT1  0xfd;
#define KEY_BIT2  0xfb;
#define KEY_BIT3  0xf7;
#define KEY_BIT4  0xef;
#define KEY_BIT7  0x7f;

sKeyInfo KeyInfo[40];


u8 getRow(int nPosition) {
	return KeyInfo[nPosition].rownum;
}

u8 getPressMask(int nPosition) {
	return (KeyInfo[nPosition].bitmask)&0xFF;
}

u8 getReleaseMask(int nPosition) {
	return (~((KeyInfo[nPosition].bitmask)&0xFF))&0xFF;
}

u8 getKeyPosition(const char* strKey) {
	u8 nRet=5; //Poslu kod Enter, kdyz nenajdu shodu
	for(int i=0;i<40;i++){
	  if(strcmp(KeyInfo[i].name,strKey)==0){
		nRet=i;
	    break;
	  }
	}
	return nRet;
}


void getKeyName(char* strName,int nPosition) {
	memcpy(strName,KeyInfo[nPosition].name,strlen(KeyInfo[nPosition].name)+1);
}

void fillKeyInfo() {
// Row B - Break/Space
	KeyInfo[0].rownum=KeyInfo[1].rownum=KeyInfo[2].rownum=KeyInfo[3].rownum=KeyInfo[4].rownum=7;
	KeyInfo[0].bitmask=KEY_BIT0;
	memcpy(KeyInfo[0].name,"Break Space",12);

	KeyInfo[1].bitmask=KEY_BIT1;
	memcpy(KeyInfo[1].name,"Symbol Shift",13);

	KeyInfo[2].bitmask=KEY_BIT2;
	memcpy(KeyInfo[2].name,"M",2);

	KeyInfo[3].bitmask=KEY_BIT3;
	memcpy(KeyInfo[3].name,"N",2);

	KeyInfo[4].bitmask=KEY_BIT4;
	memcpy(KeyInfo[4].name,"B",2);

// Row ENTER - H
	KeyInfo[5].rownum=KeyInfo[6].rownum=KeyInfo[7].rownum=KeyInfo[8].rownum=KeyInfo[9].rownum=6;
	KeyInfo[5].bitmask=KEY_BIT0;
	memcpy(KeyInfo[5].name,"Enter",6);

	KeyInfo[6].bitmask=KEY_BIT1;
	memcpy(KeyInfo[6].name,"L",2);

	KeyInfo[7].bitmask=KEY_BIT2;
	memcpy(KeyInfo[7].name,"K",2);

	KeyInfo[8].bitmask=KEY_BIT3;
	memcpy(KeyInfo[8].name,"J",2);

	KeyInfo[9].bitmask=KEY_BIT4;
	memcpy(KeyInfo[9].name,"H",2);


// Row P - Y
	KeyInfo[10].rownum=KeyInfo[11].rownum=KeyInfo[12].rownum=KeyInfo[13].rownum=KeyInfo[14].rownum=5;
	KeyInfo[10].bitmask=KEY_BIT0;
	memcpy(KeyInfo[10].name,"P",2);

	KeyInfo[11].bitmask=KEY_BIT1;
	memcpy(KeyInfo[11].name,"O",2);

	KeyInfo[12].bitmask=KEY_BIT2;
	memcpy(KeyInfo[12].name,"I",2);

	KeyInfo[13].bitmask=KEY_BIT3;
	memcpy(KeyInfo[13].name,"U",2);

	KeyInfo[14].bitmask=KEY_BIT4;
	memcpy(KeyInfo[14].name,"Y",2);

// Row 0 - 6
	KeyInfo[15].rownum=KeyInfo[16].rownum=KeyInfo[17].rownum=KeyInfo[18].rownum=KeyInfo[19].rownum=4;
	KeyInfo[15].bitmask=KEY_BIT0;
	memcpy(KeyInfo[15].name,"0",2);

	KeyInfo[16].bitmask=KEY_BIT1;
	memcpy(KeyInfo[16].name,"9",2);

	KeyInfo[17].bitmask=KEY_BIT2;
	memcpy(KeyInfo[17].name,"8",2);

	KeyInfo[18].bitmask=KEY_BIT3;
	memcpy(KeyInfo[18].name,"7",2);

	KeyInfo[19].bitmask=KEY_BIT4;
	memcpy(KeyInfo[19].name,"6",2);

// Row 1 - 5
	KeyInfo[20].rownum=KeyInfo[21].rownum=KeyInfo[22].rownum=KeyInfo[23].rownum=KeyInfo[24].rownum=3;
	KeyInfo[20].bitmask=KEY_BIT0;
	memcpy(KeyInfo[20].name,"1",2);

	KeyInfo[21].bitmask=KEY_BIT1;
	memcpy(KeyInfo[21].name,"2",2);

	KeyInfo[22].bitmask=KEY_BIT2;
	memcpy(KeyInfo[22].name,"3",2);

	KeyInfo[23].bitmask=KEY_BIT3;
	memcpy(KeyInfo[23].name,"4",2);

	KeyInfo[24].bitmask=KEY_BIT4;
	memcpy(KeyInfo[24].name,"5",2);

// Row Q - T
	KeyInfo[25].rownum=KeyInfo[26].rownum=KeyInfo[27].rownum=KeyInfo[28].rownum=KeyInfo[29].rownum=2;
	KeyInfo[25].bitmask=KEY_BIT0;
	memcpy(KeyInfo[25].name,"Q",2);

	KeyInfo[26].bitmask=KEY_BIT1;
	memcpy(KeyInfo[26].name,"W",2);

	KeyInfo[27].bitmask=KEY_BIT2;
	memcpy(KeyInfo[27].name,"E",2);

	KeyInfo[28].bitmask=KEY_BIT3;
	memcpy(KeyInfo[28].name,"R",2);

	KeyInfo[29].bitmask=KEY_BIT4;
	memcpy(KeyInfo[29].name,"T",2);

// Row A - G
	KeyInfo[30].rownum=KeyInfo[31].rownum=KeyInfo[32].rownum=KeyInfo[33].rownum=KeyInfo[34].rownum=1;
	KeyInfo[30].bitmask=KEY_BIT0;
	memcpy(KeyInfo[30].name,"A",2);

	KeyInfo[31].bitmask=KEY_BIT1;
	memcpy(KeyInfo[31].name,"S",2);

	KeyInfo[32].bitmask=KEY_BIT2;
	memcpy(KeyInfo[32].name,"D",2);

	KeyInfo[33].bitmask=KEY_BIT3;
	memcpy(KeyInfo[33].name,"F",2);

	KeyInfo[34].bitmask=KEY_BIT4;
	memcpy(KeyInfo[34].name,"G",2);

// Row Caps Shift - V
	KeyInfo[35].rownum=KeyInfo[36].rownum=KeyInfo[37].rownum=KeyInfo[38].rownum=KeyInfo[39].rownum=0;
	KeyInfo[35].bitmask=KEY_BIT0;
	memcpy(KeyInfo[35].name,"Caps Shift",11);

	KeyInfo[36].bitmask=KEY_BIT1;
	memcpy(KeyInfo[36].name,"Z",2);

	KeyInfo[37].bitmask=KEY_BIT2;
	memcpy(KeyInfo[37].name,"X",2);

	KeyInfo[38].bitmask=KEY_BIT3;
	memcpy(KeyInfo[38].name,"C",2);

	KeyInfo[39].bitmask=KEY_BIT4;
	memcpy(KeyInfo[39].name,"V",2);

}


