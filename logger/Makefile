# Change these to reflect your project
TARGET ?= main
PORT ?= /dev/ttyACM0
SPORT ?= /dev/ttyUSB0

# Change UART settings
UART := picocom
BAUD := 4800

# Change these to reflect your device
DEVICE = atmega328p
MCU = atmega328p
AVRDUDE_DEVICE = m328p

# Define project structure
OUT_DIR  ?= ./build/logger
SRC_DIRS ?= ./src ../common/
INC_DIRS ?= ./ ../common/

# Toolchain
CC := avr-gcc
OBJ2HEX = avr-objcopy 
AVRDUDE = avrdude
PROGRAMMER = avrisp2

# Remove DEBUG define to compile in release mode
DEBUG = true

# C compiler/linker options
CSTANDARD = --std=gnu99
CTUNING = function-sections data-sections unsigned-char unsigned-bitfields pack-struct short-enums
CWARN = all strict-prototypes
OPTIMIZE = -Os
CFLAGS = -mcall-prologues -mmcu=$(MCU)
LDFLAGS = -Wl,-gc-sections -lpololu_$(DEVICE) -Wl,-relax -Os
DEFINES = F_CPU=8000000UL DRV_MMC=0

# From this point nothing should be changed
ifeq ($(DEBUG) , true)
	CFLAGS += -g
	DEFINES += DEBUG
endif

TARGET_ELF := $(OUT_DIR)/$(TARGET).elf
TARGET_HEX := $(TARGET_ELF:.elf=.hex)

SRCS := $(shell find $(SRC_DIRS) -name '*.c' 2>&1 | grep -v "Permission denied")
OBJS = $(SRCS:%=$(OUT_DIR)/%.o)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CFLAGS += $(CSTANDARD)
CFLAGS += $(addprefix -f,$(CTUNING))
CFLAGS += $(addprefix -W,$(CWARN))
CFLAGS += $(INC_FLAGS)
CFLAGS += $(addprefix -D,$(DEFINES))

all: $(TARGET_HEX)

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

$(TARGET_ELF): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(OUT_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

program: $(TARGET_HEX)
	$(AVRDUDE) -p $(AVRDUDE_DEVICE) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$(TARGET_HEX)

.PHONY: clean

clean:
	$(RM) -r $(OUT_DIR)

uart: 
	$(UART) $(SPORT) -b $(BAUD)