TARGET = Vita_WoL

VITASDK=/usr/local/vitasdk

LINK_LIBS = -lSceKernel_stub -lSceIofilemgr_stub -lSceIofilemgrForDriver_stub -lSceNet_stub -lSceSysmodule_stub -lSceCommonDialog_stub

all: eboot.bin

eboot.bin: main.c
	$($VITASDK)/bin/arm-vita-eabi-gcc -Wl,-q -Wall -O3 -I$(VITASDK)/include -L$(VITASDK)/lib main.c -lc $(LINK_LIBS) -o main.elf
	$($VITASDK)/bin/arm-vita-eabi-strip -g main.elf
	$($VITASDK)/bin/vita-elf-create main.elf main.velf
	$($VITASDK)/bin/vita-make-fself main.velf eboot.bin
