########################################
# Toolchain e flags
########################################
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS  = -Wall -mthumb -mcpu=cortex-m0plus
CFLAGS += -ffunction-sections -fdata-sections
CPPFLAGS = -I ./includes -I ./drivers -DCPU_MKL46Z256VLL4
LDFLAGS  = --specs=nano.specs -Wl,--gc-sections,-Map=main.map,-Tlink.ld

########################################
# Flags de optimización
########################################
CFLAGS_O0    = $(CFLAGS) -O0
CFLAGS_OFAST = $(CFLAGS) -Ofast
CFLAGS_O2    = $(CFLAGS) -O2

########################################
# Ficheiros comuns
########################################
DRIVER_SRCS = $(wildcard drivers/*.c)
DRIVER_OBJS = $(DRIVER_SRCS:.c=.o)

########################################
# Ficheiros ASM
########################################
ASM_SRCS = $(wildcard reverse*.s)
ASM_OBJS = $(ASM_SRCS:.s=.o)

########################################
# Ficheiros reverse con optimizaciones específicas
########################################
REVERSE_O0_SRCS    = reverse1.c reverse4.c
REVERSE_OFAST_SRCS = reverse5.c reverse6.c

REVERSE_O0_OBJS    = $(REVERSE_O0_SRCS:.c=.o)
REVERSE_OFAST_OBJS = $(REVERSE_OFAST_SRCS:.c=.o)

########################################
# Projecto único: main
########################################
MAIN_SRCS = main.c startup.c
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

ALL_OBJS = $(MAIN_OBJS) $(DRIVER_OBJS) $(ASM_OBJS) $(REVERSE_O0_OBJS) $(REVERSE_OFAST_OBJS)

MAIN_ELF  = main.elf
MAIN_BIN  = main.bin

########################################
# Regras principais
########################################
.PHONY: all flash clean

all: $(MAIN_ELF)

flash: $(MAIN_ELF)
	openocd -f openocd.cfg -c "program $(MAIN_ELF) verify reset exit"

# Compilación y linkado
$(MAIN_ELF): $(ALL_OBJS)
	$(CC) $(CFLAGS_O2) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

########################################
# Reglas específicas para reverse con -O0
########################################
reverse1.o: reverse1.c
	$(CC) $(CPPFLAGS) $(CFLAGS_O0) -c $< -o $@

reverse4.o: reverse4.c
	$(CC) $(CPPFLAGS) $(CFLAGS_O0) -c $< -o $@

########################################
# Reglas específicas para reverse con -Ofast
########################################
reverse5.o: reverse5.c
	$(CC) $(CPPFLAGS) $(CFLAGS_OFAST) -c $< -o $@

reverse6.o: reverse6.c
	$(CC) $(CPPFLAGS) $(CFLAGS_OFAST) -c $< -o $@

########################################
# Reglas generales
########################################
# C (resto de archivos con -O2)
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS_O2) -c $< -o $@

# ASM
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

# Xeración do binario
%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

########################################
# Limpeza
########################################
clean:
	rm -f *.o drivers/*.o *.elf *.bin *.map
