#include "zxsnd.h"
#include "zxem.h"

#define SND_BUFF 1768
unsigned char sndBufffer[SND_BUFF];

typedef struct {
	int nReadPos;
	int nWritePos;
	u64 nReadAbsolutePos;
	u64 nWriteAbsolutePos;
} buffStatus;

buffStatus bs;
int nVolume;
int nWaveMaxValue;
int nWaveMinValue;

void sndBufferCear() {
	memset(sndBufffer, 128, SND_BUFF);
}

void sndBufferInit() {
	sndBufferCear();
	bs.nReadPos = 0;
	bs.nWritePos = 0;
	bs.nReadAbsolutePos = 0;
	bs.nWriteAbsolutePos = 0;
}

void sndSyncReadWritepos(u64 nAbsolutePos) {
	sndBufferCear();
	bs.nReadAbsolutePos = nAbsolutePos;
	bs.nWriteAbsolutePos = nAbsolutePos;
	bs.nWritePos = nAbsolutePos % SND_BUFF;
	bs.nReadPos = bs.nWritePos;
}

u8 getSndByte() {
	if (bs.nReadAbsolutePos >= bs.nWriteAbsolutePos) {
		//d_fprintf("rdabs=%i>=wrtabs=%i\n",bs.nReadAbsolutePos,bs.nWriteAbsolutePos);
		return 128;
	}
	u8 nRet = sndBufffer[bs.nReadPos];
	bs.nReadPos++;
	bs.nReadAbsolutePos++;
	if (bs.nReadPos >= SND_BUFF) {
		bs.nReadPos = 0;
	}
	//d_fprintf("send=%i\n",nRet);
	return nRet;
}

bool writeSndByte(u64 nAbsolutePos, u8 nByte) {
	if (nByte > 0) {
		nByte = nWaveMaxValue;
	} else {
		nByte = nWaveMinValue;
	}
	int nSize = nAbsolutePos - bs.nWriteAbsolutePos;
	if (nSize < 0)
		return false;
	if (nSize > SND_BUFF)
		return false;
	if ((bs.nReadAbsolutePos < bs.nWriteAbsolutePos)
			&& (nSize + bs.nWriteAbsolutePos - bs.nReadAbsolutePos >= SND_BUFF))
		return false;
	//muzu zapsat
	if ((bs.nWritePos + nSize) > SND_BUFF) {
		//zpet na zacatek bufferu
		memset(&sndBufffer[bs.nWritePos], nByte, SND_BUFF - bs.nWritePos);
		memset(&sndBufffer[0], nByte, nSize - (SND_BUFF - bs.nWritePos));
		bs.nWritePos = nSize - (SND_BUFF - bs.nWritePos);
		bs.nWriteAbsolutePos = nAbsolutePos;
	} else {
		memset(&sndBufffer[bs.nWritePos], nByte, nSize);
		bs.nWritePos += nSize;
		bs.nWriteAbsolutePos = nAbsolutePos;
	}
	return true;
}

void sndFinishFrame(u64 uTotalCycles) {
	u64 nPos = (u64) ((441 * uTotalCycles) / (u64) CYCLES_PER_STEP);
	//d_fast_fprintf("frame total=%llu\n",nPos);
	//pokud se prehravany zvuk priliz vzdalil od nacachovaneho, vycisti cache a udelej drop
	if (nPos - bs.nWriteAbsolutePos >= SND_BUFF) {
		sndSyncReadWritepos(nPos);
	}
}

bool bSndOn;
//prehrava bajty z bufferu
void SndIrq() {
	PWM_IntClear(PWMSND_SLICE);
	if (nVolume > 0) {
		if (bSndOn) {
			PWM_Comp(PWMSND_SLICE, PWMSND_CHAN, (u8) getSndByte());
		}
	}
}

void SndInit() {
	GlobalSoundOff = False;
	sndBufferInit();
	bSndOn = true;
	PWM_Reset(PWMSND_SLICE);
	PWM_GpioInit(PWMSND_GPIO);
	for (int i = 0; i < USE_PWMSND; i++)
		SoundCnt[i] = 0;
	SetHandler(IRQ_PWM_WRAP, SndIrq);
	NVIC_IRQEnable(IRQ_PWM_WRAP);
	PWM_Clock(PWMSND_SLICE, PWMSND_CLOCK);
	PWM_Top(PWMSND_SLICE, PWMSND_TOP);
	PWM_Comp(PWMSND_SLICE, PWMSND_CHAN, 128);
	PWM_IntEnable(PWMSND_SLICE);
	PWM_Enable(PWMSND_SLICE);
}

void setVolume(int value) {
	nVolume = value;
	int nWavVal = (nVolume * 128) / 9;
	nWaveMaxValue = 128 + nWavVal;
	if (nWaveMaxValue > 255)
		nWaveMaxValue = 255;
	nWaveMinValue = 128 - nWavVal;
	if (nWaveMinValue < 0)
		nWaveMinValue = 0;
}

int getVolume() {
	return nVolume;
}

void sndOff() {
	bSndOn = false;
}

void sndOn() {
	bSndOn = true;
}

