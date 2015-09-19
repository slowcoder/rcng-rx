#CROSS ?= /opt/Xarm-2013.05/bin/arm-none-eabi
CROSS ?= /opt/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi
CC := $(CROSS)-gcc

CFLAGS := -g -Os -Wall -mlittle-endian -mthumb -mcpu=cortex-m0 -mthumb-interwork
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -std=gnu99
CFLAGS += -DUSE_HW_SPI
CFLAGS += -DGIT_BASE_REVISION=\"`git rev-parse --short HEAD`\"
LDFLAGS := -nostdlib -nodefaultlibs -nostartfiles -Wl,-TSTM32F030C8_FLASH.ld
#LDFLAGS +=  -Wl,--print-gc-sections
LDFLAGS += -g -Xlinker --gc-sections

#CFLAGS += -fomit-frame-pointer

CFLAGS += -Iinc/ -DUSE_STDPERIPH_DRIVER

CFLAGS += -DSTM32F030xC -DSTM32F030

OBJS := \
	src/startup_stm32f030.o \
	src/system_stm32f0xx.o \
	src/main.o \
	src/freertos.o \
	src/uart.o \
	src/servo.o \
	src/leds.o \
	src/cli/cli.o \
	src/cli/cmd_free.o \
	src/cli/cmd_info.o \
	src/cli/cmd_ps.o \
	src/cli/cmd_uname.o \
	src/syscalls.o \
	../common/src/log.o


include Makefile.stdperiph
include Makefile.mw

ALL_DEPS := $(patsubst %.o,%.d,$(OBJS))

all: rcng_rx.bin

clean:
	rm -f rcng_rx.bin rcng_rx.elf rcng_rx.map $(OBJS) $(ALL_DEPS)

debug: rcng_rx.elf
	openocd -f scripts/stlink-v2.cfg -f scripts/stm32f0x.cfg &
	$(CROSS)-gdb $<
	killall openocd

newflash: rcng_rx.bin rcng_rx.elf
	openocd -f scripts/stlink-v2.cfg -f scripts/stm32f0x.cfg \
		-c init \
		-c "reset halt" \
		-c "reset init" \
		-c "flash write_image erase rcng_rx.elf" \
		-c "verify_image rcng_rx.elf" \
		-c "reset run" \
		-c shutdown

flash: rcng_rx.bin
	~/bin/stlink/st-flash write $< 0x08000000

%.bin:%.elf
	@$(CROSS)-objcopy -O binary $< $@
	@echo "Firmware size: "
	@du -hs $@

rcng_rx.elf: $(OBJS)
	@echo "LD $@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Wl,-Map=rcng_rx.map

%.o:%.c
	@echo "CC $<"
	@$(CC) -MM -MF $(subst .o,.d,$@) -MP -MT "$@ $(subst .o,.d,$@)" $(CFLAGS) $<
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o:%.s
	@echo "CC $<"
	@$(CC) -MM -MF $(subst .o,.d,$@) -MP -MT "$@ $(subst .o,.d,$@)" $(CFLAGS) $<
	@$(CC) $(CFLAGS) -c -o $@ $<	

ifneq ("$(MAKECMDGOALS)","clean")
-include $(ALL_DEPS)
endif
