# Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Target
TARGET = main

# Directories
FREERTOS_DIR = freertos
FREERTOS_PORT_DIR = $(FREERTOS_DIR)/portable/GCC/ARM_CM0
FREERTOS_MEMMANG_DIR = $(FREERTOS_DIR)/portable/MemMang

# Common flags
COMMON_FLAGS = -mthumb -mcpu=cortex-m0plus -mfloat-abi=soft
COMMON_FLAGS += -DCPU_MKL46Z128VLH4

# Compiler flags
CFLAGS = -c $(COMMON_FLAGS)
CFLAGS += -I. -I./includes
CFLAGS += -I$(FREERTOS_DIR)/include
CFLAGS += -I$(FREERTOS_PORT_DIR)
CFLAGS += -O0 -g -Wall
CFLAGS += -ffunction-sections -fdata-sections

# Linker flags
LDFLAGS = $(COMMON_FLAGS)
LDFLAGS += --specs=nano.specs
LDFLAGS += -Wl,--gc-sections,-Map,$(TARGET).map,-Tlink.ld
LDFLAGS += -lc -lm

# Source files
SRCS = main.c
SRCS += startup.c
SRCS += lcd.c
SRCS += $(FREERTOS_DIR)/list.c
SRCS += $(FREERTOS_DIR)/queue.c
SRCS += $(FREERTOS_DIR)/tasks.c
SRCS += $(FREERTOS_MEMMANG_DIR)/heap_2.c
SRCS += $(FREERTOS_PORT_DIR)/port.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET).elf 
	@echo "Build complete!"

# Link
$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	$(SIZE) $@


# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean
clean:
	rm -f $(OBJS)
	rm -f $(TARGET).elf $(TARGET).map 

flash: $(TARGET).elf
	openocd -f openocd.cfg -c "program $(TARGET).elf verify reset exit"
# Phony targets
.PHONY: all clean

