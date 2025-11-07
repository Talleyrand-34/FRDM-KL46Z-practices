# Compiler and Flags
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
CFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z128VLH4
ASFLAGS = -mthumb -mcpu=cortex-m0plus
LDFLAGS = -O2 -Wall -Wextra -mthumb -mcpu=cortex-m0plus --specs=nosys.specs -Wl,--gc-sections -Tlink.ld

# Targets
TARGET_STANDALONE = main-standalone.elf
TARGET_SHARED = main-shared.elf

# Source Files
C_SRC_STANDALONE = main-standalone.c
C_SRC_SHARED = main-shared.c
ASM_SRC = rev-opt.s

# Object Files
OBJ_STANDALONE = $(C_SRC_STANDALONE:.c=.o)
OBJ_SHARED = $(C_SRC_SHARED:.c=.o) $(ASM_SRC:.s=.o)

# Build All Targets
all: $(TARGET_STANDALONE) $(TARGET_SHARED)

# Compile C Source Files to Object Files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Assemble Assembly Files to Object Files
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

# Link main-standalone (inline ASM)
$(TARGET_STANDALONE): $(OBJ_STANDALONE)
	$(CC) $(LDFLAGS) -Wl,-Map=main-standalone.map $(OBJ_STANDALONE) -o $(TARGET_STANDALONE)
	@echo "Build complete: $(TARGET_STANDALONE)"

# Link main-shared (external ASM)
$(TARGET_SHARED): $(OBJ_SHARED)
	$(CC) $(LDFLAGS) -Wl,-Map=main-shared.map $(OBJ_SHARED) -o $(TARGET_SHARED)
	@echo "Build complete: $(TARGET_SHARED)"

# Flash Standalone Target to Device
flash-standalone: $(TARGET_STANDALONE)
	openocd -f openocd.cfg -c "program $(TARGET_STANDALONE) verify reset exit"

# Flash Shared Target to Device
flash-shared: $(TARGET_SHARED)
	openocd -f openocd.cfg -c "program $(TARGET_SHARED) verify reset exit"

# Clean Build Artifacts
clean:
	rm -f $(OBJ_STANDALONE) $(OBJ_SHARED) $(TARGET_STANDALONE) $(TARGET_SHARED) *.map

# Phony targets
.PHONY: all clean flash-standalone flash-shared
