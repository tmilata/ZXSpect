# ZXSpect
ZX Spectrum emulator for the PicoPad console
Supported features:
-ZX Spectrum 48k and 128k emulation
-sound emulation - beeper and AY8912
-support for Z80, SNA (in both 48k and 128k modes) and TAP files (only in 48k mode) loading
-fast save and load current memory state
-user mapping of ZX keys to PicoPad keys, for each game separately with automatic saving
-support for external USB keyboard

This emulator is based on the code https://github.com/MikeDX/z80emu.
It also uses parts of code from other emulators:
QtSpecem emulator - TAP file loading,
ZX-ESPectrum - Z80 file loading,
Ziga Ramsak's ZX Spectrum - SNA file loading
GZX - George's ZX Spectrum Emulator - AY chip emulation

The code requires the SDK v1.03 or v1.04 from https://github.com/Panda381/PicoLibSDK to compile
Compilation steps:
-download and extract PicoLib SDK version 1.03 or 1.04
-install the ARM cross-compiler recommended in the PicoLib SDK
-copy the source code of this project to a directory named NEW and run the a.bat in the ZXSpec subdirectory
