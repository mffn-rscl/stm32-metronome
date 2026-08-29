TARGET = main

BUILD_DIR = build
SRC_DIR = Src
STARTUP_DIR = Startup

LD_SCRIPT = STM32F103C8T6.ld
MCU_SPEC  = cortex-m3

TOOLCHAIN = /usr
CC = $(TOOLCHAIN)/bin/arm-none-eabi-gcc
AS = $(TOOLCHAIN)/bin/arm-none-eabi-as
LD = $(TOOLCHAIN)/bin/arm-none-eabi-ld
OC = $(TOOLCHAIN)/bin/arm-none-eabi-objcopy
OD = $(TOOLCHAIN)/bin/arm-none-eabi-objdump
OS = $(TOOLCHAIN)/bin/arm-none-eabi-size

INCLUDE = -ICMSIS/Device -ICMSIS/Include

ASFLAGS += -c
ASFLAGS += -O0
ASFLAGS += -mcpu=$(MCU_SPEC)
ASFLAGS += -mthumb
ASFLAGS += -Wall
ASFLAGS += -fmessage-length=0


CFLAGS += -mcpu=$(MCU_SPEC)
CFLAGS += -mthumb
CFLAGS += -Wall
CFLAGS += -g
CFLAGS += -fmessage-length=0
CFLAGS += --specs=nosys.specs

LSCRIPT = ./$(LD_SCRIPT)
LFLAGS += -mcpu=$(MCU_SPEC)
LFLAGS += -mthumb
LFLAGS += -Wall
LFLAGS += --specs=nosys.specs
LFLAGS += -nostdlib
LFLAGS += -lgcc
LFLAGS += -T$(LSCRIPT)

AS_SRC   = $(STARTUP_DIR)/core.S
AS_SRC  += $(STARTUP_DIR)/vector_table.S
C_SRC    = $(SRC_DIR)/main.c
C_SRC		+= $(SRC_DIR)/i2c_driver.c
C_SRC		+= $(SRC_DIR)/rcc_conf.c
C_SRC		+= $(SRC_DIR)/ssd1306.c


OBJS  = $(AS_SRC:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)
OBJS += $(C_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

ELF = $(BUILD_DIR)/$(TARGET).elf
BIN = $(BUILD_DIR)/$(TARGET).bin

.PHONY: all clean

all: $(BIN)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
	$(CC) -x assembler-with-cpp $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(ELF): $(OBJS)
	$(CC) $^ $(LFLAGS) -o $@

$(BIN): $(ELF)
	$(OC) -S -O binary $< $@
	$(OS) $<

asm: $(ELF)
	$(OD) -S $< > $(BUILD_DIR)/$(TARGET).asm

clean:
	rm -rf $(BUILD_DIR)
