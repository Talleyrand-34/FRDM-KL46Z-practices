# Compiler and Flags
CC = arm-none-eabi-gcc
CFLAGS = -I./includes -I./drivers -O2 -Wall -mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z128VLH4
LDFLAGS = -O2 -Wall -Wextra -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=output.map -Tlink.ld

# Source Files
SRC = main.c startup.c lcd.c aux.c
OBJ = $(SRC:.c=.o)

# Target
TARGET = main.elf

# Build Target
all: $(TARGET)

# Compile Source Files to Object Files
compile: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link Object Files to Create Executable
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET)

# Flash Target to Device (Optional)
flash: $(TARGET)
	openocd -f openocd.cfg -c "program $(TARGET) verify reset exit"

# Clean Build Artifacts
clean:
	rm -f $(OBJ) $(TARGET) output.map
