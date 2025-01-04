PHONY := all package

CC := arm-vita-eabi-gcc
STRIP := arm-vita-eabi-strip

PROJECT_TITLE := Vita_WoL
PROJECT_TITLEID := VWOL00001

PROJECT := Vita_WoL
CFLAGS += -Wl,-q -I../common

SRC_C := main.c

OBJ_DIRS := $(addprefix out/, $(dir src))
OBJS := $(addprefix out/, $(SRC_C:src/%.c=%.o))

LIBS += -SceIofilemgr_stub -SceIofilemgrForDriver_stub -SceNet_stub -SceSysmodule_stub -SceCommonDialog_stub

all: package

package: $(PROJECT).vpk

$(PROJECT).vpk: eboot.bin param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
	$(PROJECT).vpk

eboot.bin: $(PROJECT).velf
	vita-make-fself $(PROJECT).velf eboot.bin

param.sfo:
	vita-mksfoex -s TITLE_ID="VWOL00001" "Vita_WoL" param.sfo

$(PROJECT).velf: $(PROJECT).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(PROJECT).elf: $(OBJS)
	$(CXX) $^ $(LIBS) -o $@

$(OBJ_DIRS):
	mkdir -p $@
	
out/%.o : src/%.c | $(OBJ_DIRS)
	arm-vita-eabi-gcc -c $(CFLAGS) -o $@ $<
