#include "zxsnd.h"
#include "zxem.h"
#include "debug.h"
#include "ay8912.h"


unsigned char sndBeeperBufffer[SND_BUFF];

int nWrited = 0;

typedef struct {
	int nReadPos;
	int nWritePos;
	u64 nReadAbsolutePos;
	u64 nWriteAbsolutePos;
} buffStatus;

//beeper buffer
buffStatus bs;

int nVolume;
int nWaveMaxValue;
int nWaveMinValue;

void sndBufferClear() {
	memset(sndBeeperBufffer, beeper > 0 ? nWaveMaxValue : nWaveMinValue, SND_BUFF);
}

void sndBufferInit() {
	sndBufferClear();
	bs.nReadPos = 0;
	bs.nWritePos = 0;
	bs.nReadAbsolutePos = 0;
	bs.nWriteAbsolutePos = 0;
}

void sndSyncReadWritepos(u64 nAbsolutePos) {
	//d_fprintf("+snc\n");
	sndBufferClear();
	bs.nReadAbsolutePos = nAbsolutePos;
	bs.nWriteAbsolutePos = nAbsolutePos;
	bs.nWritePos = nAbsolutePos % SND_BUFF;
	bs.nReadPos = bs.nWritePos;
}

u8 getSndByte() {
	if (bs.nReadAbsolutePos >= bs.nWriteAbsolutePos) {
		return beeper > 0 ? nWaveMaxValue : nWaveMinValue;
	}
	u8 nRet = sndBeeperBufffer[bs.nReadPos];
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
		memset(&sndBeeperBufffer[bs.nWritePos], nByte, SND_BUFF - bs.nWritePos);
		memset(&sndBeeperBufffer[0], nByte, nSize - (SND_BUFF - bs.nWritePos));
		bs.nWritePos = nSize - (SND_BUFF - bs.nWritePos);
		bs.nWriteAbsolutePos = nAbsolutePos;
	} else {
		memset(&sndBeeperBufffer[bs.nWritePos], nByte, nSize);
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

//play bytes via IRQ
void SndIrq() {
	PWM_IntClear(PWMSND_SLICE);
	if (nVolume > 0) {
		if (bSndOn) {

			u16 aySmpl = 0;
			if (ay0_enable){
				aySmpl = (u16) ay_get_sample(&ay0);
			}
			u16 bprSmpl = (u16) getSndByte();
			//mix beeper sound with AY sound
			u16 samp = (aySmpl + bprSmpl) / 2;
			//slight sound amplification
			//samp = (5*samp)/4;

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

