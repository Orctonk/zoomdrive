TARGET ?= main
PORT ?= /dev/ttyACM0
SPORT ?= /dev/ttyACM1

DEVICE = atmega328p
MCU = atmega328p
AVRDUDE_DEVICE = m328p

CC := avr-gcc
CFLAGS = -g -Wall -mcall-prologues -mmcu=$(MCU) $(DEVICE_SPECIFIC_CFLAGS) -Os
LDFLAGS = -Wl,-gc-sections -lpololu_$(DEVICE) -Wl,-relax
OBJ2HEX = avr-objcopy 
AVRDUDE = avrdude

OUT_DIR  ?= ./build
SRC_DIRS ?= ./ ../common/
INC_DIRS ?= ./ ../common/ 

TARGET_ELF := $(OUT_DIR)/$(TARGET).elf
TARGET_HEX := $(TARGET_ELF:.elf=.hex)

SRCS := $(shell find $(SRC_DIRS) -name '*.c' 2>&1 | grep -v "Permission denied")
OBJS := $(SRCS:%=$(OUT_DIR)/%.o)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CFLAGS += $(INC_FLAGS)

all: $(TARGET_HEX)

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

$(TARGET_ELF): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(OUT_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

program: $(TARGET_HEX)
	sudo $(AVRDUDE) -p $(AVRDUDE_DEVICE) -c avrisp2 -P $(PORT) -U flash:w:$(TARGET_HEX)

.PHONY: clean

clean:
	$(RM) -r $(OUT_DIR)
	
UART := picocom
BAUD := 4800

uart: 
	sudo $(UART) $(SPORT) -b $(BAUD)
