TARGET = smallHex
OBJS   = main.o system.o draw.o font8.o graphics.o input.o smallhex.o

LIBS = -lc_stub -lSceKernel_stub -lSceDisplay_stub -lSceGxm_stub	\
	-lSceCtrl_stub -lSceTouch_stub

CC      = arm-none-eabi-gcc
READELF = arm-none-eabi-readelf
OBJDUMP = arm-none-eabi-objdump
CFLAGS  = -Wall -specs=psp2.specs -D PLATFORM_PSP2
ASFLAGS = $(CFLAGS)

all: $(TARGET)_fixup.elf

%_fixup.elf: %.elf
	psp2-fixup -q -S $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET)_fixup.elf $(TARGET).elf $(OBJS)

