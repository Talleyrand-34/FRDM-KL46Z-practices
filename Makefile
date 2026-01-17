########################################
# Toolchain y flags
########################################
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS  = -Wall -mthumb -mcpu=cortex-m0plus
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -O2

CPPFLAGS = -I ./includes -DCPU_MKL46Z256VLL4
LDFLAGS  = --specs=nano.specs -Wl,--gc-sections,-Map=main.map,-Tlink.ld
LDLIBS   = -lm

########################################
# Ficheros
########################################
SRCS = main.c startup.c $(wildcard drivers/*.c)
OBJS = $(SRCS:.c=.o)

TARGET_ELF = main.elf
TARGET_BIN = main.bin

########################################
# Reglas
########################################
.PHONY: all flash clean

all: $(TARGET_BIN)

flash: $(TARGET_ELF)
	openocd -f openocd.cfg -c "program $(TARGET_ELF) verify reset exit"

$(TARGET_ELF): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJS) $(TARGET_ELF) $(TARGET_BIN) *.map