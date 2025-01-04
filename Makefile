#
# Copyright (c) 2015 Sergi Granell (xerpi)
# based on Cirne's vita-toolchain test Makefile
#

TARGET = Vita_WoL
OBJS := main.o

LIBS = -lc -lSceKernel_stub -lSceIofilemgr_stub -lSceIofilemgrForDriver_stub -lSceNet_stub -lSceSysmodule_stub -lSceCommonDialog_stub

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
READELF = $(PREFIX)-readelf
OBJDUMP = $(PREFIX)-objdump
CFLAGS  = -Wl,-q -Wall -O3 -I$(VITASDK)/include -L$(VITASDK)/lib
ASFLAGS = $(CFLAGS)

all: $(TARGET).velf

%.velf: %.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@ > /dev/null

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS)

copy: $(TARGET).velf
	@cp $(TARGET).velf ~/shared/vitasample.elf
	@echo "Copied!"

run: $(TARGET).velf
	@sh run_homebrew_unity.sh $(TARGET).velf

send: $(TARGET).velf
	curl -T $(TARGET).velf ftp://$(PSVITAIP):1337/cache0:/
	@echo "Sent."
