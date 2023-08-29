#include "zxsnd.h"
#include "zxem.h"
#include "debug.h"
#include "ay8912.h"

#define ONE_FRAME 441
#define SND_BUFF 8*ONE_FRAME
unsigned char sndBufffer[SND_BUFF];

int nWrited = 0;

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
	memset(sndBufffer, beeper > 0 ? nWaveMaxValue : nWaveMinValue, SND_BUFF);
}

void sndBufferInit() {
	sndBufferCear();
	bs.nReadPos = 0;
	bs.nWritePos = 0;
	bs.nReadAbsolutePos = 0;
	bs.nWriteAbsolutePos = 0;
}

void sndSyncReadWritepos(u64 nAbsolutePos) {
	//d_fprintf("+snc\n");
	sndBufferCear();
	bs.nReadAbsolutePos = nAbsolutePos;
	bs.nWriteAbsolutePos = nAbsolutePos;
	bs.nWritePos = nAbsolutePos % SND_BUFF;
	bs.nReadPos = bs.nWritePos;
}

u8 getSndByte() {
	if (bs.nReadAbsolutePos >= bs.nWriteAbsolutePos) {
		return beeper > 0 ? nWaveMaxValue : nWaveMinValue;
	}
	u8 nRet = sndBufffer[bs.nReadPos];
	bs.nReadPos++;
	bs.nReadAbsolutePos++;
	if (bs.nReadPos >= SND_BUFF) {
		bs.nReadPos = 0;
	}
	//d_fprintf("send=%i\n", nRet);
	return nRet;
}

bool writeSndByte(u64 nAbsolutePos, u8 nByte) {
	//d_fprintf("+wrt\n");
	nWrited = 1;
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
	if ((bs.nReadAbsolutePos < bs.nWriteAbsolutePos) && (nSize + bs.nWriteAbsolutePos - bs.nReadAbsolutePos >= SND_BUFF))
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
	//d_fprintf("*%llu\n", bs.nWriteAbsolutePos);
	return true;
}

void sndFinishFrame(u64 uTotalCycles) {
	u64 nPos = (u64) ((ONE_FRAME * uTotalCycles) / (u64) CYCLES_PER_STEP);
	//pokud nebyl zadny zapis v tomto frame, tak proved zapis alespon 1 bajtu aby se writepointer prilis nerozesel s readpointerem
	if (nWrited == 0)
		writeSndByte(nPos, beeper);
	//pokud se prehravany zvuk priliz vzdalil od nacachovaneho, vycisti cache a udelej drop
	if (nPos - bs.nWriteAbsolutePos >= SND_BUFF) {
		//d_debug("-%i",nPos - bs.nWriteAbsolutePos);
		sndSyncReadWritepos(nPos);
	}
	nWrited = 0;
}

bool bSndOn;
//for silent on device detection
u8 nLastBeep = 0;
u8 nLastAy = 0;
u8 nPrimaryDevice = 0;
unsigned long long int nLastBeepCnt = 0;
unsigned long long int nLastAyCnt = 0;
//play bytes via IRQ
void SndIrq() {
	PWM_IntClear(PWMSND_SLICE);
	if (nVolume > 0) {
		if (bSndOn) {
			//get AY sample
			u16 aySmpl = 0;
			if (ay0_enable)
				aySmpl = (u16) ay_get_sample(&ay0);
			//increase or reset silent counter
			if (aySmpl != nLastAy) {
				nLastAy = aySmpl;
				nLastAyCnt = 0;
				nPrimaryDevice=1;
			} else {
				nLastAyCnt++;
			}
			//get Beeper sample
			u16 bprSmpl = (u16) getSndByte();
			//increase or reset silent counter
			if (bprSmpl != nLastBeep) {
				nLastBeep = bprSmpl;
				nLastBeepCnt = 0;
				nPrimaryDevice=0;
			} else {
				nLastBeepCnt++;
			}

			//mix beeper sound with AY sound
			u16 samp= (aySmpl + bprSmpl) - (aySmpl * bprSmpl) / 256;
			//if the device is unchanged for a long time, it should be removed from the mixing otherwise it may distort the overall sound.
			//For example. Fuka's Tetris has the beeper in high mode (bit=1) all the time and thus distorts the sound from the AY
			int nSilentLimit = 2*ONE_FRAME;
			if ((nLastBeepCnt > nSilentLimit)&&(nPrimaryDevice==1)) {
				samp = aySmpl;
			}
			if ((nLastAyCnt > nSilentLimit)&&(nPrimaryDevice==0)) {
				samp = bprSmpl;
			}

			if (samp > 255)
				samp = 255;
			PWM_Comp(PWMSND_SLICE, PWMSND_CHAN, samp);
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

