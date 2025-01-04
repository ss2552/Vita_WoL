TARGET = Vita_WoL

LINK_LIBS = -lSceKernel_stub -lSceIofilemgr_stub -lSceIofilemgrForDriver_stub -lSceNet_stub -lSceSysmodule_stub -lSceCommonDialog_stub

all: eboot.bin

eboot.bin: main.c
	arm-vita-eabi-gcc -Wl,-q -Wall -O3 -I$(VITASDK)/include -L$(VITASDK)/lib $^ -lc $(LINK_LIBS) -o $@
	arm-vita-eabi-strip -g $<
	vita-elf-create $< $@
	vita-make-fself $@ $<
